
OUTPUT_FORMAT("elf32-bigarm", "elf32-bigarm", "elf32-bigarm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
	_text = .;

    .text :
    {
        obj/port/arm7_9/head.o(.text)
        *(.text)
        *(.text.*)
    }

    . = ALIGN(4);

    /* .ctors .dtors are used for c++ constructors/destructors */

    .ctors :
    {
        __ctors_start__ = .;
        *(.ctors.*)
        *(.ctors)
        __ctors_end__ = .;
    }

    .dtors :
    {
        PROVIDE(__dtors_start__ = .);
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))
        PROVIDE(__dtors_end__ = .);
    }

    .dev_init :
    {
        __dev_init_start = .;
        *(.initcall0.init)
        *(.initcall1.init)
        *(.initcall2.init)
        *(.initcall3.init)
        *(.initcall4.init)
        *(.initcall5.init)
        *(.initcall6.init)
        *(.initcall7.init)
        *(.initcall8.init)
        __dev_init_end = .;
    }

    . = ALIGN(4);

    .rodata :
    {
        *(.rodata)
        *(.rodata.*)
    }

    . = ALIGN(4);

    __cmd_start = .;
    .cmd_tbl :
    {
        *(.cmd_tbl)
    }
    __cmd_end = .;

    _etext = .;

    . = ALIGN(4);

    .dev_tbl :
    {
        __DEV_TBL_START__ = .;
        *(.dev_tbl)
        __DEV_TBL_END__ = .;
    }

    . = ALIGN(4);

    .data :
    {
        *(.data)
    }
    _end = .;

    . = ALIGN(4);

    _bss_start = .;
    .bss :
    {
        *(.bss)
    }
    _bss_end = .;
}

