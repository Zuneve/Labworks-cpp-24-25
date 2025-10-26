#pragma once

#include <fstream>

#include "MyStructs.h"

const uint8_t kBMPHeadersize = 14;
const uint8_t kBMPHeaderInfosize = 40;
const uint8_t kColorTablesize = 64;

struct BMPHeader {
    uint16_t signature = 0x4D42;
    uint32_t file_size = 0;
    uint32_t reserved = 0;
    uint32_t data_offset = 0;
};

struct BMPInfoHeader {
    uint32_t size = 40;
    uint32_t width = 0;
    uint32_t height = 0;
    uint16_t planes = 1;
    uint16_t bits_per_pixel = 4;
    uint32_t compression = 0;
    uint32_t image_size = 0;
    uint32_t x_pixels_per_metr = 0;
    uint32_t y_pixels_per_metr = 0;
    uint32_t colors_used = 16;
    uint32_t important_colors = 0;
};

void CreateBMPImage(Arguments* args, int iteration);
