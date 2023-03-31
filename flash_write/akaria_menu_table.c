#include "akaria_menu.h"


// Add prototype to add menu table
extern void akaria_sf3_test(void);
extern void akaria_flash_write(void);

/*---------------------------------------------------------------*/
//
//  ID          : A string to deciede this element
//  Discription : Explanation for this element
//  Function    : A function called at selected
//
//  ID     Discription                  Function Pointer
/*---------------------------------------------------------------*/
const AkariaMenuTable akaria_menu_table[ AKARIA_MENU_MAX ] = {
  { "f",  "Pmod SF3(SPI0 Flash) test",  akaria_sf3_test},
  { "w",  "Write bootcode to Pmod SF3", akaria_flash_write},
  { "h",  "Help",                       akaria_menu_help  },
  {0, 0, 0}  // Last element
};
  

