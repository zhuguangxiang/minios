/**INC+************************************************************************/
/* Header:  fifo.h                                                            */
/*                                                                            */
/* Purpose: A simple FIFO implementation.                                     */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_FIFO_H_
#define _MINIOS_FIFO_H_

#include "common/types.h"

typedef struct fifo {
    UINT8 *buffer;
    UINT32 size;
    UINT32 in;
    UINT32 out;
} FIFO;

/* size must be less than MAX size of UINT32 */
#define FIFO_MAX_SIZE 0x80000000

VOID fifo_init(FIFO *fifo, UINT8 *buffer, UINT32 size);
UINT32 fifo_put(FIFO *fifo, UINT8 *buffer, UINT32 len);
UINT32 fifo_get(FIFO *fifo, UINT8 *buffer, UINT32 len);
UINT32 fifo_count(FIFO *fifo);
UINT32 fifo_space(FIFO *fifo);

#endif /* _FIFO_H_ */

/******************************************************************************/
