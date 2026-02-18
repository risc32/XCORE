
#define STB_IMAGE_IMPLEMENTATION
#include <cstdint>

#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <vector>


bool png_to_int_array(const char* filename,
                      std::vector<uint32_t>& pixels,
                      int& width, int& height) {


    int channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);

    if (!data) {
        std::cerr << "Ошибка загрузки: " << stbi_failure_reason() << std::endl;
        return false;
    }


    pixels.resize(width * height);

    for (int i = 0; i < width * height; ++i) {
        unsigned char* pixel = &data[i * 4];







        uint32_t color = 0x00000000 |
                         (pixel[2] << 16) |
                         (pixel[1] << 8)  |
                         (pixel[0]);

        pixels[i] = color;
    }



    stbi_image_free(data);
    return true;
}


void generate_cpp_code(const char* varname,
                       const std::vector<uint32_t>& pixels,
                       int width, int height,
                       const char* output_filename) {

    std::ofstream out(output_filename);
    if (!out) return;

    out << "
    out << "#pragma once\n\n";
    out << "const uint32_t " << varname << "_data["
        << (width * height) << "] = {\n";

    for (size_t i = 0; i < pixels.size(); ++i) {
        if (i % 8 == 0) out << "    ";
        out << "0x" << std::hex << pixels[i] << std::dec;
        if (i != pixels.size() - 1) out << ", ";
        if (i % 8 == 7) out << "\n";
    }

    out << "\n};\n\n";
    out << "const int " << varname << "_width = " << width << ";\n";
    out << "const int " << varname << "_height = " << height << ";\n";

    out.close();
}

