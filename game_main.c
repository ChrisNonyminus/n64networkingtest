#include "game_main.h"

grect16_t game_viewport;
gentity_t game_players[16];
int num_players;
int player_idx;
uint32_t game_bflags;
gnetstate_t g_us;       // our net state we send
gnetstate_t g_them[16]; // their net states we recieve (the entity at our
                        // index is blank)

static gcolor_t player_colors[16] = {
    [0] = (gcolor_t){255, 0, 0, 255},
    [1] = (gcolor_t){0, 0, 255, 255}
};

void Game_Init(int argc, char **argv)
{
    player_idx = GPLAYER_INDEX;
    for (int i = 0; i < 16; i++)
    {
        game_players[i].rep_color = player_colors[i];
        float negx = i > 8 ? -160 : 160;
        float negy = i > 8 ? -120 : 120;
        game_players[i].position = (gpos16_t){((float)i / 16) * negx, negy};
        game_players[i].rep_char = i + '1';
    }
    g_us = (gnetstate_t){player_idx, game_players[player_idx]};
}
int Game_MainLoop(void)
{
    while (1)
    {
        game_bflags = 0;
        Plat_BeginFrame();
        Plat_SendEvents();

        if (game_bflags & GKEYPRESS_UP)
        {
            gentity_t *you = &game_players[player_idx];
            you->position.y -= 4;
            if (you->position.y == -160)
                you->position.y = 160;
        }

        if (game_bflags & GKEYPRESS_DOWN)
        {
            gentity_t *you = &game_players[player_idx];
            you->position.y += 4;
            if (you->position.y == 160)
                you->position.y = -160;
        }

        if (game_bflags & GKEYPRESS_RIGHT)
        {
            gentity_t *you = &game_players[player_idx];
            you->position.x += 4;
            if (you->position.x == 120)
                you->position.x = -120;
        }

        if (game_bflags & GKEYPRESS_LEFT)
        {
            gentity_t *you = &game_players[player_idx];
            you->position.x -= 4;
            if (you->position.x == -120)
                you->position.x = 120;
        }

        g_us.player_state = game_players[player_idx];

        Plat_NetSync();

        for (int i = 0; i < num_players; i++)
        {
            if (i != player_idx)
            {
                game_players[i] = g_them[i].player_state;
            }
            gpos16_t spos = Game_GetScreenPos(game_players[i].position);
            Plat_PlaceChar(game_players[i].rep_char, spos.x, spos.y,
                           (gcolor_t){0, 0, 0, 0}, game_players[i].rep_color);
        }

        Plat_EndFrame();
    }
    return 0;
}

gpos16_t Game_GetScreenPos(gpos16_t worldpos)
{
    assert(game_viewport.w > 0 && game_viewport.h > 0);
    gpos16_t ret = {
        // ((int)worldpos.x + 32767) * ((float)game_viewport.w / 65535),
        // ((int)worldpos.y + 32767) * ((float)game_viewport.h / 65535)
        (worldpos.x + 160),
        worldpos.y + 120
        };
    return ret;
}
