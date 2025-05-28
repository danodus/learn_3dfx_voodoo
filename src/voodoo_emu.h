#ifndef VOODOO_EMU_H
#define VOODOO_EMU_H

#include "voodoo_types.h"
#include "voodoo_data.h"

void voodoo_w(UINT32 offset, UINT32 data, UINT32 mask);
UINT32 voodoo_r(UINT32 offset);

void voodoo_init(int type);
void voodoo_shutdown();
void voodoo_leave(void);

void voodoo_activate(void);
void voodoo_update_dimensions(void);
void voodoo_set_window(void);

void voodoo_vblank_flush(void);
void voodoo_swap_buffers(voodoo_state *v);

#endif
