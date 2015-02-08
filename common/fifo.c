/**MOD+************************************************************************/
/* Module:  fifo.c                                                            */
/*                                                                            */
/* Purpose: A simple FIFO implementation.                                     */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**MOD-************************************************************************/

#include "common/fifo.h"
#include "common/bug.h"
#include "common/string.h"

VOID fifo_init(FIFO *fifo, UINT8 *buffer, UINT32 size)
{
    /* size must be less than MAX size of UINT32 */
    BUG_ON(size > FIFO_MAX_SIZE);
    /*
     * size must be power of 2, since our 'let the indices wrap' tech
     * works only in this case.
     */
    BUG_ON(size & (size - 1));

    fifo->buffer = buffer;
    fifo->size = size;
    fifo->in = 0;
    fifo->out = 0;
}

/* puts some data into the FIFO, returns the number of bytes copied */
UINT32 fifo_put(FIFO *fifo, UINT8 *buffer, UINT32 len)
{
    UINT32 l;

    len = MIN(len, fifo->size - fifo->in + fifo->out);

    /* first put the data starting from fifo->in to buffer end */
    l = MIN(len, fifo->size - (fifo->in & (fifo->size - 1)));
    memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l);

    /* then put the rest (if any) at the beginning of the buffer */
	memcpy(fifo->buffer, buffer + l, len - l);

    /* update the fifo->in index */
    fifo->in += len;

    return len;
}

/* gets some data from the FIFO, returns the number of bytes copied */
UINT32 fifo_get(FIFO *fifo, UINT8 *buffer, UINT32 len)
{
    UINT32 l;

    len = MIN(len, fifo->in - fifo->out);

    /* first get the data from fifo->out until the end of the buffer */
    l = MIN(len, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);

    /* then get the rest (if any) from the beginning of the buffer */
	memcpy(buffer + l, fifo->buffer, len - l);

    /* update the fifo->out index */
    fifo->out += len;

	/*
	 * optimization: if the FIFO is empty, set the indices to 0
	 * so we don't wrap the next time
	 */
    if (fifo->in == fifo->out)
        fifo->in = fifo->out = 0;

    return len;
}

/* returns the number of used bytes in the FIFO */
UINT32 fifo_count(FIFO *fifo)
{
    return fifo->in - fifo->out;
}

/* returns the number of available bytes in the FIFO */
UINT32 fifo_space(FIFO *fifo)
{
    return fifo->size - fifo->in + fifo->out;
}

/******************************************************************************/
