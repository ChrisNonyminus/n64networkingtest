#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>

// here are the struct defs
typedef struct gcolor
{                       // a color
    uint8_t r, g, b, a; // red, green, blue, alpha (0-255)
} gcolor_t;
typedef struct gpos16
{               // a vec2s position
    short x, y; // x and y coordinates
} gpos16_t;
typedef struct grect16
{ // a vec4s rect
    short x, y, w, h;
} grect16_t;
typedef struct gentity
{                  // a game entity
    char rep_char; // the text character to graphically represent this entity
    gcolor_t rep_color; // the color of this entity
    gpos16_t position;  // where the entity is
    int player_index;   // which player is this entity?
} gentity_t;
typedef struct gnetstate
{                           // net buffer
    int player;             // which player the buffer is from
    gentity_t player_state; // the player state reported by the player's console
} gnetstate_t;

// here are the global variables
extern grect16_t game_viewport;    // the screen
extern gentity_t game_players[16]; // the players (up to 16 via udp connection);
extern int num_players;            // number of players currently in the game
extern int player_idx;             // which player index you are
#define GKEYPRESS_UP 1 << 1        // move player up
#define GKEYPRESS_DOWN 1 << 2      // move player down
#define GKEYPRESS_LEFT 1 << 3      // move player left
#define GKEYPRESS_RIGHT 1 << 4     // move player right
extern uint32_t game_bflags;       // button flags
extern gnetstate_t g_us;           // our net state we send
extern gnetstate_t g_them[16]; // their net states we recieve (the entity at our
                               // index is blank)

// here are the platform-agnostic functions
void Game_Init(int argc, char **argv); // initializes the game
int Game_MainLoop(void);               // main loop
gpos16_t Game_GetScreenPos(
    gpos16_t worldpos); // gets a screen coordinate from a worldspace coordinate

// here are the functions each platform implementation needs to define
void Plat_Print(char *fmt, ...); // vprintf a message
void Plat_PlaceChar(char c, int x, int y, gcolor_t bg,
                    gcolor_t fg); // draw a text character
void Plat_BeginFrame(void);       // initialize and clear a framebuffer
void Plat_EndFrame(void);         // display and close a framebuffer
void Plat_SendEvents(
    void); // take in the platform's events (i.e user interactions) and convert
           // them to this game's format and push them to the game
void Plat_NetSync(void); // send and recieve data


#endif
