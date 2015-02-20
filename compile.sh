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
LD_FLOAT_LIB="-lgcc -L/usr/lib/gcc/arm-none-eabi/4.8.2"
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

compile "common/error.c"
compile "common/crc16.c"
compile "common/ctype.c"
compile "common/itoa.c"
compile "common/vsnprintf.c"
compile "common/memset.c"
compile "common/memcmp.c"
compile "common/memmove.c"
compile "common/memcpy.c"
compile "common/strlen.c"
compile "common/strcmp.c"
compile "common/strcpy.c"
compile "common/printf.c"
compile "common/panic.c"
compile "common/fifo.c"
ar "obj/common/*.o" "libcommon.a"

compile "kernel/task.c"
compile "kernel/hsr.c"
compile "kernel/wait_queue.c"
compile "kernel/timer.c"
compile "kernel/mutex.c"
compile "kernel/semaphore.c"
compile "kernel/msg_queue.c"
ar "obj/kernel/*.o" "libkernel.a"

compile "mm/mem_pool.c"
ar "obj/mm/*.o" "libmm.a"

compile "fs/fd.c"
compile "fs/mount.c"
compile "fs/open.c"
compile "fs/read_write.c"
compile "fs/namei.c"
ar "obj/fs/*.o" "libvfs.a"

compile "fs/ram/ramfs.c"
ar "obj/fs/ram/*.o" "libramfs.a"

compile "common/arm/div0.c"
compile "common/arm/_divsi3.S"
compile "common/arm/_modsi3.S"
compile "common/arm/_udivsi3.S"
compile "common/arm/_umodsi3.S"
ar "obj/common/arm/*.o" "libarm.a"

if [ -f obj/hal/arm7_9/head.o ]; then
rm obj/hal/arm7_9/head.o
fi

compile "hal/arm7_9/context_switch.S"
compile "hal/arm7_9/interrupt.S"
ar "obj/hal/arm7_9/*.o" "libarm7_9.a"
compile "hal/arm7_9/head.S"

compile "hal/s3c2440/s3c2440_interrupt.c"
compile "hal/s3c2440/s3c2440_timer.c"
compile "hal/s3c2440/puts.c"
ar "obj/hal/s3c2440/*.o" "libs3c2440.a"

compile "app/app.c"
compile "app/test_mutex.c"
compile "app/test_sem.c"
compile "app/test_mq.c"
compile "app/test_mempool.c"
compile "app/test_fs.c"

ld "minios.elf" "obj/app/*.o" "-lkernel -lmm -lramfs -lvfs -lramfs -lcommon -larm -larm7_9 -ls3c2440 -L."
dump "minios.elf" "minios.elf.dump.txt"
bin "minios.elf" "minios.bin"


