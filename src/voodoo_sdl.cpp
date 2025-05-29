#include "voodoo_sdl.h"

#include <SDL.h>

#include <cstdio>

extern SDL_Renderer *g_renderer;

bool voodoo_sdl_init(voodoo_state *v){
    return true;
}

void voodoo_sdl_leave(bool leavemode) {
}

void voodoo_sdl_swap_buffer() {
}

void voodoo_sdl_set_window(voodoo_state *v) {
}

void voodoo_sdl_clip_window(voodoo_state *v) {
}

void voodoo_sdl_draw_pixel(int x, int y, bool has_rgb, bool has_alpha, int r, int g, int b, int a) {
    // Constants taken from https://stackoverflow.com/a/9069480
    Uint8 rr = has_rgb ? (r * 527 + 23) >> 6 : 0;
    Uint8 gg = has_rgb ? (g * 259 + 33) >> 6 : 0;
    Uint8 bb = has_rgb ? (b * 527 + 23) >> 6 : 0;
    SDL_SetRenderDrawColor(g_renderer, rr, gg, bb, has_alpha ? a : 255);
    SDL_RenderDrawPoint(g_renderer, x, y);
}

void voodoo_sdl_draw_pixel_pipeline(int x, int y, int r, int g, int b) {
}

UINT32 voodoo_sdl_read_pixel(int x, int y) {
    return 0;
}

void voodoo_sdl_texture_clear(UINT32 texbase, int TMU) {
}

void voodoo_sdl_shutdown(voodoo_state *v) {
}

void voodoo_sdl_fastfill(void) {
}

void voodoo_sdl_invalidate_paltex(void) {
}

void voodoo_sdl_draw_triangle(poly_extra_data *extra) {
}

void voodoo_sdl_clear(void) {
    SDL_RenderClear(g_renderer);
}

void voodoo_sdl_vblank_flush(void) {
}

void voodoo_sdl_draw_buffer(voodoo_state *v) {
    UINT16 *drawbuf = (UINT16 *)(v->fbi.ram + v->fbi.rgboffs[v->fbi.backbuf]);
    for (int y = 0; y < v->fbi.height; y++)
        for (int x = 0; x < v->fbi.width; x++) {
            Uint8 r8 = ( ((*drawbuf >> 11) & 0x1F) * 527 + 23 ) >> 6;
            Uint8 g8 = ( ((*drawbuf >> 5) & 0x3F) * 259 + 33 ) >> 6;
            Uint8 b8 = ( (*drawbuf & 0x1F) * 527 + 23 ) >> 6;
            SDL_SetRenderDrawColor(g_renderer, r8, g8, b8, 255);
            SDL_RenderDrawPoint(g_renderer, x, y);
            drawbuf++;
        }
}