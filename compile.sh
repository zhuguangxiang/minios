#!/bin/bash
PORT_DIR=arm7_9
ARCH=arm-none-eabi
CC=$ARCH-gcc
LD=$ARCH-ld
AR=$ARCH-ar
OBJDUMP=$ARCH-objdump
OBJCOPY=$ARCH-objcopy
C_OPT="-fno-omit-frame-pointer -O0"
C_DBG="-g"
C_WARN="-Wall -Wformat -Wstrict-prototypes -Wstrict-aliasing"
C_ARCH="-mapcs-frame -std=gnu99 -mbig-endian -march=armv4"
C_DEF="-DCPU_BITS=32"
C_FLAGS="-c -nostdinc -fno-builtin $C_OPT $C_DBG $C_WARN $C_ARCH $C_DEF"

LD_SCRIPT="hal/arm7_9/arm7_9.lds"
TEXT_BASE="0x30000000"
LD_FLAGS="-Bstatic -nostdlib -T $LD_SCRIPT -Ttext $TEXT_BASE"

function compile()
{
	obj="${1%.*}.o"
	obj="${obj##*/}"
	path="${1%/*}"
	# echo "$path"
	if [ ! -d obj/$path ]; then
		mkdir -p obj/$path
	fi
    cmd="$CC $C_FLAGS -I. $1 -o obj/$path/$obj"
    echo "[CC] ${1%.*}.o"
    `$cmd`
}

function ld()
{
	cmd="$LD $LD_FLAGS $2 $3 $4 $5 $6 -o $1"
    echo "[LD] $1"
	`$cmd`
}

function ar()
{
	cmd="$AR cr $2 $1"
    echo "[AR] $2"
	`$cmd`
}

function dump()
{
    echo "[DUMP] $2"
	`$OBJDUMP -D $1 > $2`
}

function bin()
{
    echo "[BIN] $2"
	`$OBJCOPY -O binary $1 $2`
}

if [ ! -d obj ]; then
mkdir obj
fi

# rm obj/*.o

compile "os/task.c"
compile "os/hsr.c"
compile "os/timer.c"
compile "os/main.c"
ar "obj/os/*.o" "libos.a"

compile "hal/arm7_9/context_switch.S"
compile "hal/arm7_9/interrupt.S"
compile "hal/arm7_9/head.S"
compile "hal/s3c2440/s3c2440_interrupt.c"

compile "app/app.c"

ld "minios.elf" "obj/app/*.o" "obj/hal/arm7_9/*.o" "obj/hal/s3c2440/*.o" "-los -L."
dump "minios.elf" "minios.elf.dump.txt"
bin "minios.elf" "minios.bin"


