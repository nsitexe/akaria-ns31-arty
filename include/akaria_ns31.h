#ifndef AKARIA_NS31_H
#define AKARIA_NS31_H

#define  MIE_MASK    0xF
#define  MIE_ENABLE  0x8
#define  MIE_DISABLE 0x0

#define  ADDR_MTIMECMP 0x02004000
#define  ADDR_MTIME    0x0200BFF8

#define  SET_TIMER_COUNT 25000                     // 1msec is made by 25,000 cycles on 25MHz 
#define  CLOCKS_PER_SEC  (SET_TIMER_COUNT*1000)    // 1sec is made by SET_TIMER_COUNT * 1000


#define  SET_MTIE 0x80



#endif    // AKARIA_NS31_H
