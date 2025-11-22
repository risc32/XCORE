use32
org 0x7c00

start:

    ; Сохраняем регистры
    pusha
    push es
    
    ; Устанавливаем сегмент
    mov ax, 0x0000
    mov es, ax
    
    mov di, 0x7D00      ; Начало очистки
    mov ecx, [0x7DF8]      ; Количество слов: (0x10000 - 0x7E00) / 2 = 0x4100
    xor ax, ax          ; AX = 0
    rep stosd           ; Заполняем хуями
    
    ; Восстанавливаем регистры
    pop es
    popa
    jmp 0xFFFF:0x0000

; Функция вывода строки
print_string:
    mov ah, 0x0E
.print_loop:
    lodsb
    test al, al
    jz .print_done
    int 0x10
    jmp .print_loop
.print_done:
    ret

success_msg db 'Memory after 0x7E00 cleared!', 0x0D, 0x0A, 0

; Заполняем до конца сектора
times 510-($-$$) db 0
dw 0xAA55