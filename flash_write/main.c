#include <stdio.h>
#include "akaria_ns31.h"
#include "akaria_ns31_arty.h"
#include "akaria_menu.h"

int main(void){

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
