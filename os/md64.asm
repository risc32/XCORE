; Блочный комментарий, а не строчный, чтобы сохранить совместимость
; Bootloader for x86_64 long mode
; FASM syntax

PML4 equ 0xA000
PDPT equ 0xB000
PD   equ 0xC000

ORG 0x7E00
use16

; Main entry point where BIOS leaves us.
Main:
    jmp 0x0000:.FlushCS               ; Фиксируем CS на 0x0000

.FlushCS:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, Main - 0x100              ; Стек ниже кода
    mov bp, sp
    cld

    ; Загружаем ядро по адресу 0x10000 (сегмент 0x1000, смещение 0x0000)
        mov ax, 0x1500      ; ES:BX = 0x1000:0x0000
        mov es, ax
        xor bx, bx

        mov ah, 0x02
        mov al, 128           ; 64KB (128 секторов)
        mov ch, 0             ; Цилиндр 0
        mov cl, 7             ; Сектор 2 (после загрузчика)
        mov dh, 0             ; Головка 0
        mov dl, 0x80          ; Диск 0x80 (первый жесткий диск)
        int 0x13
        jc .disk_error

    call CheckCPU
    jc .NoLongMode

    ; Переключаемся в Long Mode
    call SwitchToLongMode

.NoLongMode:
    mov si, NoLongModeM
    call Print
    jmp .Die

.disk_error:
    mov si, DiskError
    call Print

.Die:
    hlt
    jmp .Die

; Константы для таблиц страниц
PAGE_PRESENT equ (1 shl 0)
PAGE_WRITE   equ (1 shl 1)
PAGE_LARGE   equ (1 shl 7)

CODE_SEG     equ GDT.Code - GDT
DATA_SEG     equ GDT.Data - GDT

; Заглушка для IDT
IDT:
    dw 0             ; Лимит
    dd 0             ; База

SwitchToLongMode:
    ; Очищаем таблицы страниц
    mov edi, PML4
    mov ecx, 4096
    xor eax, eax
    rep stosb

    mov edi, PDPT
    mov ecx, 4096
    rep stosb

    mov edi, PD
    mov ecx, 4096
    rep stosb

    ; Настраиваем PML4[0] -> PDPT (64-битный указатель!)
    mov edi, PML4
    mov eax, PDPT
    or eax, PAGE_PRESENT or PAGE_WRITE
    mov [edi], eax
    mov dword [edi+4], 0             ; Верхние 32 бита

    ; Настраиваем PDPT[0] -> PD
    mov edi, PDPT
    mov eax, PD
    or eax, PAGE_PRESENT or PAGE_WRITE
    mov [edi], eax
    mov dword [edi+4], 0

    ; Настраиваем 2MB страницы (первые 512 записей = 1GB)
    mov edi, PD
    mov eax, PAGE_PRESENT or PAGE_WRITE or PAGE_LARGE
    xor edx, edx
    mov ecx, 512

.SetupPD:
    mov [edi], eax
    mov [edi+4], edx
    add eax, 0x200000
    adc edx, 0
    add edi, 8
    loop .SetupPD

    ; Отключаем прерывания
    cli

    ; Отключаем IRQs через PIC
    mov al, 0xFF
    out 0xA1, al
    out 0x21, al

    ; Загружаем пустую IDT
    lidt [IDT]

    ; Включаем PAE
    mov eax, cr4
    or eax, (1 shl 5)
    mov cr4, eax

    ; Устанавливаем CR3
    mov eax, PML4
    mov cr3, eax

    ; Включаем long mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, (1 shl 8)
    wrmsr

    ; Включаем пейджинг
    mov eax, cr0
    or eax, (1 shl 31) or (1 shl 0)
    mov cr0, eax

    ; Загружаем GDT
    lgdt [GDT.Pointer]

    ; Дальний переход для входа в long mode
    jmp CODE_SEG:(Main + LongModeEntry - $$)

; GDT
GDT:
.Null:  dq 0
.Code:  dq 0x00209A0000000000    ; 64-bit code
.Data:  dq 0x0000920000000000    ; 64-bit data

.Pointer:
    dw $ - GDT - 1
    dq GDT

; ============ 64-bit Entry Point ============
use64

LongModeEntry:
    ; Настраиваем сегментные регистры
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Настраиваем стек
    mov rsp, 0x200000

    ; Очищаем RFLAGS
    push 0
    popfq

    ; Переход на ядро
    mov rax, 0x15000
    jmp rax                     ; Используем jmp вместо call

.Hang:
    hlt
    jmp .Hang

; ============ 16-bit Functions ============
use16

CheckCPU:
    ; Проверка CPUID
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 0x200000
    push eax
    popfd
    pushfd
    pop eax
    xor eax, ecx
    and eax, 0x200000
    jz .NoLongMode
    push ecx
    popfd

    ; Проверка расширенного CPUID
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .NoLongMode

    ; Проверка бита LM
    mov eax, 0x80000001
    cpuid
    test edx, 1 shl 29
    jz .NoLongMode

    clc
    ret

.NoLongMode:
    stc
    ret

Print:
    pusha
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    popa
    ret

NoLongModeM db "CPU doesn't support long mode.", 0x0D, 0x0A, 0
DiskError db "Disk error.", 0x0D, 0x0A, 0