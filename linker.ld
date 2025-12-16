ENTRY(_start)

SECTIONS
{
    . = 0x15000;

    /* ВАЖНО: Все секции должны идти последовательно */
    .text : {
        *(.text*)
    }

    /* .rodata должна быть сразу после .text */
    . = ALIGN(16);
    .rodata : {
        *(.rodata*)
        *(.rdata*)
        *(.rdata$*)
    }

    . = ALIGN(16);
    .data : {
        *(.data*)
    }

    .bss : {
        *(.bss*)
        *(COMMON)
    }

    /* Проверочные метки */
    _text_end = .;
    _rodata_start = .;
    _rodata_end = .;
    _data_start = .;
}