#include <stdio.h>
#include "akaria_ns31_entry_kit.h"
#include "akaria_PmodSF3.h"

XSpi_Config Akaria_XSpi_SF3Config = {
   0,                   /* Device ID */
   0,                   /* BaseAddress */
   1,                   /* HasFifos */
   0,                   /* SlaveOnly */
   1,                   /* NumSlaveBits */
   XSP_DATAWIDTH_BYTE,  /* DataWidth */
   XSP_STANDARD_MODE,   /* SpiMode */
   0,                   /* AxiInterface */
   0,                   /* AxiFullBaseAddress */
   0,                   /* XipMode */
   0,                   /* Use_Startup */
   16                   /* FifosDepth */
};

/* Prototype declarations for functions used internally */
XStatus akaria_SPI_Init(XSpi *SpiPtr);
XStatus akaria_SF3_GetStatus(PmodSF3 *InstancePtr);      
XStatus akaria_SF3_WaitForFlashReady(PmodSF3 *InstancePtr);

/*-------------------------------------------------------------------------------------------------------*/
/* Name      akaria_SF3_Init                                                                             */
/* Function  initializes the SF3 device and attaches and starts it's SPI driver                          */
/* Arg[1]    PmodSF3 * : A pointer to the instance of the SF3 device.                                    */
/* Arg[2]    u32 : The base address of the SF3 SPI device.                                               */
/* Return    XStatus : XST_SUCCESS if successful else XST_FAILURE.                                       */
/*-------------------------------------------------------------------------------------------------------*/

XStatus akaria_SF3_Init(PmodSF3 *InstancePtr, u32 SPI_Address) {
   XStatus Status;

   Akaria_XSpi_SF3Config.BaseAddress = SPI_Address;

    //Set SPI0_Select to Standard mode
    akaria_write_reg(SPI0_SEL, SPI0_SEL_STD); //STD Mode

   // Initialize SPI
   Status = akaria_SPI_Init(&InstancePtr->SF3Spi);
   if (Status != XST_SUCCESS) {
      printf("Error initializing SPI\n");
      return XST_FAILURE;
   }

   XSpi_IntrGlobalDisable(&InstancePtr->SF3Spi);

   return XST_SUCCESS;
}

/*-------------------------------------------------------------------------------------------------------*/
/* Name      akaria_SPI_Init                                                                             */
/* Function  Initializes the SPI and performs a self test to ensure that the HW was set correctly        */
/* Arg[1]    XSpi * : A pointer to the instance of the SPI device.                                       */
/* Return    XStatus : XST_SUCCESS if successful else XST_FAILURE.                                       */
/*-------------------------------------------------------------------------------------------------------*/
XStatus akaria_SPI_Init(XSpi *SpiPtr) {
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

/*-------------------------------------------------------------------------------------------------------*/
/* Name      akaria_SF3_FlashWriteEnable                                                                 */
/* Function  enables writes to the Serial Flash memory.                                                  */
/* Arg[1]    PmodSF3 * : A pointer to the instance of the PmodSF3 device.                                */
/* Return    XStatus : XST_SUCCESS if successful else XST_FAILURE.                                       */
/*-------------------------------------------------------------------------------------------------------*/
XStatus akaria_SF3_FlashWriteEnable(PmodSF3 *InstancePtr) {
   XStatus Status;
   u8 Buffer[SF3_WRITE_ENABLE_BYTES];

   Status = akaria_SF3_WaitForFlashReady(InstancePtr);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   Buffer[0] = SF3_COMMAND_WRITE_ENABLE;

   Status = XSpi_Transfer(&InstancePtr->SF3Spi, Buffer, NULL,
         SF3_WRITE_ENABLE_BYTES);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   return XST_SUCCESS;
}

/*-------------------------------------------------------------------------------------------------------*/
/* Name      akaria_SF3_FlashWrite                                                                       */
/* Function  Writes the data to the specified locations in the Serial Flash memory.                      */
/* Arg[1]    PmodSF3 * : A pointer to the instance of the PmodSF3 device.                                */
/* Arg[2]    u32       : The address in the Buffer, where to write the data.                             */
/* Arg[3]    u32       : The number of bytes to be written.                                              */
/* Arg[4]    u8        : Selects which command to use to write with.                                     */
/* Arg[5]    u8 **     : The pointer to a data buffer                                                    */
/*                       (minimum size ByteCount + AKARIA_SF3_WRITE_EXTRA_BYTES).                        */
/* Return    XStatus   : XST_SUCCESS if successful else XST_FAILURE.                                     */
/* Note      The Buffer at BufferPtr is moved forward by the number of command bytes used.               */
/*-------------------------------------------------------------------------------------------------------*/
XStatus akaria_SF3_FlashWrite(PmodSF3 *InstancePtr, u32 Addr, u32 ByteCount,
      u8 WriteCmd, u8 **BufferPtr) {
   XStatus Status;

   Status = akaria_SF3_WaitForFlashReady(InstancePtr);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   // Prepare the buffer with write command data
   (*BufferPtr)[0] = WriteCmd;
   (*BufferPtr)[1] = (u8) (Addr >> 24);
   (*BufferPtr)[2] = (u8) (Addr >> 16);
   (*BufferPtr)[3] = (u8) (Addr >> 8);
   (*BufferPtr)[4] = (u8) (Addr);

   Status = XSpi_Transfer(&InstancePtr->SF3Spi, *BufferPtr, NULL,
         (ByteCount + AKARIA_SF3_WRITE_EXTRA_BYTES));

   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   // BufferPtr should point to the beginning of the data written
   *BufferPtr += AKARIA_SF3_WRITE_EXTRA_BYTES;

   return XST_SUCCESS;
}


/*-------------------------------------------------------------------------------------------------------*/
/* Name      akaria_SF3_FlashWrite                                                                       */
/* Function  Reads the data from the Serial Flash Memory.                                                */
/* Arg[1]    PmodSF3 * : A pointer to the instance of the PmodSF3 device.                                */
/* Arg[2]    u32       : The starting address in the Flash Memory from which the data is to be read.     */
/* Arg[3]    u32       : The number of bytes to be read.                                                 */
/* Arg[4]    u8        : Selects which command to use to read with.                                      */
/* Arg[5]    u8 **     : A pointer to the buffer which data is to be read into,                          */
/*                       expected size of ByteCount + DummyBytes(ReadCmd).                               */
/* Return    XStatus   : XST_SUCCESS if successful else XST_FAILURE.                                     */
/*-------------------------------------------------------------------------------------------------------*/
XStatus akaria_SF3_FlashRead(PmodSF3 *InstancePtr, u32 Addr, u32 ByteCount, u8 ReadCmd,
      u8 **BufferPtr) {
   XStatus Status;

   Status = akaria_SF3_WaitForFlashReady(InstancePtr);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   // Prepare the buffer with the command data
   (*BufferPtr)[0] = ReadCmd;
   (*BufferPtr)[1] = (u8) (Addr >> 24);
   (*BufferPtr)[2] = (u8) (Addr >> 16);
   (*BufferPtr)[3] = (u8) (Addr >> 8);
   (*BufferPtr)[4] = (u8) (Addr);

   Status = XSpi_Transfer(&InstancePtr->SF3Spi, *BufferPtr, *BufferPtr,
         (ByteCount + AKARIA_SF3_READ_MIN_EXTRA_BYTES));
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   // Increment the pointer forward to the beginning of data
   *BufferPtr += AKARIA_SF3_READ_MIN_EXTRA_BYTES;

   return XST_SUCCESS;
}


/*-------------------------------------------------------------------------------------------------------*/
/* Name      akaria_SF3_BulkErase                                                                        */
/* Function  Erases the entire contents of the Serial Flash Memory.                                      */
/* Arg[1]    PmodSF3 * : A pointer to the instance of the PmodSF3 device.                                */
/* Return    XStatus   : XST_SUCCESS if successful else XST_FAILURE.                                     */
/* Note      The erased bytes will read as 0xFF.                                                         */
/*-------------------------------------------------------------------------------------------------------*/
XStatus akaria_SF3_BulkErase(PmodSF3 *InstancePtr) {
   XStatus Status;
   u8 Buffer[SF3_BULK_ERASE_BYTES];

   Status = akaria_SF3_WaitForFlashReady(InstancePtr);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   Buffer[0] = SF3_COMMAND_BULK_ERASE;

   Status = XSpi_Transfer(&InstancePtr->SF3Spi, Buffer, NULL,
         SF3_BULK_ERASE_BYTES);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   return XST_SUCCESS;
}

/*-------------------------------------------------------------------------------------------------------*/
/* Name      akaria_SF3_SectorErase                                                                      */
/* Function  Erases the contents of an entire 64KB sector.                                               */
/* Arg[1]    PmodSF3 * : A pointer to the instance of the PmodSF3 device.                                */
/* Arg[2]    u32       : the address of the first byte in the sector to be erased.                       */
/* Return    XStatus   : XST_SUCCESS if successful else XST_FAILURE.                                     */
/* Note      The erased bytes will read as 0xFF.                                                         */
/*-------------------------------------------------------------------------------------------------------*/
XStatus akaria_SF3_SectorErase(PmodSF3 *InstancePtr, u32 Addr) {
   XStatus Status;
   u8 Buffer[SF3_SECTOR_ERASE_BYTES];

   Status = akaria_SF3_WaitForFlashReady(InstancePtr);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   // Prepare the buffer with the command data
   Buffer[0] = SF3_COMMAND_SECTOR_ERASE;
   Buffer[1] = (u8) (Addr >> 24);
   Buffer[2] = (u8) (Addr >> 16);
   Buffer[3] = (u8) (Addr >> 8);
   Buffer[4] = (u8) (Addr);


   Status = XSpi_Transfer(&InstancePtr->SF3Spi, Buffer, NULL,
         AKARIA_SF3_SECTOR_ERASE_BYTES);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   return XST_SUCCESS;
}

/*-------------------------------------------------------------------------------------------------------*/
/* Name      akaria_SF3_GetStatus                                                                        */
/* Function  Captures the status register of the SF3 and stores it in InstancePtr->Status.               */
/* Arg[1]    PmodSF3 * : A pointer to the instance of the PmodSF3 device.                                */
/* Return    XStatus   : XST_SUCCESS if successful else XST_FAILURE.                                     */
/* Note      This function reads the status register of the Buffer and waits until the WIP bit of the    */
/*           status register becomes 0.                                                                  */
/*-------------------------------------------------------------------------------------------------------*/
XStatus akaria_SF3_GetStatus(PmodSF3 *InstancePtr) {
   XStatus Status;

   u8 Buffer[SF3_STATUS_READ_BYTES];

   Buffer[0] = SF3_COMMAND_READ_STATUSREG;

   Status = XSpi_Transfer(&InstancePtr->SF3Spi, Buffer, Buffer,
         SF3_STATUS_READ_BYTES);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   InstancePtr->Status = Buffer[1];

   return XST_SUCCESS;
}

/*-------------------------------------------------------------------------------------------------------*/
/* Name      akaria_SF3_WaitForFlashReady                                                                */
/* Function  Waits until the serial Flash is ready to accept next command.                               */
/* Arg[1]    PmodSF3 * : A pointer to the instance of the PmodSF3 device.                                */
/* Return    XStatus   : XST_SUCCESS if successful else XST_FAILURE.                                     */
/* Note      This function reads the status register of the Buffer and waits until the WIP bit of the    */
/*           status register becomes 0.                                                                  */
/*-------------------------------------------------------------------------------------------------------*/
XStatus akaria_SF3_WaitForFlashReady(PmodSF3 *InstancePtr) {
   XStatus Status;
   u8 StatusReg;

   while (1) {
      /*
       * Get the Status REgister. The status register content is
       * stored at the second byte pointed by the ReadBuffer.
       */
      Status = akaria_SF3_GetStatus(InstancePtr);
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

/*-------------------------------------------------------------------------------------------------------*/
/* Name      akaria_SF3_ReadID                                                                           */
/* Function  Reads the ID register of the serial Flash                                                   */
/* Arg[1]    PmodSF3 * : A pointer to the instance of the PmodSF3 device.                                */
/* Return    XStatus   : XST_SUCCESS if successful else XST_FAILURE.                                     */
/*-------------------------------------------------------------------------------------------------------*/
XStatus akaria_SF3_ReadID(PmodSF3 *InstancePtr) {
   XStatus Status;
   int i;
   u8 Buffer[SF3_ID_READ_BYTES];

   Buffer[0] = SF3_COMMAND_READ_ID;

   Status = XSpi_Transfer(&InstancePtr->SF3Spi, Buffer, Buffer, SF3_ID_READ_BYTES);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   puts("ID =");
   /* Skip extra bytes */
   for (i = 1; i < SF3_ID_READ_BYTES; i++)
      printf(" %x", Buffer[i]);
      puts("\n");
   return XST_SUCCESS;
}
