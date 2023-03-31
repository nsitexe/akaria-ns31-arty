/******************************************************************************/
/*                                                                            */
/* main.c -- PmodSF3 Example Project                                          */
/*                                                                            */
/******************************************************************************/
/* Author: Andrew Holzer                                                      */
/* Copyright 2017, Digilent Inc.                                              */
/******************************************************************************/
/* Module Description:                                                        */
/*                                                                            */
/* This file contains code for running a demonstration of the PmodSF3 IP      */
/* core.                                                                      */
/*                                                                            */
/* This demo initializes the PmodSF3 drivers and then performs several        */
/* self-tests, writing data into two sequential memory pages using different  */
/* write commands and reads that data back out using each of the read         */
/* commands.                                                                  */
/*                                                                            */
/******************************************************************************/
/* Revision History:                                                          */
/*                                                                            */
/*    ??/??/????(AHolzer):  Created                                           */
/*    07/26/2017(ArtVVB):   Refactored and Validated                          */
/*    03/16/2018(atangzwj): Validated for Vivado 2017.4                       */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>
#include "akaria_ns31_entry_kit.h"
#include "akaria_PmodSF3.h"

PmodSF3 akariaSF3;

XStatus DemoInitialize();
void DemoRun();
XStatus SF3_SpiFlashTest(PmodSF3 *InstancePtr);
XStatus SF3_ReadTest(PmodSF3 *InstancePtr);
XStatus SF3_WriteTest(PmodSF3 *InstancePtr);

void akaria_sf3_test() {
   XStatus Status;

   Status = DemoInitialize();
   if (Status != XST_SUCCESS) {
      printf("Error Initializing SF3 Device\n");
      return;
   }
   DemoRun();

   /* Stop SPI Driver */
   XSpi_Stop(&akariaSF3.SF3Spi);

   /* Set SPI0 Selcet to XIP mode */
   akaria_write_reg(SPI0_SEL, SPI0_SEL_XIP);

   return;
}

XStatus DemoInitialize() {
   XStatus Status;

   /* Initialize and Start SPI Driver */
   Status = SF3_begin(&akariaSF3, SPI0_BASE_ADDR);
   if (Status != XST_SUCCESS) {
      printf("Error initializing SF3 device\n");
      return XST_FAILURE;
   }
   return XST_SUCCESS;
}

void DemoRun() {
   XStatus Status;

   printf("Starting Flash Memory Tests\n");

   Status = SF3_SpiFlashTest(&akariaSF3);
   if (Status != XST_SUCCESS) {
      printf("Error Running Flash Memory Tests\n\n");
   } else {
      printf("Flash Memory Tests Successful!\n\n");
   }
}

XStatus SF3_SpiFlashTest(PmodSF3* InstancePtr) {
   XStatus Status;
   u32 Index;
   u32 Address;
   u8 TestByte;

   // Buffers require space to store SPI read/write commands as well as data
   u8 WriteBuffer[SF3_PAGE_SIZE + SF3_WRITE_EXTRA_BYTES];
   u8 ReadBuffer[SF3_PAGE_SIZE + SF3_READ_MAX_EXTRA_BYTES];
   u8 *ReadBufferPtr;
   u8 *WriteBufferPtr;

   Address = SF3_FLASH_TEST_ADDRESS;
   TestByte = 0x20;

   // Fill in the data that is to be written into the Micron Serial Flash
   for (Index = 0; Index < SF3_PAGE_SIZE; Index++) {
      WriteBuffer[Index + SF3_WRITE_EXTRA_BYTES] = (u8) (Index + TestByte);
   }

   Status = SF3_FlashWriteEnable(InstancePtr);
   if (Status != XST_SUCCESS) {
      printf("SPI Flash wait ready error\n");
      return XST_FAILURE;
   }

   // Perform the sector erase operation.
   Status = SF3_SectorErase(InstancePtr, Address);
   if (Status != XST_SUCCESS) {
      printf("SPI Flash sector erase error\n");
      return XST_FAILURE;
   }

   // Perform write enable operation
   Status = SF3_FlashWriteEnable(InstancePtr);
   if (Status != XST_SUCCESS) {
      printf("SPI Flash write enable error\n");
      return XST_FAILURE;
   }

   // Write the data to the page using page program command.
   WriteBufferPtr = WriteBuffer;
   Status = SF3_FlashWrite(InstancePtr, Address, SF3_PAGE_SIZE,
         SF3_COMMAND_PAGE_PROGRAM, &WriteBufferPtr);
   if (Status != XST_SUCCESS) {
      printf("SPI Flash page write error\n");
      return XST_FAILURE;
   }

   printf("SPI Write Successful\n");

   // Clear the read buffer
   for (Index = 0; Index < SF3_PAGE_SIZE + SF3_READ_MAX_EXTRA_BYTES; Index++) {
      ReadBuffer[Index] = 0x0;
   }

   // Read the data from the page using read command
   ReadBufferPtr = ReadBuffer;
   Status = SF3_FlashRead(InstancePtr, Address, SF3_PAGE_SIZE,
         SF3_COMMAND_READ, &ReadBufferPtr);
   if (Status != XST_SUCCESS) {
      printf("SPI Flash read error\n");
      return XST_FAILURE;
   }

   printf("SPI Flash READ Command Successful\n");

   // Compare the data read against data written
   for (Index = 0; Index < SF3_PAGE_SIZE; Index++) {
      if (ReadBufferPtr[Index] != WriteBufferPtr[Index]) {
         printf("SPI Flash data read/write comparison error\n");
         printf("Address = 0x%x, ReadBuffer = 0x%x, WriteBuffer = 0x%x\n", (Address + Index), ReadBufferPtr[Index], WriteBufferPtr[Index]);

         return XST_FAILURE;
      }
   }

   printf("SPI Flash Read Test Passed\n");

   // Clear the read buffer
   for (Index = 0; Index < SF3_PAGE_SIZE + SF3_READ_MAX_EXTRA_BYTES; Index++) {
      ReadBuffer[Index] = 0x0;
   }

   // Read the data from the page using dual output fast read command.
   ReadBufferPtr = ReadBuffer;
   Status = SF3_FlashRead(InstancePtr, Address, SF3_PAGE_SIZE,
         SF3_COMMAND_FAST_READ, &ReadBufferPtr);
   if (Status != XST_SUCCESS) {
      printf("SPI Flash FAST READ Command error\n");
      return XST_FAILURE;
   }

   printf("SPI Flash FAST READ Command Successful\n");

   // Compare data read against the data written
   for (Index = 0; Index < SF3_PAGE_SIZE; Index++) {
      if (ReadBufferPtr[Index] != WriteBufferPtr[Index]) {
         printf("SPI Flash data read/write comparison error\n");
         printf("Address = 0x%x, ReadBuffer = 0x%x, WriteBuffer = 0x%x\n", (Address + Index), ReadBufferPtr[Index], WriteBufferPtr[Index]);
         return XST_FAILURE;
      }
   }

   printf("SPI Flash Fast Read test passed\n");

   return XST_SUCCESS;
}
