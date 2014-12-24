/**INC+************************************************************************/
/* Header:  s3c2440_regs.h                                                    */
/*                                                                            */
/* Purpose: s3c2440 registers                                                 */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _HAL_S3C2440_REG_H_
#define _HAL_S3C2440_REG_H_

#define CLOCK_BASE 0x4c000000
#define oLOCKTIME  0x00
#define oMPLLCON   0x04
#define oUPLLCON   0x08
#define oCLKCON    0x0c
#define oCLKSLOW   0x10
#define oCLKDIVN   0x14
#define oCAMDIVN   0x18

#define MEMORY_BASE 0x48000000
#define oBWSCON     0x00
#define oBANKCON0   0x04
#define oBANKCON1   0x08
#define oBANKCON2   0x0c
#define oBANKCON3   0x10
#define oBANKCON4   0x14
#define oBANKCON5   0x18
#define oBANKCON6   0x1c
#define oBANKCON7   0x20
#define oREFRESH    0x24
#define oBANKSIZE   0x28
#define oMRSRB6     0x2c
#define oMRSRB7     0x30

#define WTD_BASE  0x53000000
#define oWTCON    0x00
#define oWTDAT    0x04
#define oWTCNT    0x08

#define INT_BASE    0x4a000000
#define oSRCPND     0x00
#define oINTMOD     0x04
#define oINTMSK     0x08
#define oPRIORITY   0x0c
#define oINTPND     0x10
#define oINTOFFSET  0x14
#define oSUBSRCPND  0x18
#define oINTSUBMSK  0x1c

#define GPIO_BASE   0x56000000
#define oGPIO_A     0x00
#define oGPIO_B     0x10
#define oGPIO_C     0x20
#define oGPIO_D     0x30
#define oGPIO_E     0x40
#define oGPIO_F     0x50
#define oGPIO_G     0x60
#define oGPIO_H     0x70
#define oGPIO_J     0xd0

#define oCON 0
#define oDAT 4
#define oUP  8

#define GPACON      (GPIO_BASE+oGPIO_A+oCON)
#define GPADAT      (GPIO_BASE+oGPIO_A+oDAT)

#define GPBCON      (GPIO_BASE+oGPIO_B+oCON)
#define GPBDAT      (GPIO_BASE+oGPIO_B+oDAT)
#define GPBUP       (GPIO_BASE+oGPIO_B+oUP)

#define GPCCON      (GPIO_BASE+oGPIO_C+oCON)
#define GPCDAT      (GPIO_BASE+oGPIO_C+oDAT)
#define GPCUP       (GPIO_BASE+oGPIO_C+oUP)

#define GPDCON      (GPIO_BASE+oGPIO_D+oCON)
#define GPDDAT      (GPIO_BASE+oGPIO_D+oDAT)
#define GPDUP       (GPIO_BASE+oGPIO_D+oUP)

#define GPECON      (GPIO_BASE+oGPIO_E+oCON)
#define GPEDAT      (GPIO_BASE+oGPIO_E+oDAT)
#define GPEUP       (GPIO_BASE+oGPIO_E+oUP)

#define GPFCON      (GPIO_BASE+oGPIO_F+oCON)
#define GPFDAT      (GPIO_BASE+oGPIO_F+oDAT)
#define GPFUP       (GPIO_BASE+oGPIO_F+oUP)

#define GPGCON      (GPIO_BASE+oGPIO_G+oCON)
#define GPGDAT      (GPIO_BASE+oGPIO_G+oDAT)
#define GPGUP       (GPIO_BASE+oGPIO_G+oUP)

#define GPHCON      (GPIO_BASE+oGPIO_H+oCON)
#define GPHDAT      (GPIO_BASE+oGPIO_H+oDAT)
#define GPHUP       (GPIO_BASE+oGPIO_H+oUP)

#define TIMER_BASE  0x51000000

#define TCFG0       (TIMER_BASE+0x00)
#define TCFG1       (TIMER_BASE+0x04)
#define TCON        (TIMER_BASE+0x08)

#define PWM_PORTn(No, x) (TIMER_BASE + 0x0c + (No)*0x0c + x)

#define TCNTB0      PWM_PORTn(0, 0)
#define TCMPB0      PWM_PORTn(0, 4)
#define TCNTO0      PWM_PORTn(0, 8)

#define TCNTB1      PWM_PORTn(1, 0)
#define TCMPB1      PWM_PORTn(1, 4)
#define TCNTO1      PWM_PORTn(1, 8)

#define TCNTB2      PWM_PORTn(2, 0)
#define TCMPB2      PWM_PORTn(2, 4)
#define TCNTO2      PWM_PORTn(2, 8)

#define TCNTB3      PWM_PORTn(3, 0)
#define TCMPB3      PWM_PORTn(3, 4)
#define TCNTO3      PWM_PORTn(3, 8)

#define TCNTB4      PWM_PORTn(4, 0)
#define TCNTO4      PWM_PORTn(4, 4)

#define UART_BASE   0x50000000

#define UART_PORT(no, x) (UART_BASE + (no)*0x4000 + x)

#define oULCON    0x00
#define oUCON     0x04
#define oUFCON    0x08
#define oUMCON    0x0c
#define oUTRSTAT  0x10
#define oUERSTAT  0x14
#define oUFSTAT   0x18
#define oUMSTAT   0x1c
#define oUTXHL    0x20
#define oUTXHB    0x23
#define oURXHL    0x24
#define oURXHB    0x27
#define oUBRDIV   0x28

#define ULCON0      UART_PORT(0, oULCON)
#define UCON0       UART_PORT(0, oUCON)
#define UFCON0      UART_PORT(0, oUFCON)
#define UMCON0      UART_PORT(0, oUMCON)
#define UTRSTAT0    UART_PORT(0, oUTRSTAT)
#define UERSTAT0    UART_PORT(0, oUERSTAT)
#define UFSTAT0     UART_PORT(0, oUFSTAT)
#define UMSTAT0     UART_PORT(0, oUMSTAT)
#define UTXHL0      UART_PORT(0, oUTXHL)
#define URXHL0      UART_PORT(0, oURXHL)
#define UTXHB0      UART_PORT(0, oUTXHB)
#define URXHB0      UART_PORT(0, oURXHB)
#define UBRDIV0     UART_PORT(0, oUBRDIV)

#define NAND_BASE 0x4E000000
#define NFCONF    (NAND_BASE + 0x00)
#define NFCONT    (NAND_BASE + 0x04)
#define NFCMD     (NAND_BASE + 0x08)
#define NFADDR    (NAND_BASE + 0x0c)
#define NFDATA    (NAND_BASE + 0x10)
#define NFMECCD0  (NAND_BASE + 0x14)
#define NFMECCD1  (NAND_BASE + 0x18)
#define NFSECCD   (NAND_BASE + 0x1c)
#define NFSTAT    (NAND_BASE + 0x20)
#define NFESTAT0  (NAND_BASE + 0x24)
#define NFESTAT1  (NAND_BASE + 0x28)
#define NFMECC0   (NAND_BASE + 0x2c)
#define NFMECC1   (NAND_BASE + 0x30)
#define NFSECC    (NAND_BASE + 0x34)

#define INTERRUPT_BASE 0x4A000000
#define SRCPND     (INTERRUPT_BASE + 0x00)
#define INTMOD     (INTERRUPT_BASE + 0x04)
#define INTMASK    (INTERRUPT_BASE + 0x08)
#define PRIORITY   (INTERRUPT_BASE + 0x0c)
#define INTPND     (INTERRUPT_BASE + 0x10)
#define INTOFFSET  (INTERRUPT_BASE + 0x14)
#define SUBSRCPND  (INTERRUPT_BASE + 0x18)
#define INTSUBMASK (INTERRUPT_BASE + 0x1c)

#endif /* _HAL_S3C2440_REG_H_ */

/******************************************************************************/
// EOF s3c2440_regs.h
