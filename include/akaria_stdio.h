#ifndef AKARIA_STDIO_H
#define AKARIA_STDIO_H

#include <stdio.h>

#ifndef NULL
#define NULL (0)
#endif

#define RETURN_CHAR        '\n'

char *  akaria_gets( const char *str, int size );
char*  akaria_puts_gets( const char *output, const char *input, int num );

#endif    // AKARIA_STDIO_H
