#ifndef AKARIA_MENU_H
#define AKARIA_MENU_H

#define AKARIA_MENU_MAX  64

typedef struct {
  const char *identifier;    // Identifier to select
  const char *discription;   // Discription to print
  void (*func)(void);        // Function pointer
} AkariaMenuTable;

extern void akaria_menu_help( void );
extern int akaria_menu_select( const char *str );
extern void akaria_menu( void );

#endif
