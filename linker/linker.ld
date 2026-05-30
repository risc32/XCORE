ENTRY(_start)

SECTIONS
{
    . = 0x200000;

    /* Символы для загрузчика */
    _kernel_start = .;

    .text : ALIGN(4096) {
        _text_start = .;
        *(.text)
        *(.text.*)
        _text_end = .;
    }

    .rodata : ALIGN(4096) {
        _rodata_start = .;
        *(.rodata)
        *(.rodata.*)
        _rodata_end = .;
    }

    .data : ALIGN(4096) {
        _data_start = .;
        *(.data)
        *(.data.*)
        _data_end = .;
    }

    .init_array : ALIGN(8) {
        __init_array_start = .;
        KEEP(*(.init_array*))
        KEEP(*(SORT(.init_array.*)))
        __init_array_end = .;
    }

    .ctors : ALIGN(8) {
        __CTOR_LIST__ = .;
        KEEP(*(.ctors))
        KEEP(*(SORT(.ctors.*)))
        __CTOR_END__ = .;
    }

    .bss : ALIGN(4096) {
        sbss = .;
        _bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(16);
        ebss = .;
        _bss_end = .;
    }

    _kernel_end = .;

    /* ПОДСКАЗКИ ДЛЯ ЗАГРУЗЧИКА */
    _text_size = _text_end - _text_start;
    _data_size = _data_end - _data_start;
    _bss_size = _bss_end - _bss_start;
}