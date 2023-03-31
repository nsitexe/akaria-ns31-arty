#ifndef AKARIA_NS31_H
#define AKARIA_NS31_H


#define USE_AKARIA_NS31_ENTRY_KIT
// #include "tx_user.h"   // When we use ThreadX



#define  MIE_MASK    0xF
#define  MIE_ENABLE  0x8
#define  MIE_DISABLE 0x0



#ifdef USE_AKARIA_NS31_ENTRY_KIT
/* NS31 Entry Kit on Arty-A7 */
#define  ADDR_MTIMECMP_L 0x02000200
#define  ADDR_MTIME_L    0x02000308
#else    // USE_AKARIA_NS31_ENTRY_KIT
/* SPIKE */
#define  ADDR_MTIMECMP_L 0x02004000
#define  ADDR_MTIME_L    0x0200BFF8
#endif    // USE_AKARIA_NS31_ENTRY_KIT

#define  ADDR_MCYCLE     0x0000AC00
#define  ADDR_MCYCLE_H   0x0000AE00


#define  SET_TIMER_COUNT 25000    // 1msec is made by 25,000 cycles on 25MHz 
#define  CLOCKS_PER_SEC  (SET_TIMER_COUNT*1000)    // 1sec is made by SET_TIMER_COUNT * 1000


#define  SET_MTIE 0x80



#endif    // AKARIA_NS31_H
