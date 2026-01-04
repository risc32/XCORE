// png_to_array.cpp
#define STB_IMAGE_IMPLEMENTATION
#include <cstdint>

#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <vector>

// Конвертирует PNG в массив uint32_t ARGB
bool png_to_int_array(const char* filename,
                      std::vector<uint32_t>& pixels,
                      int& width, int& height) {

    // Загружаем изображение
    int channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);

    if (!data) {
        std::cerr << "Ошибка загрузки: " << stbi_failure_reason() << std::endl;
        return false;
    }

    // Преобразуем в ARGB32
    pixels.resize(width * height);

    for (int i = 0; i < width * height; ++i) {
        unsigned char* pixel = &data[i * 4];  // BGRA из PNG (если библиотека так возвращает)

        // Формат 0x00RRGGBB, но у нас BGR, поэтому:
        // pixel[0] = Blue
        // pixel[1] = Green
        // pixel[2] = Red
        // pixel[3] = Alpha

        uint32_t color = 0x00000000 |          // Альфа = 00 (непрозрачный)
                         (pixel[2] << 16) |    // Red (третий элемент!) -> байт 2
                         (pixel[1] << 8)  |    // Green (второй элемент) -> байт 1
                         (pixel[0]);           // Blue (первый элемент) -> байт 0

        pixels[i] = color;
    }

    // Пример: RGB(255, 128, 0) = 0x00FF8000

    stbi_image_free(data);
    return true;
}

// Генерирует C++ код с массивом
void generate_cpp_code(const char* varname,
                       const std::vector<uint32_t>& pixels,
                       int width, int height,
                       const char* output_filename) {

    std::ofstream out(output_filename);
    if (!out) return;

    out << "// Автоматически сгенерировано из PNG\n";
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

