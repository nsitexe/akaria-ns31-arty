/******************************************************************************/
/*                                                                            */
/* PmodSF3.h -- PmodSF3 Driver Header                                         */
/*                                                                            */
/******************************************************************************/
/* Author: Andrew Holzer                                                      */
/* Copyright 2017, Digilent Inc.                                              */
/******************************************************************************/
/* Module Description:                                                        */
/*                                                                            */
/* This file contains driver prototypes, types, and macros for the PmodSF3    */
/* driver source.	                                                          */
/*                                                                            */
/******************************************************************************/
/* Revision History:                                                          */
/*                                                                            */
/*    ??/??/????(AHolzer):  Created                                           */
/*    07/26/2017(ArtVVB):   Refactored and Validated                          */
/*    03/16/2018(atangzwj): Validated for Vivado 2017.4                       */
/*                                                                            */
/******************************************************************************/

#ifndef PMODSF3_H_
#define PMODSF3_H_

/***************************** Include Files *********************************/

#include "xspi.h"
#include "xstatus.h"

#define SF3_MANUFACTURER_ID 0x20

/************* Pmod SF3 SPI Flash(Micron) Command Definition *****************/
#define SF3_COMMAND_PAGE_PROGRAM 	0x02 // Page program command
#define SF3_COMMAND_READ 	            0x03 // Read command
#define SF3_COMMAND_FAST_READ             0x0B // Fast Read 
#define SF3_COMMAND_WRITE_ENABLE 	      0x06 // Write Enable command
#define SF3_COMMAND_WRITE_DISABLE 	      0x04 // Write Enable command
#define SF3_COMMAND_SECTOR_ERASE	      0xD8 // Sector Erase command
#define SF3_COMMAND_BULK_ERASE		0xC7 // Bulk Erase command
#define SF3_COMMAND_READ_STATUSREG 	      0x05 // Status read command
#define SF3_COMMAND_READ_ID 		      0x9E // ID read command

/*
 * These definitions specify the EXTRA bytes in each of the command
 * transactions. This count includes the Command byte, address bytes and any
 * don't care bytes needed.
 */
#define SF3_WRITE_EXTRA_BYTES       5  /* 1byte(COMMAND) + 4byte(Address) */
#define SF3_READ_MIN_EXTRA_BYTES    5  /* 1byte(COMMAND) + 4byte(Address) */
#define SF3_READ_MAX_EXTRA_BYTES    6 /* 1byte(COMMAND + 4byte(Address) + 1byte(Dummy Cycles) */ 
#define SF3_WRITE_ENABLE_BYTES 	1  /* 1byte(COMMAND) */
#define SF3_WRITE_DISABLE_BYTES 	1  /* 1byte(COMMAND) */
#define SF3_SECTOR_ERASE_BYTES	5  /* 1byte(COMMAND) + 4byte(Address) */
#define SF3_BULK_ERASE_BYTES        1  /* 1byte(COMMAND) */
#define SF3_STATUS_READ_BYTES	      2  /* 1byte(COMMAND) + 1byte(Data) */
#define SF3_STATUS_WRITE_BYTES	 2 /* 1byte(COMMAND) + 1byte(Data) */
#define SF3_ID_READ_BYTES		 21 /* 1byte(COMMAND) + 20byte(Data) */

// Flash not busy mask in the status register of the flash device
#define SF3_FLASH_SR_IS_READY_MASK 0x01

// Number of bytes per page in the flash device
#define SF3_PAGE_SIZE               256 /* 256Bytes */

#define SF3_PAGES_PER_SUBSECTOR     16  /* SubSector = 4KB */
#define SF3_SUBSECTORS_PER_SECTOR	16  /* 8192 subsector / 512 sector */
#define SF3_NUM_SECTORS			8192
#define SF3_MAX_ADDRESS 		0x1FFFFFF

// Address of the page to perform erase, write and read operations.
#define SF3_FLASH_TEST_ADDRESS	0x004000

typedef struct PmodSF3 {
   XSpi SF3Spi;
   u8 Status;
} PmodSF3;

/************************** Function Prototypes ******************************/
XStatus SF3_begin(PmodSF3 *InstancePtr, u32 SPI_Address);
XStatus SF3_SpiInit(XSpi *SpiPtr);
XStatus SF3_FlashWriteEnable(PmodSF3 *InstancePtr);
XStatus SF3_FlashWriteDisable(PmodSF3 *InstancePtr);
XStatus SF3_FlashWrite(PmodSF3 *InstancePtr, u32 Addr, u32 ByteCount,
      u8 WriteCmd, u8 **BufferPtr);
XStatus SF3_FlashRead(PmodSF3 *InstancePtr, u32 Addr, u32 ByteCount, u8 ReadCmd,
      u8 **BufferPtr);
XStatus SF3_BulkErase(PmodSF3 *InstancePtr);
XStatus SF3_SectorErase(PmodSF3 *InstancePtr, u32 Addr);
XStatus SF3_GetStatus(PmodSF3 *InstancePtr);
XStatus SF3_WaitForFlashReady(PmodSF3 *InstancePtr);
XStatus SF3_ReadID(PmodSF3 *InstancePtr);

void SF3_SpiHandler(void *CallBackRef, u32 StatusEvent, unsigned int ByteCount);

#endif /* PMODSF3_H_ */
