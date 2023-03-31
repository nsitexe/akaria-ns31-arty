/******************************************************************************/
/*                                                                            */
/* PmodSF3.c -- PmodSF3 Driver Header                                         */
/*                                                                            */
/******************************************************************************/
/* Author: Andrew Holzer                                                      */
/* Copyright 2017, Digilent Inc.                                              */
/******************************************************************************/
/* Module Description:                                                        */
/*                                                                            */
/* This file contains function definitions for the PmodSF3 driver.            */
/*                                                                            */
/******************************************************************************/
/* Revision History:                                                          */
/*                                                                            */
/*    ??/??/????(AHolzer):  Created                                           */
/*    07/26/2017(ArtVVB):   Refactored and Validated                          */
/*    03/16/2018(atangzwj): Validated for Vivado 2017.4                       */
/*                                                                            */
/******************************************************************************/

/***************************** Include Files *********************************/
#include <stdio.h>
#include "akaria_PmodSF3.h"
#include "akaria_ns31_entry_kit.h"

/************************** Variable Definitions *****************************/
#ifndef XSPI_TRANSFER_POLLING_MODE
static volatile int TransferInProgress;

// This variable tracks any errors that occur during interrupt processing
static int ErrorCount;
#endif

/*
 * Byte offset value written to Flash. This need to be redefined for writing
 * different patterns of data to the Flash device.
 */

XSpi_Config Akaria_XSpi_SF3Config = {
   0,                   /* Device ID */
   0,                   /* BaseAddress */
   1,                   /* HasFifos */
   0,                   /* SlaveOnly */
   1,                   /* NumSlaveBits */
   XSP_DATAWIDTH_BYTE,  /* DataWidth */
   XSP_STANDARD_MODE,       /* SpiMode */
   0,                   /* AxiInterface */
   0,                   /* AxiFullBaseAddress */
   0,                   /* XipMode */
   0,                   /* Use_Startup */
   16                   /* FifosDepth */
};

/*****************************************************************************
* This function initializes the SF3 device and attaches and enables it's SPI
* interrupt
*
* @param    InstancePtr is a pointer to the instance of the SF3 device.
* @param    ivt is the interrupt vector table containing the SF3 SPI handler as
*              it's first element.
* @param    SPI_Address is the base address of the SF3 SPI device.
*
* @return   XST_SUCCESS if successful else XST_FAILURE.
*
* @note     None
*
******************************************************************************/
//XStatus SF3_begin(PmodSF3 *InstancePtr, INTC *IntcPtr, const ivt_t ivt[],
//      u32 SPI_Address) {
XStatus SF3_begin(PmodSF3 *InstancePtr, u32 SPI_Address) {
   XStatus Status;

   Akaria_XSpi_SF3Config.BaseAddress = SPI_Address;

    //Set SPI0_Select to Standard mode
    akaria_write_reg(SPI0_SEL, SPI0_SEL_STD); //STD Mode

   // Initialize SPI
   Status = SF3_SpiInit(&InstancePtr->SF3Spi);
   if (Status != XST_SUCCESS) {
      printf("Error initializing SPI\n");
      return XST_FAILURE;
   }
   // Enable all interrupts in our interrupt vector table
   // Make sure all driver instances using interrupts are initialized first
#ifdef XSPI_TRANSFER_POLLING_MODE
   XSpi_IntrGlobalDisable(&InstancePtr->SF3Spi);
   printf("SPI Driver is polling mode.\n");
#else
   akaria_extint_initialize(SPI0_EXTINT_NO, XSpi_InterruptHandler, (void *)&InstancePtr->SF3Spi);
   printf("SPI Driver is interrupt mode. \n");
#endif

   /* Enter 4-byte address mode */
   //SF3_Enter4ByteAddressMode(InstancePtr);

   return XST_SUCCESS;
}

/*****************************************************************************
* This function initializes the SPI and performs a self test to ensure that the
* HW was set correctly
*
* @param    SpiPtr is a pointer to the instance of the SPI device.
*
* @return   XST_SUCCESS if successful else XST_FAILURE.
*
* @note     None
*
******************************************************************************/
XStatus SF3_SpiInit(XSpi *SpiPtr) {
   XStatus Status;
   XSpi_Config *ConfigPtr;

   Xil_AssertNonvoid(SpiPtr != NULL);

   ConfigPtr = &Akaria_XSpi_SF3Config;
   if (ConfigPtr == NULL) {
      return XST_DEVICE_NOT_FOUND;
   }

   Status = XSpi_CfgInitialize(SpiPtr, ConfigPtr, ConfigPtr->BaseAddress);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   // Set the handler for the SPI that is called from the interrupt context
#ifndef XSPI_TRANSFER_POLLING_MODE
   XSpi_SetStatusHandler(SpiPtr, SpiPtr, (XSpi_StatusHandler) SF3_SpiHandler);
#endif

   Status = XSpi_SetOptions(SpiPtr,
         (XSP_MASTER_OPTION | XSP_MANUAL_SSELECT_OPTION ));
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   Status = XSpi_SetSlaveSelect(SpiPtr, 1);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   // Start the SPI driver, enabling the device
   Status = XSpi_Start(SpiPtr);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }
   return XST_SUCCESS;
}

/******************************************************************************
* This function enables writes to the Winbond Serial Flash memory.
*
* @param    InstancePtr is a pointer to the instance of the PmodSF3 device.
*
* @return   XST_SUCCESS if successful else XST_FAILURE.
*
* @note     None
*
******************************************************************************/
XStatus SF3_FlashWriteEnable(PmodSF3 *InstancePtr) {
   XStatus Status;
   u8 Buffer[SF3_WRITE_ENABLE_BYTES];

   Status = SF3_WaitForFlashReady(InstancePtr);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   Buffer[0] = SF3_COMMAND_WRITE_ENABLE;

#ifndef XSPI_TRANSFER_POLLING_MODE
   TransferInProgress = TRUE;
#endif
   Status = XSpi_Transfer(&InstancePtr->SF3Spi, Buffer, NULL,
         SF3_WRITE_ENABLE_BYTES);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

#ifndef XSPI_TRANSFER_POLLING_MODE
   while (TransferInProgress)
      ;
   if (ErrorCount != 0) {
      ErrorCount = 0;
      return XST_FAILURE;
   }
#endif

   return XST_SUCCESS;
}

/******************************************************************************
* This function disables writes to the Serial Flash memory.
*
* @param    InstancePtr is a pointer to the instance of the PmodSF3 device.
*
* @return   XST_SUCCESS if successful else XST_FAILURE.
*
* @note     None
*
******************************************************************************/
XStatus SF3_FlashWriteDisable(PmodSF3 *InstancePtr) {
   XStatus Status;
   u8 Buffer[SF3_WRITE_DISABLE_BYTES];

   Status = SF3_WaitForFlashReady(InstancePtr);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   Buffer[0] = SF3_COMMAND_WRITE_DISABLE;

#ifndef XSPI_TRANSFER_POLLING_MODE
   TransferInProgress = TRUE;
#endif

   Status = XSpi_Transfer(&InstancePtr->SF3Spi, Buffer, NULL,
         SF3_WRITE_DISABLE_BYTES);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

#ifndef XSPI_TRANSFER_POLLING_MODE
   while (TransferInProgress)
      ;
   if (ErrorCount != 0) {
      ErrorCount = 0;
      return XST_FAILURE;
   }
#endif

   return XST_SUCCESS;
}

/*****************************************************************************
* This function writes the data to the specified locations in the Serial Flash
* memory.
*
* @param    InstancePtr is a pointer to the instance of the PmodSF3 device.
* @param    Addr is the address in the Buffer, where to write the data.
* @param    ByteCount is the number of bytes to be written.
* @param    WriteCmd selects which command to use to write with.
* @param    BufferPtr is the pointer to a data buffer
*              (minimum size ByteCount + SF3_WRITE_EXTRA_BYTES).
*
* @return   XST_SUCCESS if successful else XST_FAILURE.
*
* @note     The Buffer at BufferPtr is moved forward by the number of command bytes used
*
******************************************************************************/
XStatus SF3_FlashWrite(PmodSF3 *InstancePtr, u32 Addr, u32 ByteCount,
      u8 WriteCmd, u8 **BufferPtr) {
   XStatus Status;

   Status = SF3_WaitForFlashReady(InstancePtr);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   // Prepare the buffer with write command data
   // NS31A is 32bit Address mode
   (*BufferPtr)[0] = WriteCmd;
   (*BufferPtr)[1] = (u8) (Addr >> 24);
   (*BufferPtr)[2] = (u8) (Addr >> 16);
   (*BufferPtr)[3] = (u8) (Addr >> 8);
   (*BufferPtr)[4] = (u8) (Addr);

#ifndef XSPI_TRANSFER_POLLING_MODE
   TransferInProgress = TRUE;
#endif
   Status = XSpi_Transfer(&InstancePtr->SF3Spi, *BufferPtr, NULL,
         (ByteCount + SF3_WRITE_EXTRA_BYTES));

   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

#ifndef XSPI_TRANSFER_POLLING_MODE
   while (TransferInProgress)
      ;
   if (ErrorCount != 0) {
      ErrorCount = 0;
      return XST_FAILURE;
   }
#endif

   // BufferPtr should point to the beginning of the data written
   *BufferPtr += SF3_WRITE_EXTRA_BYTES;

   return XST_SUCCESS;
}



/*****************************************************************************
* This function reads the data from the Serial Flash Memory.
*
* @param    InstancePtr is a pointer to the instance of the PmodSF3 device.
* @param    Addr is the starting address in the Flash Memory from which the data
*              is to be read.
* @param    ByteCount is the number of bytes to be read.
* @param    ReadCmd selects which command to use to read with.
* @param    BufferPtr is a pointer to the buffer which data is to be read into,
*              expected size of ByteCount + DummyBytes(ReadCmd).
*
* @return   XST_SUCCESS if successful else XST_FAILURE.
*
* @note     None
*
******************************************************************************/
XStatus SF3_FlashRead(PmodSF3 *InstancePtr, u32 Addr, u32 ByteCount, u8 ReadCmd,
      u8 **BufferPtr) {
   XStatus Status;
   u32 DummyBytes;

   Status = SF3_WaitForFlashReady(InstancePtr);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   // Prepare the buffer with the command data
   // 4-byte Address Mode
   (*BufferPtr)[0] = ReadCmd;
   (*BufferPtr)[1] = (u8) (Addr >> 24);
   (*BufferPtr)[2] = (u8) (Addr >> 16);
   (*BufferPtr)[3] = (u8) (Addr >> 8);
   (*BufferPtr)[4] = (u8) (Addr);

   // Select the number of additional bytes required before data is passed
   // depending on the command
   #if 0
   switch (ReadCmd) {
   case SF3_COMMAND_DUAL_READ:
   case SF3_COMMAND_DUAL_IO_READ:
      DummyBytes = SF3_DUAL_READ_DUMMY_BYTES;
      break;
   case SF3_COMMAND_QUAD_IO_READ:
      DummyBytes = SF3_QUAD_IO_READ_DUMMY_BYTES;
      break;
   case SF3_COMMAND_QUAD_READ:
      DummyBytes = SF3_QUAD_READ_DUMMY_BYTES;
      break;
   default:
      DummyBytes = 0;
      break;
   }
   #endif

   if(ReadCmd == SF3_COMMAND_FAST_READ) DummyBytes = 1; /* Dummy clock cycles = 8 */
   else DummyBytes = 0;

#ifndef XSPI_TRANSFER_POLLING_MODE
   TransferInProgress = TRUE;
#endif
   Status = XSpi_Transfer(&InstancePtr->SF3Spi, *BufferPtr, *BufferPtr,
         (ByteCount + DummyBytes + SF3_READ_MIN_EXTRA_BYTES));
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

#ifndef XSPI_TRANSFER_POLLING_MODE
   while (TransferInProgress)
      ;

   if (ErrorCount != 0) {
      ErrorCount = 0;
      return XST_FAILURE;
   }
#endif

   // Increment the pointer forward to the beginning of data
   *BufferPtr += DummyBytes + SF3_READ_MIN_EXTRA_BYTES;

   return XST_SUCCESS;
}

/*****************************************************************************
* This function erases the entire contents of the Micron Serial Flash device.
*
* @param    InstancePtr is a pointer to the instance of the PmodSF3 device.
*
* @return   XST_SUCCESS if successful else XST_FAILURE.
*
* @note     The erased bytes will read as 0xFF.
*
******************************************************************************/
XStatus SF3_BulkErase(PmodSF3 *InstancePtr) {
   XStatus Status;
   u8 Buffer[SF3_BULK_ERASE_BYTES];

   Status = SF3_WaitForFlashReady(InstancePtr);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   Buffer[0] = SF3_COMMAND_BULK_ERASE;


#ifndef XSPI_TRANSFER_POLLING_MODE
   TransferInProgress = TRUE;
#endif
   Status = XSpi_Transfer(&InstancePtr->SF3Spi, Buffer, NULL,
         SF3_BULK_ERASE_BYTES);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

#ifndef XSPI_TRANSFER_POLLING_MODE
   while (TransferInProgress)
      ;

   if (ErrorCount != 0) {
      ErrorCount = 0;
      return XST_FAILURE;
   }
#endif

   return XST_SUCCESS;
}

/******************************************************************************
* This function erases the contents of an entire 64KB sector.
*
* @param    InstancePtr is a pointer to the instance of the PmodSF3 device.
* @param    Addr is the address of the first byte in the sector to be erased.
*
* @return   XST_SUCCESS if successful else XST_FAILURE.
*
* @note     None
*
******************************************************************************/
XStatus SF3_SectorErase(PmodSF3 *InstancePtr, u32 Addr) {
   XStatus Status;
   u8 Buffer[SF3_SECTOR_ERASE_BYTES];

   Status = SF3_WaitForFlashReady(InstancePtr);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   // Prepare the buffer with the command data
   Buffer[0] = SF3_COMMAND_SECTOR_ERASE;
   Buffer[1] = (u8) (Addr >> 24);
   Buffer[2] = (u8) (Addr >> 16);
   Buffer[3] = (u8) (Addr >> 8);
   Buffer[4] = (u8) (Addr);


#ifndef XSPI_TRANSFER_POLLING_MODE
   TransferInProgress = TRUE;
#endif
   Status = XSpi_Transfer(&InstancePtr->SF3Spi, Buffer, NULL,
         SF3_SECTOR_ERASE_BYTES);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

#ifndef XSPI_TRANSFER_POLLING_MODE
   while (TransferInProgress)
      ;
   if (ErrorCount != 0) {
      ErrorCount = 0;
      return XST_FAILURE;
   }
#endif

   return XST_SUCCESS;
}

/******************************************************************************
* This function captures the status register of the SF3 and stores it in
* InstancePtr->Status.
*
* @param    InstancePtr is a pointer to the instance of the PmodSF3 device.
*
* @return   XST_SUCCESS if successful else XST_FAILURE.
*
* @note     This function reads the status register of the Buffer and waits
*           until the WIP bit of the status register becomes 0.
*
******************************************************************************/
XStatus SF3_GetStatus(PmodSF3 *InstancePtr) {
   XStatus Status;

   u8 Buffer[SF3_STATUS_READ_BYTES];

   Buffer[0] = SF3_COMMAND_READ_STATUSREG;

#ifndef XSPI_TRANSFER_POLLING_MODE
   TransferInProgress = TRUE;
#endif

   Status = XSpi_Transfer(&InstancePtr->SF3Spi, Buffer, Buffer,
         SF3_STATUS_READ_BYTES);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

#ifndef XSPI_TRANSFER_POLLING_MODE
   while (TransferInProgress)
      ;
   if (ErrorCount != 0) {
      ErrorCount = 0;
      return XST_FAILURE;
   }
#endif

   InstancePtr->Status = Buffer[1];

   return XST_SUCCESS;
}

/******************************************************************************
* This function waits until the serial Flash is ready to accept next command.
*
* @param    InstancePtr is a pointer to the instance of the PmodSF3 device.
*
* @return   XST_SUCCESS if successful else XST_FAILURE.
*
* @note     This function reads the status register of the Buffer and waits
*           until the WIP bit of the status register becomes 0.
*
******************************************************************************/
XStatus SF3_WaitForFlashReady(PmodSF3 *InstancePtr) {
   XStatus Status;
   u8 StatusReg;

   while (1) {
      /*
       * Get the Status REgister. The status register content is
       * stored at the second byte pointed by the ReadBuffer.
       */
      Status = SF3_GetStatus(InstancePtr);
      if (Status != XST_SUCCESS) {
         return XST_FAILURE;
      }

      /*
       * Check to see if the flash is ready to accept the next command.
       * If so, break and return successfully;
       */
      StatusReg = InstancePtr->Status;
      if ((StatusReg & 0x01) == 0) {
         break;
      }
   }

   return XST_SUCCESS;
}

/*****************************************************************************
* This function reads the ID register of the serial Flash
*
* @param    InstancePtr is a pointer to the instance of the PmodSF3 device.
*
* @return   XST_SUCCESS if successful else XST_FAILURE.
*
* @note     None
*
******************************************************************************/
XStatus SF3_ReadID(PmodSF3 *InstancePtr) {
   XStatus Status;
   int i;
   u8 Buffer[SF3_ID_READ_BYTES];

   Buffer[0] = SF3_COMMAND_READ_ID;

#ifndef XSPI_TRANSFER_POLLING_MODE
   TransferInProgress = TRUE;
#endif

   Status = XSpi_Transfer(&InstancePtr->SF3Spi, Buffer, Buffer, SF3_ID_READ_BYTES);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

#ifndef XSPI_TRANSFER_POLLING_MODE
   while (TransferInProgress)
      ;
   if (ErrorCount != 0) {
      ErrorCount = 0;
      return XST_FAILURE;
   }
#endif

   puts("ID =");
   /* Skip extra bytes */
   for (i = 1; i < SF3_ID_READ_BYTES; i++)
      printf(" %x", Buffer[i]);
      puts("\n");
   return XST_SUCCESS;
}

/*****************************************************************************
* This function is the handler which performs processing for the SPI driver.
* It is called from an interrupt context such that the amount of processing
* performed should be minimized. It is called when a transfer of SPI data
* completes or an error occurs.
*
* This handler provides an example of how to handle SPI interrupts and
* is application specific.
*
* @param    CallBackRef is the upper layer callback reference passed back
*           when the callback function is invoked.
* @param    StatusEvent is the event that just occurred.
* @param    ByteCount is the number of bytes transferred up until the event
*           occurred.
*
* @return   None.
*
* @note     None.
*
******************************************************************************/
#ifndef XSPI_TRANSFER_POLLING_MODE
void SF3_SpiHandler(void *CallBackRef, u32 StatusEvent,
      unsigned int ByteCount) {
   TransferInProgress = FALSE;

   if (StatusEvent != XST_SPI_TRANSFER_DONE) {
      ErrorCount++;
   }
}
#endif