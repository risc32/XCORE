format binary as 'img'
use16
org 0x7C00

start:
    ; Инициализация
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    ; Сообщение
    mov si, msg_loading
    call print_string
    
    ; Загрузка ядра
    mov ax, 0x1000      ; ES:BX = 0x1000:0x0000
    mov es, ax
    xor bx, bx

    mov ah, 0x02
    mov al, 128           ; 64KB
    mov ch, 0
    mov cl, 40
    mov dh, 0
    mov dl, 0x80
    int 0x13
    jc disk_error
    
    ; Подготовка к защищённому режиму
    cli
    call enable_a20
    lgdt [gdt_descriptor]
    
    ; Переход в защищённый режим
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    ; Дальний прыжок для очистки pipeline
    jmp 0x08:protected_mode

; --- 16-битные функции ---
print_string:
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

disk_error:
    mov si, msg_error
    call print_string
    jmp $

; --- GDT ---
gdt_start:
    dq 0x0000000000000000    ; Null descriptor
    dq 0x00CF9A000000FFFF    ; Code descriptor (0x08)
    dq 0x00CF92000000FFFF    ; Data descriptor (0x10)
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; --- 32-битный код ---
use32
protected_mode:
    ; Настроить сегменты данных
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Настроить стек
    mov esp, 0x90000
    
    ; Передать управление ядру
    jmp 0x10000

; --- Данные ---
msg_loading db 'Boot: Loading kernel... ',0
msg_error db 'Disk error!',0

times 510-($-$$) db 0
dw 0xAA55