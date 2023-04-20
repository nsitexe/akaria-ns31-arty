#ifndef AKARIA_PMODSF3_H_
#define AKARIA_PMODSF3_H_

#include "xspi.h"
#include "xstatus.h"
#include "PmodSF3.h"

#define AKARIA_SF3_WRITE_EXTRA_BYTES       (SF3_WRITE_EXTRA_BYTES + 1)     /* Address is 4byte */
#define AKARIA_SF3_READ_MIN_EXTRA_BYTES    (SF3_READ_MIN_EXTRA_BYTES + 1)  /* Address is 4byte */
#define AKARIA_SF3_READ_MAX_EXTRA_BYTES    (SF3_READ_MAX_EXTRA_BYTES + 1)  /* Address is 4byte */
#define AKARIA_SF3_SECTOR_ERASE_BYTES	 (SF3_SECTOR_ERASE_BYTES + 1)    /* Address is 4byte */

XStatus akaria_SF3_Init(PmodSF3 *InstancePtr, u32 SPI_Address);
XStatus akaria_SF3_FlashWriteEnable(PmodSF3 *InstancePtr);
XStatus akaria_SF3_FlashWrite(PmodSF3 *InstancePtr, u32 Addr, u32 ByteCount,
      u8 WriteCmd, u8 **BufferPtr);
XStatus akaria_SF3_FlashRead(PmodSF3 *InstancePtr, u32 Addr, u32 ByteCount, u8 ReadCmd,
      u8 **BufferPtr);
XStatus akaria_SF3_BulkErase(PmodSF3 *InstancePtr);
XStatus akaria_SF3_SectorErase(PmodSF3 *InstancePtr, u32 Addr);
XStatus akaria_SF3_ReadID(PmodSF3 *InstancePtr);

#endif /* PMODSF3_H_ */
