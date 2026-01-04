@echo off

fasm os/boot.asm build/boot.bin
if errorlevel 1 exit /b 1

fasm os/md64.asm build/md64.bin
if errorlevel 1 exit /b 1

fasm os/kernel/cmd/reboot.asm build/reboot.bin
if errorlevel 1 exit /b 1

gcc os/kernel/kernel.cpp -w -o prep/prep.ii -E -DPREP -fpermissive
gcc os/kernel/kernel.cpp -w -o prep/prep.asm -S -DPREP -fpermissive
if errorlevel 1 exit /b 1

gcc os/kernel/kernel.cpp -w -D stage2 -o prep/midprep.ii -E -DPREP -fpermissive
gcc os/kernel/kernel.cpp -w -D stage2 -o prep/midprep.asm -S -DPREP -fpermissive
if errorlevel 1 exit /b 1

REM 64-bit компиляция
x86_64-elf-g++ -w -m64 -march=x86-64 -ffreestanding -nostdlib -fno-rtti -c os/kernel/kernel.cpp -o build/kernel.o -fpermissive -fno-use-cxa-atexit -O0 -mcmodel=kernel -mno-red-zone -fno-pic -fno-pie
if errorlevel 1 exit /b 1

x86_64-elf-g++ -w -m64 -march=x86-64 -ffreestanding -nostdlib -fno-rtti -c os/kernel/kernel.cpp -o build/middle.o -fpermissive -fno-use-cxa-atexit -O0 -D stage2 -mcmodel=kernel -mno-red-zone -fno-pic -fno-pie
if errorlevel 1 exit /b 1

x86_64-elf-ld -w -m elf_x86_64 -Ttext 0x200000 -o build/kernel.bin --oformat binary build/kernel.o
if errorlevel 1 exit /b 1

x86_64-elf-ld -w -m elf_x86_64 -Ttext 0x15000 -o build/middle.bin --oformat binary build/middle.o
if errorlevel 1 exit /b 1

del os.img

dd if=/dev/zero of=os.img bs=512 count=8192
if errorlevel 1 exit /b 1

dd if=build/boot.bin of=os.img bs=512 count=1 conv=notrunc
if errorlevel 1 exit /b 1

dd if=build/md64.bin of=os.img bs=512 seek=4 conv=notrunc
if errorlevel 1 exit /b 1

dd if=build/middle.bin of=os.img bs=512 seek=6 conv=notrunc
if errorlevel 1 exit /b 1

dd if=build/kernel.bin of=os.img bs=512 seek=39 conv=notrunc
if errorlevel 1 exit /b 1

dd if=build/reboot.bin of=os.img bs=512 seek=1024 conv=notrunc
if errorlevel 1 exit /b 1

cmd /k run.bat