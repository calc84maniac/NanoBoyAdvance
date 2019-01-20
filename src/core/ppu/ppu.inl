/*
 * Copyright (C) 2018 Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

auto ReadPalette(int palette, int index) -> std::uint16_t {
    int cell = (palette * 32) + (index * 2);

    /* TODO: On Little-Endian devices we can get away with casting to uint16_t*. */
    return (pram[cell + 1] << 8) |
            pram[cell + 0];
}

void DecodeTileLine4BPP(std::uint16_t* buffer, std::uint32_t base, int palette, int number, int y, bool flip) {
    std::uint8_t* data = &vram[base + (number * 32) + (y * 4)];

    if (flip) {
        for (int x = 0; x < 4; x++) {
            int d  = *data++;
            int p1 = d & 15;
            int p2 = d >> 4;

            buffer[(x*2+0)^7] = p1 ? ReadPalette(palette, p1) : s_color_transparent;
            buffer[(x*2+1)^7] = p2 ? ReadPalette(palette, p2) : s_color_transparent;
        }
    } else {
        for (int x = 0; x < 4; x++) {
            int d  = *data++;
            int p1 = d & 15;
            int p2 = d >> 4;

            buffer[x*2+0] = p1 ? ReadPalette(palette, p1) : s_color_transparent;
            buffer[x*2+1] = p2 ? ReadPalette(palette, p2) : s_color_transparent;
        }
    }
}

void DecodeTileLine8BPP(std::uint16_t* buffer, std::uint32_t base, int number, int y, bool flip) {
    std::uint8_t* data = &vram[base + (number * 64) + (y * 8)];

    if (flip) {
        for (int x = 7; x >= 0; x--) {
            int pixel = *data++;
            buffer[x] = pixel ? ReadPalette(0, pixel) : s_color_transparent;
        }
    } else {
        for (int x = 0; x < 8; x++) {
            int pixel = *data++;
            buffer[x] = pixel ? ReadPalette(0, pixel) : s_color_transparent;
        }
    }
}

void DrawPixel(int x, int layer, int priority, std::uint16_t color) {
    if (color != s_color_transparent && priority <= this->priority[x]) {
        pixel[1][x] = pixel[0][x];
        pixel[0][x] = color;
        this->layer[1][x] = this->layer[0][x];
        this->layer[0][x] = layer;
        this->priority[x] = priority;
    }
}
