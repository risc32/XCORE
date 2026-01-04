#include "pngtoarray.cpp"

int main() {
    std::vector<uint32_t> pixels;
    int width, height;

    if (png_to_int_array("charlie.png", pixels, width, height)) {
        generate_cpp_code("my_icon", pixels, width, height, "charlie.hpp");
        std::cout << "Сгенерирован файл icon_data.hpp\n";
    }

    system("cmd.exe /c \"C:/_1A Common/BIG Projects/XCore/comp.bat\"");

    return 0;
}