; boot.asm - Stage 1 Bootloader
; Загружает переходник в 64-битный режим и запускает его

ORG 0x7C00
use16

; Константы
TRANSITION_ADDR equ 0x7E00    ; Адрес загрузки переходника
TRANSITION_SECTORS equ 2      ; 2 сектора = 1KB (переходник)
TRANSITION_START_SECTOR equ 5 ; Сектор на диске

; Main entry point
Main:
    jmp 0x0000:.FlushCS

.FlushCS:
    xor ax, ax
    mov ss, ax
    mov sp, 0x7C00           ; Стек начинается здесь
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    cld

    call selectmode

    ; Загружаем переходник в 64-битный режим
    mov ax, TRANSITION_ADDR shr 4    ; ES:BX = 0x07E0:0x0000
    mov es, ax
    xor bx, bx

    mov ah, 0x02                    ; Функция чтения секторов
    mov al, 2      ; 2 сектора
    mov ch, 0                       ; Цилиндр
    mov cl, TRANSITION_START_SECTOR ; Сектор 5
    mov dh, 0                       ; Головка
    mov dl, 0x80                    ; Диск
    int 0x13
    jc .disk_error

    jmp TRANSITION_ADDR

.disk_error:
    mov si, DiskError
    call Print



.Die:
    hlt
    jmp .Die

getch:
    mov ah, 0x00        ; Функция BIOS: читать символ
    int 0x16            ; Вызов прерывания клавиатуры
    ret

selectmode:
    mov si, LGASK
    call Print
.loop:
    call getch

    cmp al, 'y'         ; Сравнить ASCII код
    je .grap
    cmp al, 'Y'
    je .grap

    ; Вариант 3: Проверить несколько вариантов
    cmp al, 'n'
    je .text
    cmp al, 'N'
    je .text

    jmp .loop

.grap:
    call videomode
    jmp .end

.text:
.end:
    ret

; ============ Функции ============

videomode:
    mov ax, 0x4F01      ; Функция: получить информацию о режиме
    mov cx, 0x118       ; Номер режима: 1024x768, 24/32bpp
    mov di, 0x8400      ; ES:DI = буфер для Mode Info Block (256 байт)
    ; !!! ВАЖНО: используем ДРУГОЙ буфер (не тот же 0x8000) !!!
    int 0x10
    cmp ax, 0x004F
    jne .eg

    ; 3. Устанавливаем видеорежим
    mov ax, 0x4F02      ; Функция: установить видеорежим
    mov bx, 0x4118      ; Режим 0x118 + бит 14 (линейный буфер)
    ; Если не нужен LFB: mov bx, 0x0118
    int 0x10
    cmp ax, 0x004F
    jne .eg

    ; Успех
    ret

.eg:
    mov si, egm
    call Print
    jmp Main.Die

Print:
    pusha
.PrintLoop:
    lodsb
    test al, al
    jz .PrintDone
    mov ah, 0x0E
    int 0x10
    jmp .PrintLoop
.PrintDone:
    popa
    ret

; Данные
LGASK db "Boot in graphics mode? [Y/n]", 0x0D, 0x0A, 0
DiskError db "Disk Error.", 0
egm db "Graphics error.", 0

; Заполнение до 510 байт
times 510 - ($-$$) db 0
dw 0xAA55