/** @file
  This driver is used for Opal Password Feature support at AHCI mode.

Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include "OpalPasswordSmm.h"

/**
  Start command for give slot on specific port.

  @param  Port               The number of port.
  @param  CommandSlot        The number of CommandSlot.
  @param  Timeout            The timeout Value of start.

  @retval EFI_DEVICE_ERROR   The command start unsuccessfully.
  @retval EFI_TIMEOUT        The operation is time out.
  @retval EFI_SUCCESS        The command start successfully.

**/
EFI_STATUS
EFIAPI
AhciStartCommand (
  IN  UINT8                     Port,
  IN  UINT8                     CommandSlot,
  IN  UINT64                    Timeout
  );

/**
  Stop command running for giving port

  @param  Port               The number of port.
  @param  Timeout            The timeout Value of stop.

  @retval EFI_DEVICE_ERROR   The command stop unsuccessfully.
  @retval EFI_TIMEOUT        The operation is time out.
  @retval EFI_SUCCESS        The command stop successfully.

**/
EFI_STATUS
EFIAPI
AhciStopCommand (
  IN  UINT8                     Port,
  IN  UINT64                    Timeout
  );

/**
  Read AHCI Operation register.

  @param  Offset       The operation register offset.

  @return The register content read.

**/
UINT32
EFIAPI
AhciReadReg (
  IN  UINT32              Offset
  )
{
  UINT32   Data;

  Data = 0;

  Data = MmioRead32 (mAhciBar + Offset);

  return Data;
}

/**
  Write AHCI Operation register.

  @param  Offset       The operation register offset.
  @param  Data         The Data used to write down.

**/
VOID
EFIAPI
AhciWriteReg (
  IN UINT32               Offset,
  IN UINT32               Data
  )
{
  MmioWrite32 (mAhciBar + Offset, Data);

  return ;
}

/**
  Do AND operation with the Value of AHCI Operation register.

  @param  Offset       The operation register offset.
  @param  AndData      The Data used to do AND operation.

**/
VOID
EFIAPI
AhciAndReg (
  IN UINT32               Offset,
  IN UINT32               AndData
  )
{
  UINT32 Data;

  Data  = AhciReadReg (Offset);

  Data &= AndData;

  AhciWriteReg (Offset, Data);
}

/**
  Do OR operation with the Value of AHCI Operation register.

  @param  Offset       The operation register offset.
  @param  OrData       The Data used to do OR operation.

**/
VOID
EFIAPI
AhciOrReg (
  IN UINT32               Offset,
  IN UINT32               OrData
  )
{
  UINT32 Data;

  Data  = AhciReadReg (Offset);

  Data |= OrData;

  AhciWriteReg (Offset, Data);
}

/**
  Wait for memory set to the test Value.

  @param  Offset            The memory address to test.
  @param  MaskValue         The mask Value of memory.
  @param  TestValue         The test Value of memory.
  @param  Timeout           The time out Value for wait memory set.

  @retval EFI_DEVICE_ERROR  The memory is not set.
  @retval EFI_TIMEOUT       The memory setting is time out.
  @retval EFI_SUCCESS       The memory is correct set.

**/
EFI_STATUS
EFIAPI
AhciWaitMmioSet (
  IN  UINT32                    Offset,
  IN  UINT32                    MaskValue,
  IN  UINT32                    TestValue,
  IN  UINT64                    Timeout
  )
{
  UINT32     Value;
  UINT32     Delay;

  Delay = (UINT32) (DivU64x32(Timeout, 1000) + 1);

  do {
    Value = AhciReadReg (Offset) & MaskValue;

    if (Value == TestValue) {
      return EFI_SUCCESS;
    }

    //
    // Stall for 100 microseconds.
    //
    MicroSecondDelay (100);

    Delay--;

  } while (Delay > 0);

  return EFI_TIMEOUT;
}
/**
  Wait for the Value of the specified system memory set to the test Value.

  @param  Address           The system memory address to test.
  @param  MaskValue         The mask Value of memory.
  @param  TestValue         The test Value of memory.
  @param  Timeout           The time out Value for wait memory set, uses 100ns as a unit.

  @retval EFI_TIMEOUT       The system memory setting is time out.
  @retval EFI_SUCCESS       The system memory is correct set.

**/
EFI_STATUS
EFIAPI
AhciWaitMemSet (
  IN  EFI_PHYSICAL_ADDRESS      Address,
  IN  UINT32                    MaskValue,
  IN  UINT32                    TestValue,
  IN  UINT64                    Timeout
  )
{
  UINT32     Value;
  UINT32     Delay;

  Delay = (UINT32) (DivU64x32 (Timeout, 1000) + 1);

  do {
    //
    // Access sytem memory to see if the Value is the tested one.
    //
    // The system memory pointed by Address will be updated by the
    // SATA Host Controller, "volatile" is introduced to prevent
    // compiler from optimizing the access to the memory address
    // to only read once.
    //
    Value  = *(volatile UINT32 *) (UINTN) Address;
    Value &= MaskValue;

    if (Value == TestValue) {
      return EFI_SUCCESS;
    }

    //
    // Stall for 100 microseconds.
    //
    MicroSecondDelay (100);

    Delay--;

  } while (Delay > 0);

  return EFI_TIMEOUT;
}

/**
  Check the memory status to the test Value.

  @param[in]       Address           The memory address to test.
  @param[in]       MaskValue         The mask Value of memory.
  @param[in]       TestValue         The test Value of memory.
  @param[in, out]  RetryTimes        The retry times Value for waitting memory set. If 0, then just try once.

  @retval EFI_NOTREADY      The memory is not set.
  @retval EFI_TIMEOUT       The memory setting retry times out.
  @retval EFI_SUCCESS       The memory is correct set.

**/
EFI_STATUS
EFIAPI
AhciCheckMemSet (
  IN     UINTN                     Address,
  IN     UINT32                    MaskValue,
  IN     UINT32                    TestValue,
  IN OUT UINTN                     *RetryTimes OPTIONAL
  )
{
  UINT32     Value;

  if (RetryTimes != NULL) {
    (*RetryTimes)--;
  }

  Value  = *(volatile UINT32 *) Address;
  Value &= MaskValue;

  if (Value == TestValue) {
    return EFI_SUCCESS;
  }

  if ((RetryTimes != NULL) && (*RetryTimes == 0)) {
    return EFI_TIMEOUT;
  } else {
    return EFI_NOT_READY;
  }
}

/**
  Clear the port interrupt and error status. It will also clear
  HBA interrupt status.

  @param      Port           The number of port.

**/
VOID
EFIAPI
AhciClearPortStatus (
  IN  UINT8                  Port
  )
{
  UINT32 Offset;

  //
  // Clear any error status
  //
  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_SERR;
  AhciWriteReg (Offset, AhciReadReg (Offset));

  //
  // Clear any port interrupt status
  //
  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_IS;
  AhciWriteReg (Offset, AhciReadReg (Offset));

  //
  // Clear any HBA interrupt status
  //
  AhciWriteReg (EFI_AHCI_IS_OFFSET, AhciReadReg (EFI_AHCI_IS_OFFSET));
}

/**
  Enable the FIS running for giving port.

  @param      Port           The number of port.
  @param      Timeout        The timeout Value of enabling FIS.

  @retval EFI_DEVICE_ERROR   The FIS enable setting fails.
  @retval EFI_TIMEOUT        The FIS enable setting is time out.
  @retval EFI_SUCCESS        The FIS enable successfully.

**/
EFI_STATUS
EFIAPI
AhciEnableFisReceive (
  IN  UINT8                     Port,
  IN  UINT64                    Timeout
  )
{
  UINT32 Offset;

  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CMD;
  AhciOrReg (Offset, EFI_AHCI_PORT_CMD_FRE);

  return AhciWaitMmioSet (
           Offset,
           EFI_AHCI_PORT_CMD_FR,
           EFI_AHCI_PORT_CMD_FR,
           Timeout
           );
}

/**
  Disable the FIS running for giving port.

  @param      Port           The number of port.
  @param      Timeout        The timeout Value of disabling FIS.

  @retval EFI_DEVICE_ERROR   The FIS disable setting fails.
  @retval EFI_TIMEOUT        The FIS disable setting is time out.
  @retval EFI_UNSUPPORTED    The port is in running state.
  @retval EFI_SUCCESS        The FIS disable successfully.

**/
EFI_STATUS
EFIAPI
AhciDisableFisReceive (
  IN  UINT8                     Port,
  IN  UINT64                    Timeout
  )
{
  UINT32 Offset;
  UINT32 Data;

  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CMD;
  Data   = AhciReadReg (Offset);

  //
  // Before disabling Fis receive, the DMA engine of the port should NOT be in running status.
  //
  if ((Data & (EFI_AHCI_PORT_CMD_ST | EFI_AHCI_PORT_CMD_CR)) != 0) {
    return EFI_UNSUPPORTED;
  }

  //
  // Check if the Fis receive DMA engine for the port is running.
  //
  if ((Data & EFI_AHCI_PORT_CMD_FR) != EFI_AHCI_PORT_CMD_FR) {
    return EFI_SUCCESS;
  }

  AhciAndReg (Offset, (UINT32)~(EFI_AHCI_PORT_CMD_FRE));

  return AhciWaitMmioSet (
           Offset,
           EFI_AHCI_PORT_CMD_FR,
           0,
           Timeout
           );
}

/**
  Build the command list, command table and prepare the fis receiver.

  @param    AhciRegisters         The pointer to the EFI_AHCI_REGISTERS.
  @param    Port                  The number of port.
  @param    PortMultiplier        The timeout Value of stop.
  @param    CommandFis            The control fis will be used for the transfer.
  @param    CommandList           The command list will be used for the transfer.
  @param    AtapiCommand          The atapi command will be used for the transfer.
  @param    AtapiCommandLength    The Length of the atapi command.
  @param    CommandSlotNumber     The command slot will be used for the transfer.
  @param    DataPhysicalAddr      The pointer to the Data Buffer pci bus master address.
  @param    DataLength            The Data count to be transferred.

**/
VOID
EFIAPI
AhciBuildCommand (
  IN     EFI_AHCI_REGISTERS         *AhciRegisters,
  IN     UINT8                      Port,
  IN     UINT8                      PortMultiplier,
  IN     EFI_AHCI_COMMAND_FIS       *CommandFis,
  IN     EFI_AHCI_COMMAND_LIST      *CommandList,
  IN     EFI_AHCI_ATAPI_COMMAND     *AtapiCommand OPTIONAL,
  IN     UINT8                      AtapiCommandLength,
  IN     UINT8                      CommandSlotNumber,
  IN OUT VOID                       *DataPhysicalAddr,
  IN     UINT64                     DataLength
  )
{
  UINT64     BaseAddr;
  UINT64     PrdtNumber;
  UINTN      RemainedData;
  UINTN      MemAddr;
  DATA_64    Data64;
  UINT32     Offset;

  //
  // Filling the PRDT
  //
  PrdtNumber = DivU64x32 (DataLength + EFI_AHCI_MAX_DATA_PER_PRDT - 1, EFI_AHCI_MAX_DATA_PER_PRDT);

  //
  // According to AHCI 1.3 spec, a PRDT entry can point to a maximum 4MB Data block.
  // It also limits that the maximum amount of the PRDT entry in the command table
  // is 65535.
  //
  ASSERT (PrdtNumber <= 1);

  Data64.Uint64 = (UINTN) (AhciRegisters->AhciRFis);

  BaseAddr = Data64.Uint64;

  ZeroMem ((VOID *)((UINTN) BaseAddr), sizeof (EFI_AHCI_RECEIVED_FIS));

  ZeroMem (AhciRegisters->AhciCommandTable, sizeof (EFI_AHCI_COMMAND_TABLE));

  CommandFis->AhciCFisPmNum = PortMultiplier;

  CopyMem (&AhciRegisters->AhciCommandTable->CommandFis, CommandFis, sizeof (EFI_AHCI_COMMAND_FIS));

  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CMD;
  if (AtapiCommand != NULL) {
    CopyMem (
      &AhciRegisters->AhciCommandTable->AtapiCmd,
      AtapiCommand,
      AtapiCommandLength
      );

    CommandList->AhciCmdA = 1;
    CommandList->AhciCmdP = 1;

    AhciOrReg (Offset, (EFI_AHCI_PORT_CMD_DLAE | EFI_AHCI_PORT_CMD_ATAPI));
  } else {
    AhciAndReg (Offset, (UINT32)~(EFI_AHCI_PORT_CMD_DLAE | EFI_AHCI_PORT_CMD_ATAPI));
  }

  RemainedData = (UINTN) DataLength;
  MemAddr      = (UINTN) DataPhysicalAddr;
  CommandList->AhciCmdPrdtl = (UINT32)PrdtNumber;

  AhciRegisters->AhciCommandTable->PrdtTable.AhciPrdtDbc = (UINT32)RemainedData - 1;

  Data64.Uint64 = (UINT64)MemAddr;
  AhciRegisters->AhciCommandTable->PrdtTable.AhciPrdtDba  = Data64.Uint32.Lower32;
  AhciRegisters->AhciCommandTable->PrdtTable.AhciPrdtDbau = Data64.Uint32.Upper32;

  //
  // Set the last PRDT to Interrupt On Complete
  //
  AhciRegisters->AhciCommandTable->PrdtTable.AhciPrdtIoc = 1;

  CopyMem (
    (VOID *) ((UINTN) AhciRegisters->AhciCmdList + (UINTN) CommandSlotNumber * sizeof (EFI_AHCI_COMMAND_LIST)),
    CommandList,
    sizeof (EFI_AHCI_COMMAND_LIST)
    );

  Data64.Uint64 = (UINT64)(UINTN) AhciRegisters->AhciCommandTable;
  AhciRegisters->AhciCmdList[CommandSlotNumber].AhciCmdCtba  = Data64.Uint32.Lower32;
  AhciRegisters->AhciCmdList[CommandSlotNumber].AhciCmdCtbau = Data64.Uint32.Upper32;
  AhciRegisters->AhciCmdList[CommandSlotNumber].AhciCmdPmp   = PortMultiplier;

}

/**
  Buid a command FIS.

  @param  CmdFis            A pointer to the EFI_AHCI_COMMAND_FIS Data structure.
  @param  AtaCommandBlock   A pointer to the AhciBuildCommandFis Data structure.

**/
VOID
EFIAPI
AhciBuildCommandFis (
  IN OUT EFI_AHCI_COMMAND_FIS    *CmdFis,
  IN     EFI_ATA_COMMAND_BLOCK   *AtaCommandBlock
  )
{
  ZeroMem (CmdFis, sizeof (EFI_AHCI_COMMAND_FIS));

  CmdFis->AhciCFisType = EFI_AHCI_FIS_REGISTER_H2D;
  //
  // Indicator it's a command
  //
  CmdFis->AhciCFisCmdInd      = 0x1;
  CmdFis->AhciCFisCmd         = AtaCommandBlock->AtaCommand;

  CmdFis->AhciCFisFeature     = AtaCommandBlock->AtaFeatures;
  CmdFis->AhciCFisFeatureExp  = AtaCommandBlock->AtaFeaturesExp;

  CmdFis->AhciCFisSecNum      = AtaCommandBlock->AtaSectorNumber;
  CmdFis->AhciCFisSecNumExp   = AtaCommandBlock->AtaSectorNumberExp;

  CmdFis->AhciCFisClyLow      = AtaCommandBlock->AtaCylinderLow;
  CmdFis->AhciCFisClyLowExp   = AtaCommandBlock->AtaCylinderLowExp;

  CmdFis->AhciCFisClyHigh     = AtaCommandBlock->AtaCylinderHigh;
  CmdFis->AhciCFisClyHighExp  = AtaCommandBlock->AtaCylinderHighExp;

  CmdFis->AhciCFisSecCount    = AtaCommandBlock->AtaSectorCount;
  CmdFis->AhciCFisSecCountExp = AtaCommandBlock->AtaSectorCountExp;

  CmdFis->AhciCFisDevHead     = (UINT8) (AtaCommandBlock->AtaDeviceHead | 0xE0);
}

/**
  Start a PIO Data transfer on specific port.

  @param  AhciRegisters       The pointer to the EFI_AHCI_REGISTERS.
  @param  Port                The number of port.
  @param  PortMultiplier      The timeout Value of stop.
  @param  AtapiCommand        The atapi command will be used for the transfer.
  @param  AtapiCommandLength  The Length of the atapi command.
  @param  Read                The transfer direction.
  @param  AtaCommandBlock     The EFI_ATA_COMMAND_BLOCK Data.
  @param  AtaStatusBlock      The EFI_ATA_STATUS_BLOCK Data.
  @param  MemoryAddr          The pointer to the Data Buffer.
  @param  DataCount           The Data count to be transferred.
  @param  Timeout             The timeout Value of non Data transfer.

  @retval EFI_DEVICE_ERROR    The PIO Data transfer abort with error occurs.
  @retval EFI_TIMEOUT         The operation is time out.
  @retval EFI_UNSUPPORTED     The device is not ready for transfer.
  @retval EFI_SUCCESS         The PIO Data transfer executes successfully.

**/
EFI_STATUS
EFIAPI
AhciPioTransfer (
  IN     EFI_AHCI_REGISTERS         *AhciRegisters,
  IN     UINT8                      Port,
  IN     UINT8                      PortMultiplier,
  IN     EFI_AHCI_ATAPI_COMMAND     *AtapiCommand OPTIONAL,
  IN     UINT8                      AtapiCommandLength,
  IN     BOOLEAN                    Read,
  IN     EFI_ATA_COMMAND_BLOCK      *AtaCommandBlock,
  IN OUT EFI_ATA_STATUS_BLOCK       *AtaStatusBlock,
  IN OUT VOID                       *MemoryAddr,
  IN     UINT32                     DataCount,
  IN     UINT64                     Timeout
  )
{
  EFI_STATUS                    Status;
  UINT32                        FisBaseAddr;
  UINT32                        Offset;
  UINT32                        Delay;
  EFI_AHCI_COMMAND_FIS          CFis;
  EFI_AHCI_COMMAND_LIST         CmdList;
  UINT32                        PortTfd;
  UINT32                        PrdCount;
  UINT32                        OldRfisLo;
  UINT32                        OldRfisHi;
  UINT32                        OldCmdListLo;
  UINT32                        OldCmdListHi;

  Offset    = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_FB;
  OldRfisLo = AhciReadReg (Offset);
  Offset    = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_FBU;
  OldRfisHi = AhciReadReg (Offset);
  Offset    = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_FB;
  AhciWriteReg (Offset, (UINT32)(UINTN)AhciRegisters->AhciRFis);
  Offset    = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_FBU;
  AhciWriteReg (Offset, 0);

  //
  // Single task envrionment, we only use one command table for all port
  //
  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CLB;
  OldCmdListLo = AhciReadReg (Offset);
  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CLBU;
  OldCmdListHi = AhciReadReg (Offset);
  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CLB;
  AhciWriteReg (Offset, (UINT32)(UINTN)AhciRegisters->AhciCmdList);
  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CLBU;
  AhciWriteReg (Offset, 0);

  //
  // Package read needed
  //
  AhciBuildCommandFis (&CFis, AtaCommandBlock);

  ZeroMem (&CmdList, sizeof (EFI_AHCI_COMMAND_LIST));

  CmdList.AhciCmdCfl = EFI_AHCI_FIS_REGISTER_H2D_LENGTH / 4;
  CmdList.AhciCmdW   = Read ? 0 : 1;

  AhciBuildCommand (
    AhciRegisters,
    Port,
    PortMultiplier,
    &CFis,
    &CmdList,
    AtapiCommand,
    AtapiCommandLength,
    0,
    (VOID *)(UINTN)MemoryAddr,
    DataCount
    );

  Status = AhciStartCommand (
             Port,
             0,
             Timeout
             );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  //
  // Checking the status and wait the driver sending Data
  //
  FisBaseAddr = (UINT32)(UINTN)AhciRegisters->AhciRFis;
  if (Read && (AtapiCommand == 0)) {
    //
    // Wait device sends the PIO setup fis before Data transfer
    //
    Status = EFI_TIMEOUT;
    Delay  = (UINT32) (DivU64x32 (Timeout, 1000) + 1);
    do {
      Offset = FisBaseAddr + EFI_AHCI_PIO_FIS_OFFSET;

      Status = AhciCheckMemSet (Offset, EFI_AHCI_FIS_TYPE_MASK, EFI_AHCI_FIS_PIO_SETUP, 0);
      if (!EFI_ERROR (Status)) {
        Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_TFD;
        PortTfd = AhciReadReg ((UINT32) Offset);
        //
        // PxTFD will be updated if there is a D2H or SetupFIS received.
        // For PIO IN transfer, D2H means a device error. Therefore we only need to check the TFD after receiving a SetupFIS.
        //
        if ((PortTfd & EFI_AHCI_PORT_TFD_ERR) != 0) {
          Status = EFI_DEVICE_ERROR;
          break;
        }

        PrdCount = *(volatile UINT32 *) (&(AhciRegisters->AhciCmdList[0].AhciCmdPrdbc));
        if (PrdCount == DataCount) {
          break;
        }
      }

      Offset = FisBaseAddr + EFI_AHCI_D2H_FIS_OFFSET;
      Status = AhciCheckMemSet (Offset, EFI_AHCI_FIS_TYPE_MASK, EFI_AHCI_FIS_REGISTER_D2H, 0);
      if (!EFI_ERROR (Status)) {
        Status = EFI_DEVICE_ERROR;
        break;
      }

      //
      // Stall for 100 microseconds.
      //
      MicroSecondDelay(100);

      Delay--;
    } while (Delay > 0);
  } else {
    //
    // Wait for D2H Fis is received
    //
    Offset = FisBaseAddr + EFI_AHCI_D2H_FIS_OFFSET;
    Status = AhciWaitMemSet (
               Offset,
               EFI_AHCI_FIS_TYPE_MASK,
               EFI_AHCI_FIS_REGISTER_D2H,
               Timeout
               );

    if (EFI_ERROR (Status)) {
      goto Exit;
    }

    Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_TFD;
    PortTfd = AhciReadReg ((UINT32) Offset);
    if ((PortTfd & EFI_AHCI_PORT_TFD_ERR) != 0) {
      Status = EFI_DEVICE_ERROR;
    }
  }

Exit:
  AhciStopCommand (
    Port,
    Timeout
    );

  AhciDisableFisReceive (
    Port,
    Timeout
    );

  Offset    = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_FB;
  AhciWriteReg (Offset, OldRfisLo);
  Offset    = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_FBU;
  AhciWriteReg (Offset, OldRfisHi);

  Offset    = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CLB;
  AhciWriteReg (Offset, OldCmdListLo);
  Offset    = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CLBU;
  AhciWriteReg (Offset, OldCmdListHi);

  return Status;
}

/**
  Stop command running for giving port

  @param  Port               The number of port.
  @param  Timeout            The timeout Value of stop.

  @retval EFI_DEVICE_ERROR   The command stop unsuccessfully.
  @retval EFI_TIMEOUT        The operation is time out.
  @retval EFI_SUCCESS        The command stop successfully.

**/
EFI_STATUS
EFIAPI
AhciStopCommand (
  IN  UINT8                     Port,
  IN  UINT64                    Timeout
  )
{
  UINT32 Offset;
  UINT32 Data;

  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CMD;
  Data   = AhciReadReg (Offset);

  if ((Data & (EFI_AHCI_PORT_CMD_ST |  EFI_AHCI_PORT_CMD_CR)) == 0) {
    return EFI_SUCCESS;
  }

  if ((Data & EFI_AHCI_PORT_CMD_ST) != 0) {
    AhciAndReg (Offset, (UINT32)~(EFI_AHCI_PORT_CMD_ST));
  }

  return AhciWaitMmioSet (
           Offset,
           EFI_AHCI_PORT_CMD_CR,
           0,
           Timeout
           );
}

/**
  Start command for give slot on specific port.

  @param  Port               The number of port.
  @param  CommandSlot        The number of CommandSlot.
  @param  Timeout            The timeout Value of start.

  @retval EFI_DEVICE_ERROR   The command start unsuccessfully.
  @retval EFI_TIMEOUT        The operation is time out.
  @retval EFI_SUCCESS        The command start successfully.

**/
EFI_STATUS
EFIAPI
AhciStartCommand (
  IN  UINT8                     Port,
  IN  UINT8                     CommandSlot,
  IN  UINT64                    Timeout
  )
{
  UINT32                        CmdSlotBit;
  EFI_STATUS                    Status;
  UINT32                        PortStatus;
  UINT32                        StartCmd;
  UINT32                        PortTfd;
  UINT32                        Offset;
  UINT32                        Capability;

  //
  // Collect AHCI controller information
  //
  Capability = AhciReadReg(EFI_AHCI_CAPABILITY_OFFSET);

  CmdSlotBit = (UINT32) (1 << CommandSlot);

  AhciClearPortStatus (
    Port
    );

  Status = AhciEnableFisReceive (
             Port,
             Timeout
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CMD;
  PortStatus = AhciReadReg (Offset);

  StartCmd = 0;
  if ((PortStatus & EFI_AHCI_PORT_CMD_ALPE) != 0) {
    StartCmd = AhciReadReg (Offset);
    StartCmd &= ~EFI_AHCI_PORT_CMD_ICC_MASK;
    StartCmd |= EFI_AHCI_PORT_CMD_ACTIVE;
  }

  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_TFD;
  PortTfd = AhciReadReg (Offset);

  if ((PortTfd & (EFI_AHCI_PORT_TFD_BSY | EFI_AHCI_PORT_TFD_DRQ)) != 0) {
    if ((Capability & BIT24) != 0) {
      Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CMD;
      AhciOrReg (Offset, EFI_AHCI_PORT_CMD_COL);

      AhciWaitMmioSet (
        Offset,
        EFI_AHCI_PORT_CMD_COL,
        0,
        Timeout
        );
    }
  }

  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CMD;
  AhciOrReg (Offset, EFI_AHCI_PORT_CMD_ST | StartCmd);

  //
  // Setting the command
  //
  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_SACT;
  AhciAndReg (Offset, 0);
  AhciOrReg (Offset, CmdSlotBit);

  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CI;
  AhciAndReg (Offset, 0);
  AhciOrReg (Offset, CmdSlotBit);
  return EFI_SUCCESS;
}


/**
  Do AHCI HBA reset.

  @param[in]  Timeout        The timeout Value of reset.

  @retval EFI_DEVICE_ERROR   AHCI controller is failed to complete hardware reset.
  @retval EFI_TIMEOUT        The reset operation is time out.
  @retval EFI_SUCCESS        AHCI controller is reset successfully.

**/
EFI_STATUS
EFIAPI
AhciReset (
  IN  UINT64                    Timeout
  )
{
  UINT32                 Delay;
  UINT32                 Value;
  UINT32                 Capability;

  //
  // Collect AHCI controller information
  //
  Capability = AhciReadReg (EFI_AHCI_CAPABILITY_OFFSET);

  //
  // Enable AE before accessing any AHCI registers if Supports AHCI Mode Only is not set
  //
  if ((Capability & EFI_AHCI_CAP_SAM) == 0) {
    AhciOrReg (EFI_AHCI_GHC_OFFSET, EFI_AHCI_GHC_ENABLE);
  }

  AhciOrReg (EFI_AHCI_GHC_OFFSET, EFI_AHCI_GHC_RESET);

  Delay = (UINT32) (DivU64x32(Timeout, 1000) + 1);

  do {
    Value = AhciReadReg(EFI_AHCI_GHC_OFFSET);
    if ((Value & EFI_AHCI_GHC_RESET) == 0) {
      return EFI_SUCCESS;
    }

    //
    // Stall for 100 microseconds.
    //
    MicroSecondDelay(100);

    Delay--;
  } while (Delay > 0);

  return EFI_TIMEOUT;


}

/**
  Send Buffer cmd to specific device.

  @param[in]  AhciRegisters       The pointer to the EFI_AHCI_REGISTERS.
  @param[in]  Port                The port number of attached ATA device.
  @param[in]  PortMultiplier      The port number of port multiplier of attached ATA device.
  @param[in, out]  Buffer         The Data Buffer to store IDENTIFY PACKET Data.

  @retval EFI_DEVICE_ERROR    The cmd abort with error occurs.
  @retval EFI_TIMEOUT         The operation is time out.
  @retval EFI_UNSUPPORTED     The device is not ready for executing.
  @retval EFI_SUCCESS         The cmd executes successfully.

**/
EFI_STATUS
EFIAPI
AhciIdentify (
  IN EFI_AHCI_REGISTERS       *AhciRegisters,
  IN UINT8                    Port,
  IN UINT8                    PortMultiplier,
  IN OUT ATA_IDENTIFY_DATA    *Buffer
  )
{
  EFI_STATUS                   Status;
  EFI_ATA_COMMAND_BLOCK        AtaCommandBlock;

  if (AhciRegisters == NULL || Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&AtaCommandBlock, sizeof (EFI_ATA_COMMAND_BLOCK));

  AtaCommandBlock.AtaCommand     = ATA_CMD_IDENTIFY_DRIVE;
  AtaCommandBlock.AtaSectorCount = 1;

  Status = AhciPioTransfer (
             AhciRegisters,
             Port,
             PortMultiplier,
             NULL,
             0,
             TRUE,
             &AtaCommandBlock,
             NULL,
             Buffer,
             sizeof (ATA_IDENTIFY_DATA),
             ATA_TIMEOUT
             );

  return Status;
}

/**
  Get AHCI mode MMIO Bar Size.

  @param[in] Bus         The bus number of ata host controller.
  @param[in] Device      The device number of ata host controller.
  @param[in] Function    The function number of ata host controller.

  @retval  The Size of AHCI MMIO BAR.

**/
UINT32
EFIAPI
GetAhciBarSize (
  IN     UINTN                       Bus,
  IN     UINTN                       Device,
  IN     UINTN                       Function
  )
{
  UINT32     Size;
  UINT32     OldBar;

  OldBar = PciRead32 (PCI_LIB_ADDRESS (Bus, Device, Function, 0x24));
  //
  // Disable PCI CMD.MSE bit before calculating MMIO Bar Size as it needs write all 1 to BAR register.
  //
  PciAnd32 (PCI_LIB_ADDRESS (Bus, Device, Function, 0x04), (UINT32)~BIT1);

  //
  // Get AHCI MMIO Bar Size.
  //
  PciWrite32 (PCI_LIB_ADDRESS (Bus, Device, Function, 0x24), 0xFFFFFFFF);
  Size = PciRead32 (PCI_LIB_ADDRESS (Bus, Device, Function, 0x24));
  Size = (~(Size & 0xFFFFFFF0)) + 1;

  //
  // Restore old MMIO Bar.
  //
  PciWrite32 (PCI_LIB_ADDRESS (Bus, Device, Function, 0x24), OldBar);
  //
  // Enable PCI CMD.MSE bit after restoring MMIO Bar.
  //
  PciOr32 (PCI_LIB_ADDRESS (Bus, Device, Function, 0x04), BIT1);

  return Size;
}

/**
  Get AHCI mode base address registers' Value.

  @param[in] Bus         The bus number of ata host controller.
  @param[in] Device      The device number of ata host controller.
  @param[in] Function    The function number of ata host controller.

  @retval EFI_UNSUPPORTED        Return this Value when the BARs is not IO type
  @retval EFI_SUCCESS            Get the Base address successfully
  @retval Other                  Read the pci configureation Data error

**/
EFI_STATUS
EFIAPI
GetAhciBaseAddress (
  IN     UINTN                       Bus,
  IN     UINTN                       Device,
  IN     UINTN                       Function
  )
{
  UINT32  Size;

  //
  // Get AHCI MMIO Bar
  //
  mAhciBar = PciRead32 (PCI_LIB_ADDRESS (Bus, Device, Function, 0x24));
  //
  // Get AHCI MMIO Bar Size
  //
  Size = GetAhciBarSize (Bus, Device, Function);
  //
  // Check if the AHCI Bar region is in SMRAM to avoid malicious attack by modifying MMIO Bar to point to SMRAM.
  //
  if (!SmmIsMmioValid ((EFI_PHYSICAL_ADDRESS)mAhciBar, Size, NULL)) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
  Allocate transfer-related Data struct which is used at AHCI mode.

  @retval  EFI_OUT_OF_RESOURCE   The allocation is failure.
  @retval  EFI_SUCCESS           Successful to allocate memory.

**/
EFI_STATUS
EFIAPI
AhciAllocateResource (
  VOID
  )
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  Base;

  //
  // Allocate resources required by AHCI host controller.
  //
  Base = 0xFFFFFFFF;
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiACPIMemoryNVS,
                  EFI_SIZE_TO_PAGES (sizeof (EFI_AHCI_RECEIVED_FIS)),
                  &Base
                  );
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem ((VOID *)(UINTN)Base, EFI_PAGE_SIZE * EFI_SIZE_TO_PAGES (sizeof (EFI_AHCI_RECEIVED_FIS)));
  mAhciRegisters.AhciRFis = (VOID *)(UINTN)Base;

  Base = 0xFFFFFFFF;
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiACPIMemoryNVS,
                  EFI_SIZE_TO_PAGES (sizeof (EFI_AHCI_COMMAND_LIST)),
                  &Base
                  );
  if (EFI_ERROR (Status)) {
    gBS->FreePages ((EFI_PHYSICAL_ADDRESS)(UINTN)mAhciRegisters.AhciRFis, EFI_SIZE_TO_PAGES (sizeof (EFI_AHCI_RECEIVED_FIS)));
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem ((VOID *)(UINTN)Base, EFI_PAGE_SIZE * EFI_SIZE_TO_PAGES (sizeof (EFI_AHCI_COMMAND_LIST)));
  mAhciRegisters.AhciCmdList = (VOID *)(UINTN)Base;

  Base = 0xFFFFFFFF;
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiACPIMemoryNVS,
                  EFI_SIZE_TO_PAGES (sizeof (EFI_AHCI_COMMAND_TABLE)),
                  &Base
                  );
  if (EFI_ERROR (Status)) {
    gBS->FreePages ((EFI_PHYSICAL_ADDRESS)(UINTN)mAhciRegisters.AhciRFis, EFI_SIZE_TO_PAGES (sizeof (EFI_AHCI_RECEIVED_FIS)));
    gBS->FreePages ((EFI_PHYSICAL_ADDRESS)(UINTN)mAhciRegisters.AhciCmdList, EFI_SIZE_TO_PAGES (sizeof (EFI_AHCI_COMMAND_LIST)));
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem ((VOID *)(UINTN)Base, EFI_PAGE_SIZE * EFI_SIZE_TO_PAGES (sizeof (EFI_AHCI_COMMAND_TABLE)));
  mAhciRegisters.AhciCommandTable = (VOID *)(UINTN)Base;
  return EFI_SUCCESS;
}

/**
  Free allocated transfer-related Data struct which is used at AHCI mode.

**/
VOID
EFIAPI
AhciFreeResource (
  VOID
  )
{
  if (mAhciRegisters.AhciRFis != NULL) {
    gBS->FreePages ((EFI_PHYSICAL_ADDRESS)(UINTN)mAhciRegisters.AhciRFis, EFI_SIZE_TO_PAGES (sizeof (EFI_AHCI_RECEIVED_FIS)));
  }

  if (mAhciRegisters.AhciCmdList != NULL) {
    gBS->FreePages ((EFI_PHYSICAL_ADDRESS)(UINTN)mAhciRegisters.AhciCmdList, EFI_SIZE_TO_PAGES (sizeof (EFI_AHCI_COMMAND_LIST)));
  }

  if (mAhciRegisters.AhciCommandTable != NULL) {
    gBS->FreePages ((EFI_PHYSICAL_ADDRESS)(UINTN)mAhciRegisters.AhciCommandTable, EFI_SIZE_TO_PAGES (sizeof (EFI_AHCI_COMMAND_TABLE)));
  }
}

/**
  Initialize ATA host controller at AHCI mode.

  The function is designed to initialize ATA host controller.

  @param[in]  Port          The port number to do initialization.

**/
EFI_STATUS
EFIAPI
AhciModeInitialize (
  UINT8              Port
  )
{
  EFI_STATUS         Status;
  UINT32             Capability;
  UINT32             Offset;
  UINT32             Data;
  UINT32             PhyDetectDelay;

  Status = AhciReset (ATA_TIMEOUT);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Collect AHCI controller information
  //
  Capability = AhciReadReg (EFI_AHCI_CAPABILITY_OFFSET);

  //
  // Enable AE before accessing any AHCI registers if Supports AHCI Mode Only is not set
  //
  if ((Capability & EFI_AHCI_CAP_SAM) == 0) {
    AhciOrReg (EFI_AHCI_GHC_OFFSET, EFI_AHCI_GHC_ENABLE);
  }

  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_FB;
  AhciWriteReg (Offset, (UINT32)(UINTN)mAhciRegisters.AhciRFis);

  //
  // Single task envrionment, we only use one command table for all port
  //
  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CLB;
  AhciWriteReg (Offset, (UINT32)(UINTN)mAhciRegisters.AhciCmdList);

  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CMD;
  Data = AhciReadReg (Offset);
  if ((Data & EFI_AHCI_PORT_CMD_CPD) != 0) {
    AhciOrReg (Offset, EFI_AHCI_PORT_CMD_POD);
  }

  if ((Capability & BIT27) != 0) {
    AhciOrReg (Offset, EFI_AHCI_PORT_CMD_SUD);
  }

  //
  // Disable aggressive power management.
  //
  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_SCTL;
  AhciOrReg (Offset, EFI_AHCI_PORT_SCTL_IPM_INIT);
  //
  // Disable the reporting of the corresponding interrupt to system software.
  //
  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_IE;
  AhciAndReg (Offset, 0);

  Status = AhciEnableFisReceive (
             Port,
             EFI_TIMER_PERIOD_MILLISECONDS(500)
             );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // According to SATA1.0a spec section 5.2, we need to wait for PxTFD.BSY and PxTFD.DRQ
  // and PxTFD.ERR to be zero. The maximum wait time is 16s which is defined at ATA spec.
  //
  PhyDetectDelay = 16 * 1000;
  do {
    Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_SERR;
    if (AhciReadReg(Offset) != 0) {
      AhciWriteReg (Offset, AhciReadReg(Offset));
    }
    Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_TFD;

    Data = AhciReadReg (Offset) & EFI_AHCI_PORT_TFD_MASK;
    if (Data == 0) {
      break;
    }

    MicroSecondDelay (1000);
    PhyDetectDelay--;
  } while (PhyDetectDelay > 0);

  if (PhyDetectDelay == 0) {
    return EFI_NOT_FOUND;
  }

  Offset = EFI_AHCI_PORT_START + Port * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_SIG;
  Status = AhciWaitMmioSet (
             Offset,
             0x0000FFFF,
             0x00000101,
             EFI_TIMER_PERIOD_SECONDS(16)
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}

