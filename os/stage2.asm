format binary as 'img'
use16


stage2:
    mov ax, cs
    mov ds, ax
    mov es, ax

    mov si, msg_stage2
    call print_str

    ; === VBE ОБНАРУЖЕНИЕ ===
    mov ax, 0x4F00
    mov di, vbe_info
    mov dword [es:di], 'VBE2'
    int 0x10
    cmp ax, 0x004F
    jne no_vesa

    ; Поиск видеорежима 1024x768x32
    mov ax, [es:vbe_info + 0x0E]    ; VideoModePtr
    mov bx, [es:vbe_info + 0x10]
    mov fs, bx
    mov si, ax

.find_mode:
    mov cx, [fs:si]
    cmp cx, 0xFFFF
    je no_mode

    ; Получаем информацию о режиме
    mov ax, 0x4F01
    mov di, mode_info
    int 0x10
    cmp ax, 0x004F
    jne .next_mode

    ; Проверяем режим
    mov ax, [es:di]          ; Attributes
    test ax, 0x90            ; LFB + graphics
    jz .next_mode
    mov ax, [es:di + 0x12]   ; Width
    cmp ax, 1024
    jne .next_mode
    mov ax, [es:di + 0x14]   ; Height
    cmp ax, 768
    jne .next_mode
    mov al, [es:di + 0x19]   ; BPP
    cmp al, 32
    jne .next_mode

    ; Нашли режим!
    mov [selected_mode], cx
    jmp .found_mode

.next_mode:
    add si, 2
    jmp .find_mode

.found_mode:
    ; Запрос пользователя
    mov si, msg_ask_gfx
    call print_str

    mov ah, 0x00
    int 0x16

    cmp al, 'y'
    je setup_graphics
    jmp setup_text

setup_graphics:
    ; Установка графического режима
    mov ax, 0x4F02
    mov bx, [selected_mode]
    or bx, 0x4000      ; LFB
    int 0x10
    cmp ax, 0x004F
    jne graphics_error

    ; Сохраняем параметры для ядра
    mov eax, [es:di + 0x28]  ; LFB address
    mov dword [0x7BD0], eax
    mov ax, [es:di + 0x12]   ; Width
    mov word [0x7BD4], ax
    mov ax, [es:di + 0x14]   ; Height
    mov word [0x7BD6], ax
    mov ax, [es:di + 0x10]   ; Pitch
    mov word [0x7BD8], ax
    mov al, [es:di + 0x19]   ; BPP
    mov byte [0x7BDA], al
    mov byte [0x7BDB], 1     ; Graphics enabled

    mov si, msg_gfx_ok
    call print_str
    jmp switch_to_pm

setup_text:
    ; Текстовый режим
    mov dword [0x7BD0], 0xB8000
    mov word [0x7BD4], 80
    mov word [0x7BD6], 25
    mov word [0x7BD8], 160
    mov byte [0x7BDA], 4
    mov byte [0x7BDB], 0

    mov si, msg_text_mode
    call print_str

switch_to_pm:
    ; Переход в защищенный режим
    mov si, msg_switch_pm
    call print_str

    cli
    call enable_a20
    lgdt [gdt_ptr]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:protected_mode

no_vesa:
    mov si, msg_no_vesa
    call print_str
    jmp setup_text

no_mode:
    mov si, msg_no_mode
    call print_str
    jmp setup_text

graphics_error:
    mov si, msg_gfx_error
    call print_str
    jmp setup_text

enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

; Данные Stage 2
msg_stage2 db 'Stage 2: VBE detection...', 0x0D, 0x0A, 0
msg_ask_gfx db 'Use graphics? [y/n] ', 0
msg_gfx_ok db 'Graphics enabled!', 0x0D, 0x0A, 0
msg_text_mode db 'Text mode selected', 0x0D, 0x0A, 0
msg_switch_pm db 'Switching to PM...', 0x0D, 0x0A, 0
msg_no_vesa db 'No VESA!', 0x0D, 0x0A, 0
msg_no_mode db 'No 1024x768 mode!', 0x0D, 0x0A, 0
msg_gfx_error db 'Graphics error!', 0x0D, 0x0A, 0

selected_mode dw 0
vbe_info: times 512 db 0
mode_info: times 256 db 0

; GDT
gdt_start:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
gdt_end:

gdt_ptr:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; ==================== PROTECTED MODE ====================
use32
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    ; Переход в ядро
    jmp 0x10000

; Заполнение до 4KB (8 секторов)
times 4096-($-$$) db 0