
OUTPUT_FORMAT("elf32-bigarm", "elf32-bigarm", "elf32-bigarm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
	_text = .;

    .text :
    {
        obj/hal/arm7_9/head.o(.text)
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

    . = ALIGN(4);

    .fs_tbl :
    {
        __FS_TBL_START__ = .;
        *(.fs_tbl)
        __FS_TBL_END__ = .;
    }

    . = ALIGN(4);

    _etext = .;

    . = ALIGN(4);

    .mod_tbl :
    {
        __MOD_TBL_START__ = .;
        *(.mod_init_lvl_0.init)
        *(.mod_init_lvl_1.init)
        *(.mod_init_lvl_2.init)
        *(.mod_init_lvl_3.init)
        *(.mod_init_lvl_4.init)
        *(.mod_init_lvl_5.init)
        *(.mod_init_lvl_6.init)
        *(.mod_init_lvl_7.init)
        *(.mod_init_lvl_8.init)
        *(.mod_init_lvl_9.init)
        *(.mod_init_lvl_10.init)
        *(.mod_init_lvl_11.init)
        *(.mod_init_lvl_12.init)
        *(.mod_init_lvl_13.init)
        *(.mod_init_lvl_14.init)
        *(.mod_init_lvl_15.init)
        *(.mod_init_lvl_16.init)
        *(.mod_init_lvl_17.init)
        *(.mod_init_lvl_18.init)
        *(.mod_init_lvl_19.init)
        *(.mod_init_lvl_20.init)
        __MOD_TBL_END__ = .;
    }

    .dev_tbl :
    {
        __DEV_TBL_START__ = .;
        *(.dev_tbl_lvl_0)
        *(.dev_tbl_lvl_1)
        *(.dev_tbl_lvl_2)
        *(.dev_tbl_lvl_3)
        *(.dev_tbl_lvl_4)
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

