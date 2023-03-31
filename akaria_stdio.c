#include <stdio.h>
#include "akaria_stdio.h"

/*-------------------------------------------------------------------------------------------------------*/
/* Name      akaria_gets                                                                                 */
/* Function  Input 1 line from keyboard                                                                  */
/* Arg[1]    char * : Pointer to save read charactors                                                    */
/* Arg[2]    int : Number of charactors to read                                                          */
/* Return    NULL : Error  /  Pointer read buffer                                                        */
/*-------------------------------------------------------------------------------------------------------*/

char *  akaria_gets( const char *str, int size ){
  char *s = (char *)str;
  int num = 0;
  char c;

  while(1){

    /* Read buf */
    c = (char)getchar();

    /* Check read data */
    if( c == RETURN_CHAR ){
      s[num] = '\0';
      printf("\n");
      break;
    }
    else if( c == '\b' ){
      if(num > 0){
        // Left cursor -> space -> left cursor again
        printf("\033[D \033[D"); fflush(stdout);  
        num--;
      }
    }
    else{
      s[num] = c;
      putchar(c); fflush(stdout);
      num++;
      if( num >= size ){
        return NULL;
      }
    }
  }

  return (char *)str;
}


/*-------------------------------------------------------------------------------------------------------*/
/* Name      akaria_puts_gets                                                                            */
/* Function  Input stirng from keyboad after output string. Continue until getting input                 */
/* Arg[1]    const char * : Pointer to print string                                                      */
/* Arg[2]    const char * : Pointer to save read charactors                                              */
/* Arg[3]    int : Number of charactors to read                                                          */
/* Return    NULL : Error  /  Pointer read buffer                                                        */
/*-------------------------------------------------------------------------------------------------------*/

char*  akaria_puts_gets( const char *output, const char *input, int num ){
  char *ret;
  while(1){
    printf(output); fflush(stdout);
    ret = akaria_gets(input, num);
    
    if( ret == NULL ){
      printf("\n!!!ERROR!!! Input buffer is full, try again\n");
    }
    else if(ret[0] != '\0'){
      return ret;
    }
  }
}


