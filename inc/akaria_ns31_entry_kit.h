#ifndef AKARIA_NS31_ENTRY_H
#define AKARIA_NS31_ENTRY_H

#include "akaria_ns31.h"


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

#endif    // AKARIA_NS31_EKIT_H
