; Блочный комментарий, а не строчный, чтобы сохранить совместимость
; Bootloader for x86_64 long mode
; FASM syntax

FREE_SPACE equ 0x9000

ORG 0x7C00
use16

; Main entry point where BIOS leaves us.
Main:
    jmp 0x0000:.FlushCS               ; Фиксируем CS на 0x0000

.FlushCS:
    xor ax, ax

    ; Настраиваем сегментные регистры
    mov ss, ax
    ; Стек начинается ниже Main
    mov sp, Main

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    cld

    mov ax, 0x1500      ; ES:BX = 0x1000:0x0000
    mov es, ax
    xor bx, bx

    mov ah, 0x02
    mov al, 128           ; 64KB
    mov ch, 0
    mov cl, 5
    mov dh, 0
    mov dl, 0x80
    int 0x13
    jc .disk_error


    call CheckCPU                     ; Проверяем поддержку Long Mode
    jc .NoLongMode

    ; Переключаемся в Long Mode
    mov edi, FREE_SPACE              ; Указатель на свободную память для таблиц страниц
    jmp SwitchToLongMode

.NoLongMode:
    mov si, NoLongModeM
    call Print

.disk_error:
    mov si, DiskError
    call Print

.Die:
    hlt
    jmp .Die

; Константы для таблиц страниц
PAGE_PRESENT equ (1 shl 0)
PAGE_WRITE   equ (1 shl 1)

; Селекторы сегментов GDT
CODE_SEG     equ 0x0008
DATA_SEG     equ 0x0010

; Заглушка для IDT
ALIGN 4
IDT:
    .Length       dw 0
    .Base         dd 0

; ============ SwitchToLongMode ============
; Вход: EDI = адрес 16Кб буфера (выровненный на 4Кб)
SwitchToLongMode:

    push edi

    ; Очищаем буфер (нужно больше - 24КБ для 1ГБ)
    mov ecx, 6144        ; 6144 dword = 24КБ
    xor eax, eax
    rep stosd
    pop edi

    ; PML4[0] → PDPT
    lea eax, [edi + 0x1000]
    or eax, PAGE_PRESENT or PAGE_WRITE
    mov [edi], eax

    ; PDPT[0] → PD (первые 1ГБ)
    lea eax, [edi + 0x2000]
    or eax, PAGE_PRESENT or PAGE_WRITE
    mov [edi + 0x1000], eax

    ; Заполняем PD 2МБ страницами (512 × 2МБ = 1ГБ)
    push edi
    lea edi, [edi + 0x2000]
    mov eax, PAGE_PRESENT or PAGE_WRITE or (1 shl 7)  ; PS=1 для 2МБ страниц

    mov ecx, 512
.BuildPageTable:

    mov [edi], eax
    add eax, 0x200000    ; +2МБ (не 0x1000!)
    add edi, 8
    loop .BuildPageTable

    pop edi

    ; Остальной код без изменений...
    mov al, 0xFF
    out 0xA1, al
    out 0x21, al

    ; Загружаем нулевую IDT
    lidt [IDT]

    ; Включаем PAE и PGE
    mov eax, 10100000b
    mov cr4, eax

    ; Устанавливаем CR3 на PML4
    mov cr3, edi

    ; Включаем Long Mode в EFER MSR
    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x00000100               ; Устанавливаем бит LME
    wrmsr

    ; Включаем страничную адресацию
    mov eax, cr0
    or eax, 0x80000001               ; PG | PE
    mov cr0, eax

    ; Загружаем GDT
    lgdt [GDT.Pointer]

    ; Переход в 64-битный режим
    jmp CODE_SEG:LongMode

; ============ GDT ============
GDT:
.Null:
    dq 0x0000000000000000            ; Нулевой дескриптор

.Code:
    dq 0x00209A0000000000            ; 64-битный код: DPL=0, Present, Exec/Read
    dq 0x0000920000000000            ; 64-битные данные: DPL=0, Present, Read/Write

ALIGN 4
.Pointer:
    dw $ - GDT - 1                   ; Лимит GDT
    dd GDT                           ; Базовый адрес GDT

; ============ 64-bit Code ============
use64

LongMode:

    ; Настраиваем сегментные регистры
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov rsp, 0x80000

    jmp 0x15000

    ; Очищаем экран синим цветом
    ;mov edi, 0xB8000
    ;mov rcx, 500                      ; 500 * 8 = 4000 байт (80*25*2)
    ;mov rax, 0x1F201F201F201F20       ; Синий фон, пробелы
    ;rep stosq
;
    ;; Выводим "Hello World!" в центре экрана
    ;mov edi, 0xB8000 + (12 * 80 + 36) * 2  ; Примерно центр экрана
;
    ;; "Hello "
    ;mov rax, 0x1F6F1F201F6C1F65       ; "o leH" (little-endian)
    ;mov [edi], rax
    ;mov rax, 0x1F001F6C1F6C1F48       ; пробел + "llH"
    ;mov [edi + 8], rax
;
    ;; "World!"
    ;mov rax, 0x1F001F201F641F6F       ; пробел + пробел + "do"
    ;mov [edi + 16], rax
    ;mov rax, 0x1F211F6C1F721F57       ; "!lrW"
    ;mov [edi + 24], rax

    ; Бесконечный цикл
.IdleLoop:
    hlt
    jmp .IdleLoop

; ============ 16-bit Functions ============
use16

; Проверка поддержки Long Mode
; Выход: CF=1 если не поддерживается
CheckCPU:
    ; Проверяем наличие CPUID
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 0x200000                 ; Пробуем изменить бит ID
    push eax
    popfd
    pushfd
    pop eax
    xor eax, ecx
    shr eax, 21
    and eax, 1
    push ecx
    popfd
    test eax, eax
    jz .NoLongMode

    ; Проверяем расширенные функции CPUID
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .NoLongMode

    ; Проверяем бит Long Mode
    mov eax, 0x80000001
    cpuid
    test edx, 1 shl 29                ; LM бит
    jz .NoLongMode

    clc                               ; CF=0 - успех
    ret

.NoLongMode:
    stc                               ; CF=1 - ошибка
    ret

; Печать строки через BIOS
; Вход: DS:SI = указатель на ASCIIZ строку
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
NoLongModeM db "CPU does not support long mode.", 0
DiskError db "DiskError.", 0


; Заполнение до 510 байт
times 510 - ($-$$) db 0

; Сигнатура загрузочного сектора
dw 0xAA55