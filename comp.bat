@echo on

fasm os/boot.asm build/boot.bin
if errorlevel 1 exit /b 1

fasm os/md64.asm build/md64.bin
if errorlevel 1 exit /b 1

fasm os/ap.asm build/ap.bin
if errorlevel 1 exit /b 1

start /b gcc os/kernel/kernel.cpp -w -o prep/prep.cpp -E -DPREP -fpermissive -Iinclude
start /b gcc os/kernel/kernel.cpp -w -o prep/prep.asm -S -DPREP -fpermissive -Iinclude
if errorlevel 1 exit /b 1

start /b gcc os/kernel/middle.cpp -w -o prep/midprep.cpp -E -DPREP -fpermissive -Iinclude
start /b gcc os/kernel/middle.cpp -w -o prep/midprep.asm -S -DPREP -fpermissive -Iinclude
if errorlevel 1 exit /b 1

REM 64-bit компиляция
x86_64-elf-g++ -g -w -m64 -std=c++23 -march=x86-64 -ffreestanding -nostdlib -fno-rtti -c os/kernel/kernel.cpp -o build/kernel.o -fpermissive -fno-use-cxa-atexit -O0 -mcmodel=kernel -mno-red-zone -fno-pic -fno-pie -mno-avx -Iinclude
if errorlevel 1 exit /b 1

x86_64-elf-g++ -g -w -m64 -std=c++23 -march=x86-64 -ffreestanding -nostdlib -fno-rtti -c os/kernel/middle.cpp -o build/middle.o -fpermissive -fno-use-cxa-atexit -O0 -D stage2 -mcmodel=kernel -mno-red-zone -fno-pic -fno-pie -mno-avx -Iinclude
if errorlevel 1 exit /b 1

x86_64-elf-ld -g -m elf_x86_64 -T linker/linker.ld -o build/kernel.elf build/kernel.o objects/ff.o objects/diskio.o objects/ffsystem.o objects/ffunicode.o objects/logo.o
if errorlevel 1 exit /b 1

x86_64-elf-objcopy -O binary build/kernel.elf build/kernel.bin
if errorlevel 1 exit /b 1

x86_64-elf-ld -g -w -m elf_x86_64 -Ttext 0x15000 -o build/middle.bin --oformat binary build/middle.o
if errorlevel 1 exit /b 1

if "%*" NEQ "" (
x86_64-elf-ld -g -w -m elf_x86_64 -Ttext 0x200000 -o build/kernel.elf build/kernel.o
if errorlevel 1 exit /b 1
)
del os.img

dd if=/dev/zero of=os.img bs=512 count=32768
if errorlevel 1 exit /b 1

dd if=build/boot.bin of=os.img bs=512 count=1 conv=notrunc
if errorlevel 1 exit /b 1

dd if=build/md64.bin of=os.img bs=512 seek=4 conv=notrunc
if errorlevel 1 exit /b 1

dd if=build/middle.bin of=os.img bs=512 seek=6 conv=notrunc
if errorlevel 1 exit /b 1

dd if=build/kernel.bin of=os.img bs=512 seek=39 conv=notrunc
if errorlevel 1 exit /b 1

dd if=build/ap.bin of=os.img bs=512 seek=1 conv=notrunc
if errorlevel 1 exit /b 1

if "%*"=="" (
    cmd /k run.bat
)
exit 0