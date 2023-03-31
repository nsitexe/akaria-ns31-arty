#include "akaria_menu.h"


// Add prototype to add menu table
extern void akaria_led_test(void);
extern void akaria_time_print(void);
extern void akaria_dhrystone(void);


/*---------------------------------------------------------------*/
//
//  ID          : A string to deciede this element
//  Discription : Explanation for this element
//  Function    : A function called at selected
//
//  ID     Discription                  Function Pointer
/*---------------------------------------------------------------*/
const AkariaMenuTable akaria_menu_table[ AKARIA_MENU_MAX ] = {
  { "0",  "LED test",                   akaria_led_test   },
  /*  { "d",  "Dhrystone",                  akaria_dhrystone  }, */
  { "t",  "Print system clock",         akaria_time_print }, 
  { "h",  "Help",                       akaria_menu_help  },
  {0, 0, 0}  // Last element
};
  

