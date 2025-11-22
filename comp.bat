@echo off

fasm os/boot.asm build/boot.bin
if errorlevel 1 exit /b 1

fasm os/kernel/cmd/reboot.asm build/reboot.bin
if errorlevel 1 exit /b 1

gcc os/kernel/kernel.cpp -o prep.cpp -Wall -E -DPREP -Wchanges-meaning
gcc os/kernel/kernel.cpp -o prep.asm -Wall -S -DPREP -Wchanges-meaning -fpermissive
if errorlevel 1 exit /b 1

i686-elf-g++ -ffreestanding -nostdlib -fno-rtti -c os/kernel/kernel.cpp -o build/kernel.o -Wall -fno-use-cxa-atexit -Wchanges-meaning -O0

rem -finstrument-functions
rem -finstrument-functions -finstrument-functions-exclude-file-list=os/kernel/cpu/trace.cpp
if errorlevel 1 exit /b 1

i686-elf-ld -o build/kernel.bin -Ttext 0x10000 --oformat binary build/kernel.o -O0
if errorlevel 1 exit /b 1

del os.img

dd if=/dev/zero of=os.img bs=512 count=2048
if errorlevel 1 exit /b 1

dd if=build/boot.bin of=os.img bs=512 count=1 conv=notrunc
if errorlevel 1 exit /b 1

dd if=build/kernel.bin of=os.img bs=512 seek=39 conv=notrunc
if errorlevel 1 exit /b 1

dd if=build/reboot.bin of=os.img bs=512 seek=1024 conv=notrunc
if errorlevel 1 exit /b 1


qemu-system-x86_64 -drive format=raw,file=os.img -monitor stdio