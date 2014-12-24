/**INC+************************************************************************/
/* Header:  device.h                                                          */
/*                                                                            */
/* Purpose: Generic device io functions                                       */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_DEVICE_H_
#define _MINIOS_DEVICE_H_

#include "common/types.h"
#include "common/error.h"

/**STRUCT+*********************************************************************/
/* Structure: DEVICE_OPERATIONS                                               */
/*                                                                            */
/* Description: device operations callbacks                                   */
/**STRUCT-*********************************************************************/
typedef struct device_operations {
    /**************************************************************************/
    /* Write data to device, if supported.                                    */
    /* Parameter 'len' is data size actually written to device.               */
    /**************************************************************************/
    STATUS (*write)(HANDLE handle, CONST VOID *buf, ULONG *len);

    /**************************************************************************/
    /* Read data from device, if supported.                                   */
    /* Parameter 'len' is data size actually read from device.                */
    /**************************************************************************/
    STATUS (*read)(HANDLE handle, VOID *buf, ULONG *len);

    /**************************************************************************/
    /* Read/Write parameters to/from device, if supported.                    */
    /**************************************************************************/
    STATUS (*control)(HANDLE handle, UINT cmd, VOID *buf, ULONG *len);
} DEVICE_OPERATIONS;

/**STRUCT+*********************************************************************/
/* Structure: DEVICE                                                          */
/*                                                                            */
/* Description: device control block                                          */
/**STRUCT-*********************************************************************/
typedef struct device DEVICE;
struct device {
    CONST CHAR *name;
    CONST CHAR *dep_name;
    DEVICE_OPERATIONS *ops;
    BOOL (*init)(DEVICE *dev);
    STATUS (*lookup)(DEVICE **dev, DEVICE *sub_dev, CONST CHAR *name);
    ULONG flags;
    VOID *priv;
};

/**API+************************************************************************/

STATUS device_lookup(CONST CHAR *name, HANDLE *handle);
STATUS device_write(HANDLE handle, CONST VOID *buf, ULONG *len);
STATUS device_read(HANDLE handle, VOID *buf, ULONG *len);
STATUS device_control(HANDLE handle, UINT cmd, VOID *buf, ULONG *len);

/**API-************************************************************************/

#endif /* _MINIOS_DEVICE_H_ */

/**EOF device.h****************************************************************/
