#ifndef VOODOO_SDL_H
#define VOODOO_SDL_H

#include "voodoo_data.h"

bool voodoo_sdl_init(voodoo_state *v);
void voodoo_sdl_leave(bool leavemode);

void voodoo_sdl_swap_buffer();
void voodoo_sdl_set_window(voodoo_state *v);
void voodoo_sdl_clip_window(voodoo_state *v);
void voodoo_sdl_draw_pixel(int x, int y, bool has_rgb, bool has_alpha, int r, int g, int b, int a);
void voodoo_sdl_draw_pixel_pipeline(int x, int y, int r, int g, int b);
UINT32 voodoo_sdl_read_pixel(int x, int y);
void voodoo_sdl_texture_clear(UINT32 texbase, int TMU);
void voodoo_sdl_shutdown(voodoo_state *v);
void voodoo_sdl_fastfill(void);
void voodoo_sdl_invalidate_paltex(void);
void voodoo_sdl_draw_triangle(poly_extra_data *extra);
void voodoo_sdl_clear(void);
void voodoo_sdl_vblank_flush(void);

void voodoo_sdl_draw_buffer(voodoo_state *v);

#endif
