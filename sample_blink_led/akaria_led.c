#include "akaria_ns31.h"
#include "akaria_ns31_arty.h"

void  akaria_led_init(void){
  volatile unsigned int *p_GPIO_TRI  = (unsigned int *)GPIO_TRI;
  volatile unsigned int *p_GPIO_DATA = (unsigned int *)GPIO_DATA;  
  *p_GPIO_TRI  = (unsigned int)0x0;
  *p_GPIO_DATA = 0x0;
  return;
}

void akaria_led_on(unsigned int led){
  volatile unsigned int *pt = (unsigned int *)GPIO_DATA;
  unsigned int dt;
  dt = *pt;
  dt |= (led);
  dt &= 0xff;
  *pt = dt;
  return;
}

void akaria_led_off(unsigned int led){
  volatile unsigned int *pt = (unsigned int *)GPIO_DATA;
  unsigned int dt;
  dt = *pt;
  dt &= ~(led);
  dt &= 0xff;
  *pt = dt;
  return;
}


