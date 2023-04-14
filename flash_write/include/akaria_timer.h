#ifndef AKARIA_TIMER_H
#define AKARIA_TIMER_H


unsigned int akaria_clock( void );
unsigned int akaria_mcycle( void );
unsigned int akaria_diff_clock( unsigned int end,  unsigned int start );
void akaria_wait( unsigned int t );
void akaria_time_print( void );

#endif
