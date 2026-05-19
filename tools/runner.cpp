#include "pngtoarray.cpp"
#include "iostream"

using namespace std;

int main() {

    // int entry_number{};
    // cout << "Enter entry number:";
    // cin >> entry_number;
    // for (int i = 0;i != entry_number;i++)
    // {
    //     int number{};
    //     cout << "Entry number:";
    //     cin >> number;
    // }

    std::vector<uint32_t> pixels;
    int width, height;

    if (png_to_int_array("img.png", pixels, width, height)) {
        generate_cpp_code("logo", pixels, width, height, "logo.cpp");
        std::cout << "Сгенерирован файл icon_data.cpp\n";
    }

    system("x86_64-elf-g++ -g -w -m64 -march=x86-64 -ffreestanding -nostdlib -fno-rtti -c logo.cpp -o ../objects/logo.o");

    //system("cmd.exe /c \"C:/_1A Common/BIG Projects/XCore/comp.bat\"");

    return 0;
}