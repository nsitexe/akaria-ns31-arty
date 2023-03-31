#ifndef AKARIA_NS31_ENTRY_H
#define AKARIA_NS31_ENTRY_H

#include "akaria_ns31.h"


/*********************************************************************************************
 *
 * COMMON
 *
 *********************************************************************************************/
//unsigned int akaria_read_reg(unsigned int addr);
//void akaria_write_reg(unsigned int addr, unsigned int value);
#define akaria_read_reg(addr)          *(unsigned int *)(addr)
#define akaria_write_reg(addr, value) *(unsigned int *)(addr) = (unsigned int)(value)

/*********************************************************************************************
 *
 * LED
 *
 *********************************************************************************************/
#define GPIO_TRI   0xE4001004
#define GPIO_DATA  0xE4001000

#define LED0 0x01
#define LED1 0x02
#define LED2 0x04
#define LED3 0x08
#define LED4 0x10
#define LED5 0x20
#define LED6 0x40
#define LED7 0x80

extern void akaria_led_init(void);
extern void akaria_led_on(unsigned int);
extern void akaria_led_off(unsigned int);

/*********************************************************************************************
 *
 * SPI
 *
 *********************************************************************************************/
/* Interrupt Number */
#define SPI0_EXTINT_NO     (11)         /* External Input[11] */

/* SPI0 Select*/
#define SPI0_SEL           (0xE4000000) /* SPI0 Mode Select Register */
#define SPI0_SEL_XIP           (0x0) /* bit[1] = 0 */
#define SPI0_SEL_STD           (0x2) /* bit[1] = 1 */

/* SPI0 STD (0xE4004000) */
#define SPI0_BASE_ADDR      (0xE4004000)

/* spi driver transfer mode */
#define XSPI_TRANSFER_POLLING_MODE

#endif    // AKARIA_NS31_EKIT_H
