#include <stdio.h>
#include "akaria_ns31.h"
#include "akaria_ns31_entry_kit.h"
#include "akaria_menu.h"

/***
void  akaria_1sec_led( void ) {
  static unsigned int cnt = 0;
  
  cnt++;

  if(cnt & 0x1){
    akaria_led_on(LED7);
  }
  else{
    akaria_led_off(LED7);
  }
  return;  
}
***/


int main(void){

  // LED Init
  akaria_led_init();
  
  // LED7 blinks 1sec interval
  //akaria_timer_set(500, 0, akaria_1sec_led);

  printf("\n");
  printf("#####################################################################\n");
  printf("#                                                                   #\n");
  printf("#   A k a r i a   N S   B o a r d   S u p p o r t   P a c k a g e   #\n"); 
  printf("#                                                                   #\n");
  printf("#####################################################################\n");
  printf("\n");

  akaria_menu_help();

  while(1){
    akaria_menu();
  }

}
