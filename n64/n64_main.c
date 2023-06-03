#include <stdio.h>

#include "game_main.h"
#include "unfnetwork.h"
#include <libdragon.h>

#include <stdarg.h>
#include <stdio.h>

surface_t *n64_disp;

void Plat_Print(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void Plat_PlaceChar(char c, int x, int y, gcolor_t bg, gcolor_t fg)
{
    graphics_set_color(graphics_make_color(fg.r, fg.g, fg.b, fg.a),
                       graphics_make_color(bg.r, bg.g, bg.b, bg.a));

    graphics_draw_character(n64_disp, x, y, c);
}

void Plat_BeginFrame(void)
{
    n64_disp = display_get();
    memset(n64_disp->buffer, 0, 2 * (n64_disp->width * n64_disp->height));
}
void Plat_EndFrame(void)
{
    display_show(n64_disp);
}

void Plat_SendEvents(void)
{
    controller_scan();

    //struct controller_data keys_pressed = get_keys_down();
     struct controller_data keys_pressed = get_keys_held();
    // struct controller_data keys_pressed = get_keys_up();

    struct SI_condat pressed = keys_pressed.c[0];
    if (pressed.up)
        game_bflags |= GKEYPRESS_UP;
    if (pressed.down)
        game_bflags |= GKEYPRESS_DOWN;
    if (pressed.left)
        game_bflags |= GKEYPRESS_LEFT;
    if (pressed.right)
        game_bflags |= GKEYPRESS_RIGHT;
}

void Plat_NetSync(void)
{
    int header;
    network_udp_send_data(&g_us, sizeof(g_us));
    while ((header = usb_poll()))
    {
        int type = USBHEADER_GETTYPE(header);
        int size = USBHEADER_GETSIZE(header);
        gnetstate_t pl;
        usb_read(&pl, size);

        switch (type)
        {
        case NETTYPE_UDP_SEND:
            g_them[pl.player] = pl;
            break;
        default:
            break;
        }
    }
}

int main(int argc, char **argv)
{
    console_init();
    debug_init_isviewer();

    network_initialize();
    controller_init();

    Game_Init(argc, argv);

#if GPLAYER_INDEX
    char h[512] = {0};
    sprintf(h, "localhost:%d", 42069);
    network_udp_connect(h);
#else
    network_udp_start_server("42069");
#endif
    num_players = 2;

    int connected = 0;
    int ready = 0;
    while (!connected || !ready)
    {
        int header;

        if (!connected)
            network_udp_send_data(&g_us, sizeof(g_us));
        if ((header = usb_poll()))
        {
            int type = USBHEADER_GETTYPE(header);
            int size = USBHEADER_GETSIZE(header);
            uint8_t buffer[size];
            usb_read(buffer, size);

            switch (type)
            {
            case NETTYPE_UDP_SEND:
                if (!connected)
                {
                    connected = 1;
                    network_udp_send_data("!", 1);
                    printf("Connected to player %d!\n", ((gnetstate_t*)(buffer))->player + 1);
                }
                else if (buffer[0] == '!')
                {
                    ready = 1;
                    printf("Ready!\n");
                    network_udp_send_data("!", 1);
                }
                break;
            default:
                break;
            }
        }
    }

    console_clear();
    console_close();

    game_viewport = (grect16_t){0, 0, 320, 240};

    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);

    return Game_MainLoop();
}
