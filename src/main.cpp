// Copyright (c) 2025 Daniel Cliche
// SPDX-License-Identifier: BSD-3-Clause

#include <SDL.h>

#include "voodoo.h"
#include "voodoo_emu.h"
#include "voodoo_def.h"

SDL_Renderer *g_renderer;

static UINT32 extract_bits(UINT32 v, int msb, int lsb) {
    UINT32 m = 0xffffffff >> (31 - (msb - lsb));
    return (v >> lsb) & m;
}

static void print_status() {
    UINT32 v = voodoo_r(status);
    printf("PCI FIFO freespace: %x\n", extract_bits(v, 5, 0));
    printf("Vertical retrace: %x\n", extract_bits(v, 6, 6));
    printf("FBI graphics engine busy: %x\n", extract_bits(v, 7, 7));
    printf("TREX busy: %x\n", extract_bits(v, 8, 8));
    printf("SST-1 busy: %x\n", extract_bits(v, 9, 9));
    printf("Displayed buffer: %x\n", extract_bits(v, 11, 10));
    printf("Memory FIFO freespace: %x\n", extract_bits(v, 27, 12));
    printf("Swap buffers pending: %x\n", extract_bits(v, 30, 28));
    printf("PCI interrupt generated: %x\n", extract_bits(v, 31, 31));
}

static void voodoo_wf(UINT32 offset, float value) {
    union { float f; int i; } data;
    data.f = value;
    voodoo_w(offset, data.i, 0xffffffff);
}

static float edge(float x, float y, float xi, float yi, float dXi, float dYi) {
    return (x - xi) * dYi - (y - yi) * dXi;
}

// Ref.: https://fryzekconcepts.com/notes/rasterizing-triangles.html
static void draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2, float v0[3], float v1[3], float v2[3]) {
    float dX0 = x0 - x2;
    float dX1 = x1 - x0;
    float dX2 = x2 - x1;
    float dY0 = y0 - y2;
    float dY1 = y1 - y0;
    float dY2 = y2 - y1;
    float start_x = x0;
    float start_y = y0;
    float w0 = edge(start_x, start_y, x0, y0, dX0, dY0);
    float w1 = edge(start_x, start_y, x1, y1, dX1, dY1);
    float w2 = edge(start_x, start_y, x2, y2, dX2, dY2);
    float area = w0 + w1 + w2;
    float dXV[3], dYV[3];
    float v[3];
    for (int i = 0; i < 3; ++i) {
        dXV[i] = (dY0 * v1[i] + dY1 * v2[i] + dY2 * v0[i]) / area;
        dYV[i] = -(dX0 * v1[i] + dX1 * v2[i] + dX2 * v0[i]) / area;
        v[i] = (w0 * v1[i] + w1 * v2[i] + w2 * v0[i]) / area;
    }

    voodoo_wf(fvertexAx, x0);
    voodoo_wf(fvertexAy, y0);
    voodoo_wf(fvertexBx, x1);
    voodoo_wf(fvertexBy, y1);
    voodoo_wf(fvertexCx, x2);
    voodoo_wf(fvertexCy, y2);

    voodoo_wf(fstartR, v[0]);
    voodoo_wf(fdRdX, dXV[0]);
    voodoo_wf(fdRdY, dYV[0]);
    voodoo_wf(fstartG, v[1]);
    voodoo_wf(fdGdX, dXV[1]);
    voodoo_wf(fdGdY, dYV[1]);
    voodoo_wf(fstartB, v[2]);
    voodoo_wf(fdBdX, dXV[2]);
    voodoo_wf(fdBdY, dYV[2]);

    voodoo_w(ftriangleCMD, 0x80000000, 0xffffffff);
}

int main() {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Voodoo", SDL_WINDOWPOS_CENTERED_DISPLAY(1), SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

    g_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    Voodoo_Initialize();

    print_status();

    // 10 tiles of 64 pixels
    voodoo_w(fbiInit1, 10 << 4, 0xffffffff);
    voodoo_w(hSync, (800 - 96 - 1) << 16 | (96 - 1), 0xffffffff);
    voodoo_w(vSync, (525 - 2) << 16 | 2, 0xffffffff);
    voodoo_w(backPorch, 33 << 16 | 48, 0xffffffff);
    voodoo_w(videoDimensions, 480 << 16 | 640, 0xffffffff);

    voodoo_w(clipLeftRight, (0 << 16) | (640 << 0), 0xffffffff);
    voodoo_w(clipLowYHighY, (0 << 16) | (480 << 0), 0xffffffff);

    float red[3] = {255.0f, 0.0f, 0.0f};
    float green[3] = {0.0f, 255.0f, 0.0f};
    float blue[3] = {0.0f, 0.0f, 255.0f};

    float a[2] = {0.0f, 0.5f};
    float b[2] = {-0.5f, -0.5f};
    float c[2] = {0.5f, -0.5f};

    float angle = 1.0f;
    float scale = 100.0f;

    float t[2] = {200.0f, 200.0f};

    bool quit = false;
    while(!quit) {

        // RGB write back buffer
        voodoo_w(fbzMode, (1 << 14) | (1 << 9), 0xffffffff);
        // color1 for rgb
        voodoo_w(fbzColorPath, (2 << 0), 0xffffffff);
        voodoo_w(color1, 0xff000000, 0xffffffff);
        // Clear the RGB buffer
        voodoo_w(fastfillCMD, 0, 0xffffffff);

        // RGB write back buffer + dithering
        voodoo_w(fbzMode, (1 << 14) | (1 << 9) | (1 << 8), 0xffffffff);
        // gouraud shading with subpixel correction
        voodoo_w(fbzColorPath, (1 << 26) | (0 << 0), 0xffffffff);

        draw_triangle(
            (a[0] + cosf(angle)) * scale + t[0], (a[1] + sinf(angle)) * scale + t[1],
            (b[0] + cosf(angle)) * scale + t[0], (b[1] + sinf(angle)) * scale + t[1],
            (c[0] + cosf(angle)) * scale + t[0], (c[1] + sinf(angle)) * scale + t[1],
            red, green, blue);

        // Swap buffers
        voodoo_w(swapbufferCMD, 0, 0xffffffff);

        SDL_RenderPresent(g_renderer);

        angle += 0.01f;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            quit = true;
                            break;
                    }
                    break;
            }
        }
    }

    Voodoo_Shut_Down();

    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}