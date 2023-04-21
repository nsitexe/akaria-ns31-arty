#include <stdio.h>
#include "akaria_ns31.h"
#include "akaria_ns31_entry_kit.h"
#include "akaria_led.h"
#include "akaria_menu.h"

int main(void){

  // LED Init
  akaria_led_init();
  
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
