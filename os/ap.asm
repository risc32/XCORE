; ap.asm - исправленная версия

ORG 0x7000
use16

Main:
    jmp 0x0000:.FlushCS

.FlushCS:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, Main - 0x100
    mov bp, sp
    cld

    call SwitchToLongMode

.Die:
    hlt
    jmp .Die

PAGE_PRESENT equ (1 shl 0)
PAGE_WRITE   equ (1 shl 1)
PAGE_LARGE   equ (1 shl 7)

CODE_SEG     equ GDT.Code - GDT
DATA_SEG     equ GDT.Data - GDT

IDT:
    dw 0
    dd 0

SwitchToLongMode:
    cli

    ; Отключаем IRQs
    mov al, 0xFF
    out 0xA1, al
    out 0x21, al

    ; Пустая IDT
    lidt [IDT]

    ; Включаем PAE
    mov eax, cr4
    or eax, (1 shl 5)
    mov cr4, eax

    ; КРИТИЧЕСКИ ВАЖНО: Используем таблицы BSP (0xA000)
    mov eax, 0xA000    ; Прямой адрес таблиц BSP
    mov cr3, eax

    ; Включаем long mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, (1 shl 8)
    wrmsr

    ; Включаем пейджинг
    mov eax, cr0
    or eax, (1 shl 31) or (1 shl 0)  ; PG=1, PE=1
    or eax, (1 shl 1)                 ; MP=1 (важно!)
    mov cr0, eax

    ; Проверка что пейджинг включился
    mov eax, cr0
    test eax, (1 shl 31)
    jz Main.Die

    ; Загружаем GDT
    lgdt [GDT.Pointer]

    ; Переход в long mode
    jmp CODE_SEG:(Main + LongModeEntry - $$)

; GDT должна быть ТОЧНО ТАКОЙ ЖЕ как в md64.asm
GDT:
.Null:  dq 0
.Code:  dq 0x00209A0000000000    ; 64-bit code (0x08)
.Data:  dq 0x0000920000000000    ; 64-bit data (0x10)
.Pointer:
    dw $ - GDT - 1
    dq GDT

use64

LongModeEntry:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Проверка CR3 (должен быть 0xA000)
    mov rax, cr3
    cmp rax, 0xA000
    jne .Hang

    ; Читаем адрес функции
    mov rax, [0x100000]
    test rax, rax
    jz .Hang

    jmp rax

.Hang:
    hlt
    jmp .Hang