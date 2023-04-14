
#include "akaria_ns31.h"
#include "akaria_ns31_entry_kit.h"
#include "akaria_timer.h"

void akaria_led_test(void){
  int i;
  
  akaria_led_init();
  akaria_led_on(LED0);
  akaria_wait(500);
  akaria_led_on(LED1);
  akaria_wait(500);
  akaria_led_on(LED2);
  akaria_wait(500);
  akaria_led_on(LED3);
  akaria_wait(500);
  akaria_led_on(LED4);
  akaria_wait(500);
  akaria_led_on(LED5);
  akaria_wait(500);
  akaria_led_on(LED6);
  akaria_wait(500);
  akaria_led_on(LED7);
  akaria_wait(500);
  akaria_led_off(LED0);
  akaria_wait(500);
  akaria_led_off(LED1);
  akaria_wait(500);
  akaria_led_off(LED2);
  akaria_wait(500);
  akaria_led_off(LED3);
  akaria_wait(500);
  akaria_led_off(LED4);
  akaria_wait(500);
  akaria_led_off(LED5);
  akaria_wait(500);
  akaria_led_off(LED6);
  akaria_wait(500);
  akaria_led_off(LED7);
  akaria_wait(500);
  
  for (int i=0 ; i < 2; i++){
    akaria_led_on(LED1 | LED0);
    akaria_led_on(LED7 | LED6);
    akaria_wait(1000);
    akaria_led_off(LED1 | LED0);
    akaria_led_off(LED7 | LED6);
    akaria_led_on(LED3 | LED2);
    akaria_led_on(LED5 | LED4);
    akaria_wait(1000);
    akaria_led_off(LED3 | LED2);
    akaria_led_off(LED5 | LED4);
  }
  for (int i=0 ; i < 2; i++){
    akaria_led_on(LED2 | LED0);
    akaria_led_on(LED7 | LED5);
    akaria_wait(1000);
    akaria_led_off(LED2 | LED0);
    akaria_led_off(LED7 | LED5);
    akaria_led_on(LED3 | LED1);
    akaria_led_on(LED6 | LED4);
    akaria_wait(1000);
    akaria_led_off(LED3 | LED1);
    akaria_led_off(LED6 | LED4);
  }
  for (int i=0 ; i < 2; i++){
    akaria_led_on(LED3 | LED2 | LED1 | LED0);
    akaria_wait(500);
    akaria_led_off(LED3 | LED2 | LED1 | LED0);
    akaria_led_on(LED7 | LED6 | LED5 | LED4);
    akaria_wait(500);
    akaria_led_off(LED7 | LED6 | LED5 | LED4);
  }
  for (int i=0 ; i < 4; i++){
    akaria_led_on(LED3 | LED2 | LED1 | LED0);
    akaria_wait(250);
    akaria_led_off(LED3 | LED2 | LED1 | LED0);
    akaria_led_on(LED7 | LED6 | LED5 | LED4);
    akaria_wait(250);
    akaria_led_off(LED7 | LED6 | LED5 | LED4);
  }
  for (int i=0 ; i < 4; i++){
    akaria_led_on(LED3 | LED2 | LED1 | LED0);
    akaria_wait(125);
    akaria_led_off(LED3 | LED2 | LED1 | LED0);
    akaria_led_on(LED7 | LED6 | LED5 | LED4);
    akaria_wait(125);
    akaria_led_off(LED7 | LED6 | LED5 | LED4);
  }

  akaria_led_on(LED7 | LED6 | LED5 | LED4 | LED3 | LED2 | LED1 | LED0);
  akaria_wait(1000);
  akaria_led_off(LED7 | LED6 | LED5 | LED4 | LED3 | LED2 | LED1 | LED0);
  
  return;
}

