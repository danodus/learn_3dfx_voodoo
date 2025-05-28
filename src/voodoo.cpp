#include "voodoo.h"
#include "voodoo_emu.h"

#include <cstdio>

extern voodoo_state *v;

void Voodoo_UpdateScreenStart() {
    v->active = true;
}

void Voodoo_Initialize() {
    v = new voodoo_state;
    v->ogl = false;

    voodoo_init(VOODOO_1);
}

void Voodoo_Shut_Down() {

    voodoo_shutdown();

    if (v != NULL) {
        delete v;
        v = NULL;
    }
}

void Voodoo_Output_Enable(bool enabled) {
}

bool Voodoo_GetRetrace() {
    return false;
}

double Voodoo_GetVRetracePosition() {
    return 0;
}

double Voodoo_GetHRetracePosition() {
    return 0;
}
