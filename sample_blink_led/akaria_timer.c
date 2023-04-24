#include <stdio.h>
#include "akaria_ns31.h"
#include "akaria_ns31_arty.h"

/*-------------------------------------------------------------------------------------------------------*/
// Name      akaria_clock
// Function  Getting the hardware timer count value. It's just low 32 bits
// Return    Counter value
/*-------------------------------------------------------------------------------------------------------*/
unsigned int akaria_clock( void ){
  volatile unsigned int *p = (unsigned int *)ADDR_MTIME;
  volatile unsigned int t;
  t = *p;
  return t;
}


/*-------------------------------------------------------------------------------------------------------*/
// Name      akaria_diff_clock
// Function  Measuring the different from end to start
// Arg[1]    unsigned int end   : End clock to measurement
// Arg[2]    unsigned int start : Start clock to measurement
// Arg[3]    void (*func)(void)    : Callback user function
// Return    Success : Timer list ID / Error : Minus value
// Note      This function can measure within 0xffffffff difference by hardware clock counter
//           NS31 Entry kit timer works at 25MHz. so, it supports diff time < 171,798 msec
/*-------------------------------------------------------------------------------------------------------*/
unsigned int  akaria_diff_clock( unsigned int end,  unsigned int start ){
  unsigned int t;
  
  if( end > start ){
    t = end - start;
  }
  else{
    t = 0xffffffff - start + 1 + end;
  }
  return t;
}


/*-------------------------------------------------------------------------------------------------------*/
// Name      akaria_wait
// Function  Waiting for setting time (msec)
// Arg[1]    unsigned int t : Wait time (msec)
// Return    None. Retun after waiting time
/*-------------------------------------------------------------------------------------------------------*/
void akaria_wait( unsigned int t ){
  unsigned int setclock = t * SET_TIMER_COUNT;
  unsigned int start, end;

  start = akaria_clock();
  end   = akaria_clock();
  
  while( akaria_diff_clock(end, start) < setclock ){
    end = akaria_clock();
  }
  return;
}


/*-------------------------------------------------------------------------------------------------------*/
// Name      akaria_time_print
// Function  Print msec time called by main menu
// Return    None
/*-------------------------------------------------------------------------------------------------------*/
void akaria_time_print( void ){
  unsigned int cycl_1, cycl_2;
  unsigned int cych;
  unsigned int tml_1, tml_2;
  unsigned int tmh;


  tml_1  = *(volatile unsigned int *)ADDR_MTIME;

  tmh  = *(volatile unsigned int *)(ADDR_MTIME+0x4);

  tml_2  = *(volatile unsigned int *)ADDR_MTIME;
  
  if( tml_2 < tml_1 ){
    tmh  = *(volatile unsigned int *)(ADDR_MTIME+0x4);
  }

  printf("[Sys Cycle]  0x%08x %08x cycle\n", tmh, tml_2);
  
}

