/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/

#ifndef _HAL_S3C2440_IO_H_
#define _HAL_S3C2440_IO_H_

#include "os/minios_type.h"

#define READ_REG(addr)  *(volatile uint32_t *)(addr)
#define WRITE_REG(addr, val) *(volatile uint32_t *)(addr) = (val)

#endif // _MINIOS_S3C2440_IO_H_

/*--------------------------------------------------------------------------*/
// EOF s3c2440_io.h
