cd ../../

x86_64-elf-g++ -c -ffreestanding -w -nostdlib -Iinclude libraries/fatfs/source/ff.c -o objects/ff.o -fpermissive
if errorlevel 1 exit /b 1

x86_64-elf-g++ -c -ffreestanding -nostdlib -Iinclude libraries/fatfs/source/ffunicode.c -o objects/ffunicode.o -fpermissive -w
if errorlevel 1 exit /b 1

x86_64-elf-g++ -c -ffreestanding -nostdlib -Iinclude libraries/fatfs/source/diskio.c -o objects/diskio.o -fpermissive -w
if errorlevel 1 exit /b 1

x86_64-elf-g++ -c -ffreestanding -nostdlib -Iinclude libraries/fatfs/source/ffsystem.c -o objects/ffsystem.o -fpermissive -w
if errorlevel 1 exit /b 1
