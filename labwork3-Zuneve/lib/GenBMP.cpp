#include <cstdio>

#include "GenBMP.h"
#include "MyStructs.h"

const uint8_t ColorTable[16][4]{
    {255, 255, 255, 0}, // белый 0
    {0, 255, 0, 0}, // зеленый 1
    {255, 0, 255, 0}, // фиолетовый 2
    {0, 255, 255, 0}, // желтый 3
    {0, 0, 0, 0}, // черный 4
};

void WriteUint16(std::ofstream& out, uint16_t value) {
    out.put(static_cast<char>(value & 255));
    out.put(static_cast<char>((value >> 8) & 255));
}

void WriteUint32(std::ofstream& out, uint32_t value) {
    out.put(static_cast<char>(value & 255));
    out.put(static_cast<char>((value >> 8) & 255));
    out.put(static_cast<char>((value >> 16) & 255));
    out.put(static_cast<char>((value >> 24) & 255));
}

void WriteBMPHeader(std::ofstream& BMPImage, BMPHeader& header) {
    WriteUint16(BMPImage, header.signature);
    WriteUint32(BMPImage, header.file_size);
    WriteUint32(BMPImage, header.reserved);
    WriteUint32(BMPImage, header.data_offset);
}

void WriteBMPHeaderInfo(std::ofstream& BMPImage, BMPInfoHeader& info) {
    WriteUint32(BMPImage, info.size);
    WriteUint32(BMPImage, info.width);
    WriteUint32(BMPImage, info.height);
    WriteUint16(BMPImage, info.planes);
    WriteUint16(BMPImage, info.bits_per_pixel);
    WriteUint32(BMPImage, info.compression);
    WriteUint32(BMPImage, info.image_size);
    WriteUint32(BMPImage, info.x_pixels_per_metr);
    WriteUint32(BMPImage, info.y_pixels_per_metr);
    WriteUint32(BMPImage, info.colors_used);
    WriteUint32(BMPImage, info.important_colors);
}

void WriteColorTable(std::ofstream& BMPImage) {
    for (int i = 0; i < 16; ++i) {
        BMPImage.put(static_cast<char>(ColorTable[i][0]));
        BMPImage.put(static_cast<char>(ColorTable[i][1]));
        BMPImage.put(static_cast<char>(ColorTable[i][2]));
        BMPImage.put(static_cast<char>(ColorTable[i][3]));
    }
}

void GenerateBMPHeaders(std::ofstream& BMPImage, Arguments* args) {
    BMPHeader header;
    BMPInfoHeader info;

    info.width = args->mx_len - args->mn_len + 1;
    info.height = args->mx_high - args->mn_high + 1;

    int32_t cnt_pixels = (info.width + 1) / 2;
    int8_t padding_bytes = (4 - (cnt_pixels % 4)) % 4;

    info.image_size = (cnt_pixels + padding_bytes) * info.height;
    header.data_offset = kColorTablesize + kBMPHeadersize + kBMPHeaderInfosize;
    header.file_size = header.data_offset + info.image_size;

    WriteBMPHeader(BMPImage, header);

    WriteBMPHeaderInfo(BMPImage, info);

    WriteColorTable(BMPImage);
}


void CreateBMPImage(Arguments* args, int iteration) {
    const int bufferSize = 512;
    char* filepath = new char[bufferSize];
    snprintf(filepath, bufferSize, "%s/output%d.bmp", args->output_file, iteration);
    std::ofstream BMPImage(filepath, std::ios_base::binary);
    if (!BMPImage) {
        std::cerr << "Error opening output file!\n";
        exit(EXIT_FAILURE);
    }
    int32_t width = args->mx_len - args->mn_len + 1;
    int32_t cnt_pixels = (width + 1) / 2;
    int8_t padding_bytes = static_cast<int8_t>((4 - (cnt_pixels % 4)) % 4);

    GenerateBMPHeaders(BMPImage, args);

    Node2D* new_node;
    Node2D* first_in_col = args->first_node;
    while (first_in_col->down) {
        first_in_col = first_in_col->down;
    }
    for (int32_t y = args->mx_high; y >= args->mn_high; y--) {
        new_node = first_in_col;
        for (int32_t x = args->mn_len; x <= args->mx_len; x += 2) {
            uint8_t byte = 0;
            uint8_t color1 = new_node->value % 5;
            byte |= color1 << 4;
            if (x + 1 <= args->mx_len) {
                uint8_t color2 = new_node->right->value % 5;
                byte |= color2;
                new_node = new_node->right;
            }
            BMPImage.put(static_cast<char>(byte));
            new_node = new_node->right;
        }
        first_in_col = first_in_col->up;
        for (int p = 0; p < padding_bytes; p++) {
            BMPImage.put(0);
        }
    }

    BMPImage.close();
}
