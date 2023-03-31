#include <stdio.h>
#include <string.h>

#include "akaria_menu.h"
#include "akaria_stdio.h"

extern const AkariaMenuTable akaria_menu_table[];


void akaria_menu_help(void){
  int i;

  printf(" ==== Select ID to execute each function ================\n");
  printf("      ID        Discription \n");
  printf(" --------------------------------------------------------\n");

  for(i=0; i<AKARIA_MENU_MAX; i++){
    
    if( akaria_menu_table[i].identifier[0] == 0 ){
      break;
    }
    else {
      printf("    * %s :        %s \n", akaria_menu_table[i].identifier, akaria_menu_table[i].discription);
    }
  }
  printf(" ========================================================\n");  
}


int akaria_menu_select( const char *str ){

  int i;
  
  for(i=0; i<AKARIA_MENU_MAX; i++){
    
    if( akaria_menu_table[i].identifier[0] == 0 ){
      return -1;
    }

    if( strcmp(str, akaria_menu_table[i].identifier) == 0 ){
      return i;
    }
  }
  
  return -1;
}



void akaria_menu( void ){
  char sel[32];
  int  id;
  char c;

  while( akaria_puts_gets( "Akaria> ", sel, 32 ) == NULL){;}
  
  // Get ID
  id = akaria_menu_select( sel );
      
  if(id < 0){
    printf("!!!ERROR!!! Invalid input: ");
    printf(sel);
    printf("\n");
  }
  else{
    printf("\n");
    printf("Select: %s \n", akaria_menu_table[id].discription);
    printf("\n");
    akaria_menu_table[id].func();
  }

  return;
}


