#include <stdio.h>
#include "akaria_ns31_arty.h"
#include "akaria_PmodSF3.h"

#include "akaria_ns_bsp_bin.h" /* Sample Code binary */

PmodSF3 akariaSF3;

XStatus FlashWrite(PmodSF3 *InstancePtr);
XStatus Verify(PmodSF3 *InstancePtr);

void akaria_flash_write() {
   XStatus Status;

   /* Initialize and Start SPI Driver */
   Status = akaria_SF3_Init(&akariaSF3, SPI0_BASE_ADDR);
   if (Status != XST_SUCCESS) {
      printf("Error initializing SF3 device\n");
      return;
   }
   printf("SPI Initialize success.\n");

   Status = FlashWrite(&akariaSF3);
   Status = Verify(&akariaSF3);

   if (Status != XST_SUCCESS) {
      printf("Flash Memory Write Error!\n\n");
   } else {
      printf("Writing Flash Memory Successful!\n\n");
   }

   /* Stop SPI Driver */
   XSpi_Stop(&akariaSF3.SF3Spi);

   /* Set SPI0 Selcet to XIP mode */
    akaria_write_reg(SPI0_SEL, SPI0_SEL_XIP);


   return;
}

XStatus FlashWrite(PmodSF3* InstancePtr) {
   XStatus Status;
   u32 Index;
   u32 Address;
   u32 RemainBytes;
   u32 WriteSize;
   u32 reg;

   // Buffers require space to store SPI read/write commands as well as data
   u8 WriteBuffer[SF3_PAGE_SIZE + AKARIA_SF3_WRITE_EXTRA_BYTES];
   u8 *WriteBufferPtr;

   Address = 0x00000000; /* Flash address */

   RemainBytes = sizeof(akaria_ns_bsp_bin);

   while(RemainBytes > 0){
      if(RemainBytes > SF3_PAGE_SIZE) WriteSize = SF3_PAGE_SIZE;
      else WriteSize = RemainBytes;

      // Fill in the data that is to be written into the Micron Serial Flash
      for (Index = 0; Index < SF3_PAGE_SIZE; Index++) {
         if(Index < WriteSize){
            WriteBuffer[Index + AKARIA_SF3_WRITE_EXTRA_BYTES] = akaria_ns_bsp_bin[Address + Index];
         } 
         else  WriteBuffer[Index + AKARIA_SF3_WRITE_EXTRA_BYTES] = 0xff;
      }

      /* Sector Erase */
      if((Address % 0x10000) == 0){
         Status = akaria_SF3_FlashWriteEnable(InstancePtr);
         if (Status != XST_SUCCESS) {
            printf("SPI Flash wait ready error\n");
            return XST_FAILURE;
         }

         // Perform the sector erase operation.
         Status = akaria_SF3_SectorErase(InstancePtr, Address);
         if (Status != XST_SUCCESS) {
            printf("SPI Flash sector erase error\n");
            return XST_FAILURE;
         }
      }

      /* Page Program */
      printf("Flash writing... (0x%x)\n", Address);
      Status = akaria_SF3_FlashWriteEnable(InstancePtr);
      if (Status != XST_SUCCESS) {
         printf("SPI Flash wait ready error\n");
         return XST_FAILURE;
      }

      // Write the data to the page using page program command.
      WriteBufferPtr = WriteBuffer;
      Status = akaria_SF3_FlashWrite(InstancePtr, Address, SF3_PAGE_SIZE,
            SF3_COMMAND_PAGE_PROGRAM, &WriteBufferPtr);
      if (Status != XST_SUCCESS) {
         printf("SPI Flash page write error\n");
         return XST_FAILURE;
      }
      Address += SF3_PAGE_SIZE;
      if(RemainBytes > SF3_PAGE_SIZE) RemainBytes -= SF3_PAGE_SIZE;
      else RemainBytes = 0;
   }

   return XST_SUCCESS;
}

XStatus Verify(PmodSF3* InstancePtr) {
   XStatus Status;
   u32 Index;
   u32 Address;
   u8 ReadCmd;
   u32 RemainBytes;
   u32 ReadSize;
   u32 reg;

   // Set Read Command
   ReadCmd = SF3_COMMAND_RANDOM_READ;

   // Buffers require space to store SPI read/write commands as well as data
   u8 ReadBuffer[SF3_PAGE_SIZE + AKARIA_SF3_READ_MAX_EXTRA_BYTES];
   u8 *ReadBufferPtr;
   printf("Verifying...\n");

   Address = 0x00000000; /* Flash address */
 
   RemainBytes = sizeof(akaria_ns_bsp_bin);

   while(RemainBytes > 0){
      // Clear the read buffer
      for (Index = 0; Index < SF3_PAGE_SIZE + AKARIA_SF3_READ_MAX_EXTRA_BYTES; Index++) {
         ReadBuffer[Index] = 0x0;
      }

      // Read the data from the page using random read command
      ReadBufferPtr = ReadBuffer;
      Status = akaria_SF3_FlashRead(InstancePtr, Address, SF3_PAGE_SIZE,
            ReadCmd, &ReadBufferPtr);
      if (Status != XST_SUCCESS) {
         printf("SPI Flash read error\n");
         return XST_FAILURE;
      }

      if(RemainBytes > SF3_PAGE_SIZE) ReadSize = SF3_PAGE_SIZE;
      else ReadSize = RemainBytes;

      // Compare the data read against data written
      for (Index = 0; Index < ReadSize; Index++) {
         if (ReadBufferPtr[Index] != akaria_ns_bsp_bin[Address + Index]) {
            printf("Verify error!\n");
            printf("Address = 0x%08x, ", Address + Index);
            printf("ReadBuffer = 0x%x, ", *(ReadBufferPtr+Index));
            printf("Expected value = 0x%x\n", akaria_ns_bsp_bin[Address + Index]);
            return XST_FAILURE;
         }
      }

      Address += SF3_PAGE_SIZE;
      if(RemainBytes > SF3_PAGE_SIZE) RemainBytes -= SF3_PAGE_SIZE;
      else RemainBytes = 0;
   }

   return XST_SUCCESS;
}
