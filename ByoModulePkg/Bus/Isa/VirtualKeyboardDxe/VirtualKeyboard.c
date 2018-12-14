/** @file
==========================================================================================
      NOTICE: Copyright (c) 2006 - 2012 Byosoft Corporation. All rights reserved.
              This program and associated documentation (if any) is furnished
              under a license. Except as permitted by such license,no part of this
              program or documentation may be reproduced, stored divulged or used
              in a public system, or transmitted in any form or by any means
              without the express written consent of Byosoft Corporation.
==========================================================================================

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "VirtualKeyboard.h"

EFI_SMM_CONTROL2_PROTOCOL   *mSmmControl2 = NULL;

//
// Platform specific keyboard device path
//
PLATFORM_KEYBOARD_DEVICE_PATH     gKeyboardDevicePath = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0,
    0x1f
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_DP,
    (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8),
    EISA_PNP_ID(0x0303),
    0
  },
  gEndEntire
};

USB_CLASS_FORMAT_DEVICE_PATH gUsbClassKeyboardDevicePath = {
  {
    {
      MESSAGING_DEVICE_PATH,
      MSG_USB_CLASS_DP,
      (UINT8) (sizeof (USB_CLASS_DEVICE_PATH)),
      (UINT8) ((sizeof (USB_CLASS_DEVICE_PATH)) >> 8)
    },
    0xffff,           // VendorId 
    0xffff,           // ProductId 
    CLASS_HID,        // DeviceClass 
    SUBCLASS_BOOT,    // DeviceSubClass
    PROTOCOL_KEYBOARD // DeviceProtocol
  },

  { 
    END_DEVICE_PATH_TYPE, 
    END_ENTIRE_DEVICE_PATH_SUBTYPE, 
    END_DEVICE_PATH_LENGTH, 
    0
  }
};

//
// Function prototypes
//
/**
  Test controller is a keyboard Controller.

  @param This                 Pointer of EFI_DRIVER_BINDING_PROTOCOL
  @param Controller           driver's controller
  @param RemainingDevicePath  children device path

  @retval EFI_UNSUPPORTED controller is not floppy disk
  @retval EFI_SUCCESS     controller is floppy disk
**/
EFI_STATUS
EFIAPI
VirtualKbdControllerDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

/**
  Create KEYBOARD_CONSOLE_IN_DEV instance on controller.

  @param This         Pointer of EFI_DRIVER_BINDING_PROTOCOL
  @param Controller   driver controller handle
  @param RemainingDevicePath Children's device path

  @retval whether success to create floppy control instance.
**/
EFI_STATUS
EFIAPI
VirtualKbdControllerDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

/**
  Stop this driver on ControllerHandle. Support stoping any child handles
  created by this driver.

  @param  This              Protocol instance pointer.
  @param  ControllerHandle  Handle of device to stop driver on
  @param  NumberOfChildren  Number of Handles in ChildHandleBuffer. If number of
                            children is zero stop the entire bus driver.
  @param  ChildHandleBuffer List of Child Handles to Stop.

  @retval EFI_SUCCESS       This driver is removed ControllerHandle
  @retval other             This driver was not removed from this device

**/
EFI_STATUS
EFIAPI
VirtualKbdControllerDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN  EFI_HANDLE                     Controller,
  IN  UINTN                          NumberOfChildren,
  IN  EFI_HANDLE                     *ChildHandleBuffer
  );

/**
  Free the waiting key notify list.

  @param ListHead  Pointer to list head

  @retval EFI_INVALID_PARAMETER  ListHead is NULL
  @retval EFI_SUCCESS            Sucess to free NotifyList
**/
EFI_STATUS
KbdFreeNotifyList (
  IN OUT LIST_ENTRY           *ListHead
  );

//
// DriverBinding Protocol Instance
//
EFI_DRIVER_BINDING_PROTOCOL gVirtualKeyboardControllerDriver = {
  VirtualKbdControllerDriverSupported,
  VirtualKbdControllerDriverStart,
  VirtualKbdControllerDriverStop,
  0xa,
  NULL,
  NULL
};


BOOLEAN HasStarted = FALSE;

/**
  Test controller is a keyboard Controller.

  @param This                 Pointer of EFI_DRIVER_BINDING_PROTOCOL
  @param Controller           driver's controller
  @param RemainingDevicePath  children device path

  @retval EFI_UNSUPPORTED controller is not floppy disk
  @retval EFI_SUCCESS     controller is floppy disk
**/
EFI_STATUS
EFIAPI
VirtualKbdControllerDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                          Status;
  EFI_ISA_IO_PROTOCOL                 *IsaIo;

  if (HasStarted) {
    return EFI_ALREADY_STARTED;
  }
	
  //
  // Open the IO Abstraction(s) needed to perform the supported test
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiVirtualIsaIoProtocolGuid,
                  (VOID **) &IsaIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Close the I/O Abstraction(s) used to perform the supported test
  //
  gBS->CloseProtocol (
         Controller,
         &gEfiVirtualIsaIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return Status;
}


/**
  Create KEYBOARD_CONSOLE_IN_DEV instance on controller.

  @param This         Pointer of EFI_DRIVER_BINDING_PROTOCOL
  @param Controller   driver controller handle
  @param RemainingDevicePath Children's device path

  @retval whether success to create floppy control instance.
**/
EFI_STATUS
EFIAPI
VirtualKbdControllerDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                                Status;
  EFI_STATUS                                Status1;
  EFI_ISA_IO_PROTOCOL                       *IsaIo;
  KEYBOARD_CONSOLE_IN_DEV                   *ConsoleIn;
  UINT8                                     Data;
  EFI_DEVICE_PATH_PROTOCOL                  *ParentDevicePath; 

  if (HasStarted) {
    return EFI_ALREADY_STARTED;
  }

  DEBUG((EFI_D_ERROR, "%a()\n", __FUNCTION__));

  ConsoleIn         = NULL;
  ParentDevicePath  = (EFI_DEVICE_PATH_PROTOCOL *)&gUsbClassKeyboardDevicePath;

  Status = gBS->LocateProtocol (&gEfiVirtualIsaIoProtocolGuid, NULL, (VOID **) &IsaIo);
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }

  //
  // Allocate private data
  //
  ConsoleIn = AllocateZeroPool (sizeof (KEYBOARD_CONSOLE_IN_DEV));
  if (ConsoleIn == NULL) {
    Status      = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }
  //
  // Setup the device instance
  //
  ConsoleIn->Signature              = KEYBOARD_CONSOLE_IN_DEV_SIGNATURE;
  ConsoleIn->Handle                 = Controller;
  (ConsoleIn->ConIn).Reset          = KeyboardEfiReset;
  (ConsoleIn->ConIn).ReadKeyStroke  = KeyboardReadKeyStroke;
  ConsoleIn->DataRegisterAddress    = KEYBOARD_8042_DATA_REGISTER;
  ConsoleIn->StatusRegisterAddress  = KEYBOARD_8042_STATUS_REGISTER;
  ConsoleIn->CommandRegisterAddress = KEYBOARD_8042_COMMAND_REGISTER;
  ConsoleIn->IsaIo                  = IsaIo;
  ConsoleIn->DevicePath             = ParentDevicePath;

  ConsoleIn->ConInEx.Reset               = KeyboardEfiResetEx;
  ConsoleIn->ConInEx.ReadKeyStrokeEx     = KeyboardReadKeyStrokeEx;
  ConsoleIn->ConInEx.SetState            = KeyboardSetState;
  ConsoleIn->ConInEx.RegisterKeyNotify   = KeyboardRegisterKeyNotify;
  ConsoleIn->ConInEx.UnregisterKeyNotify = KeyboardUnregisterKeyNotify;

  InitializeListHead (&ConsoleIn->NotifyList);

  //
  // Setup the WaitForKey event
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  KeyboardWaitForKey,
                  ConsoleIn,
                  &((ConsoleIn->ConIn).WaitForKey)
                  );
  if (EFI_ERROR (Status)) {
    Status      = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }
  //
  // Setup the WaitForKeyEx event
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  KeyboardWaitForKeyEx,
                  ConsoleIn,
                  &(ConsoleIn->ConInEx.WaitForKeyEx)
                  );
  if (EFI_ERROR (Status)) {
    Status      = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }
  // Setup a periodic timer, used for reading keystrokes at a fixed interval
  //
  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  KeyboardTimerHandler,
                  ConsoleIn,
                  &ConsoleIn->TimerEvent
                  );
  if (EFI_ERROR (Status)) {
    Status      = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  Status = gBS->SetTimer (
                  ConsoleIn->TimerEvent,
                  TimerPeriodic,
                  KEYBOARD_TIMER_INTERVAL
                  );
  if (EFI_ERROR (Status)) {
    Status      = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  Status = gBS->LocateProtocol(&gEfiSmmControl2ProtocolGuid, NULL, (VOID**)&mSmmControl2);
  ASSERT_EFI_ERROR(Status);

  //
  // Reset the keyboard device
  //
  Status = ConsoleIn->ConInEx.Reset (&ConsoleIn->ConInEx, FALSE);
  if (EFI_ERROR (Status)) {
    Status      = EFI_DEVICE_ERROR;
    goto ErrorExit;
  }

  ConsoleIn->ControllerNameTable = NULL;
  AddUnicodeString2 (
    "eng",
    gVirtualKeyboardComponentName.SupportedLanguages,
    &ConsoleIn->ControllerNameTable,
    L"Virtual Keyboard Device",
    TRUE
    );
  AddUnicodeString2 (
    "en",
    gVirtualKeyboardComponentName2.SupportedLanguages,
    &ConsoleIn->ControllerNameTable,
    L"Virtual Keyboard Device",
    FALSE
    );

  //
  // Install protocol interfaces for the keyboard device.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Controller,
                  &gEfiSimpleTextInProtocolGuid,
                  &ConsoleIn->ConIn,
                  &gEfiSimpleTextInputExProtocolGuid,
                  &ConsoleIn->ConInEx,
                  &gEfiDevicePathProtocolGuid,
                  ConsoleIn->DevicePath,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "%a(),Status:%r\n", __FUNCTION__,Status));
    goto ErrorExit;
  }

  HasStarted = TRUE;
  return Status;

ErrorExit:
  if ((ConsoleIn != NULL) && (ConsoleIn->ConIn.WaitForKey != NULL)) {
    gBS->CloseEvent (ConsoleIn->ConIn.WaitForKey);
  }

  if ((ConsoleIn != NULL) && (ConsoleIn->TimerEvent != NULL)) {
    gBS->CloseEvent (ConsoleIn->TimerEvent);
  }
  if ((ConsoleIn != NULL) && (ConsoleIn->ConInEx.WaitForKeyEx != NULL)) {
    gBS->CloseEvent (ConsoleIn->ConInEx.WaitForKeyEx);
  }
  KbdFreeNotifyList (&ConsoleIn->NotifyList);
  if ((ConsoleIn != NULL) && (ConsoleIn->ControllerNameTable != NULL)) {
    FreeUnicodeStringTable (ConsoleIn->ControllerNameTable);
  }
  //
  // Since there will be no timer handler for keyboard input any more,
  // exhaust input data just in case there is still keyboard data left
  //
  if (ConsoleIn != NULL) {
    Status1 = EFI_SUCCESS;
    while (!EFI_ERROR (Status1) && (Status != EFI_DEVICE_ERROR)) {
      Status1 = KeyboardRead (ConsoleIn, &Data);;
    }
  }

  if (ConsoleIn != NULL) {
    gBS->FreePool (ConsoleIn);
  }

  return Status;
}

/**
  Stop this driver on ControllerHandle. Support stoping any child handles
  created by this driver.

  @param  This              Protocol instance pointer.
  @param  ControllerHandle  Handle of device to stop driver on
  @param  NumberOfChildren  Number of Handles in ChildHandleBuffer. If number of
                            children is zero stop the entire bus driver.
  @param  ChildHandleBuffer List of Child Handles to Stop.

  @retval EFI_SUCCESS       This driver is removed ControllerHandle
  @retval other             This driver was not removed from this device

**/
EFI_STATUS
EFIAPI
VirtualKbdControllerDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN  EFI_HANDLE                     Controller,
  IN  UINTN                          NumberOfChildren,
  IN  EFI_HANDLE                     *ChildHandleBuffer
  )
{
  EFI_STATUS                     Status;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
  KEYBOARD_CONSOLE_IN_DEV        *ConsoleIn;
  UINT8                          Data;

  //
  // Disable Keyboard
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiSimpleTextInProtocolGuid,
                  (VOID **) &ConIn,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiSimpleTextInputExProtocolGuid,
                  NULL,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ConsoleIn = KEYBOARD_CONSOLE_IN_DEV_FROM_THIS (ConIn);

  if (ConsoleIn->TimerEvent != NULL) {
    gBS->CloseEvent (ConsoleIn->TimerEvent);
    ConsoleIn->TimerEvent = NULL;
  }

  //
  // Since there will be no timer handler for keyboard input any more,
  // exhaust input data just in case there is still keyboard data left
  //
  Status = EFI_SUCCESS;
  while (!EFI_ERROR (Status)) {
    Status = KeyboardRead (ConsoleIn, &Data);;
  }
  //
  // Uninstall the SimpleTextIn and SimpleTextInEx protocols
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  Controller,
                  &gEfiSimpleTextInProtocolGuid,
                  &ConsoleIn->ConIn,
                  &gEfiSimpleTextInputExProtocolGuid,
                  &ConsoleIn->ConInEx,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  gBS->CloseProtocol (
         Controller,
         &gEfiVirtualIsaIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  //
  // Free other resources
  //
  if ((ConsoleIn->ConIn).WaitForKey != NULL) {
    gBS->CloseEvent ((ConsoleIn->ConIn).WaitForKey);
    (ConsoleIn->ConIn).WaitForKey = NULL;
  }
  if (ConsoleIn->ConInEx.WaitForKeyEx != NULL) {
    gBS->CloseEvent (ConsoleIn->ConInEx.WaitForKeyEx);
    ConsoleIn->ConInEx.WaitForKeyEx = NULL;
  }
  KbdFreeNotifyList (&ConsoleIn->NotifyList);
  FreeUnicodeStringTable (ConsoleIn->ControllerNameTable);
  gBS->FreePool (ConsoleIn);

  HasStarted = FALSE;
  return EFI_SUCCESS;
}

/**
  Free the waiting key notify list.

  @param ListHead  Pointer to list head

  @retval EFI_INVALID_PARAMETER  ListHead is NULL
  @retval EFI_SUCCESS            Sucess to free NotifyList
**/
EFI_STATUS
KbdFreeNotifyList (
  IN OUT LIST_ENTRY           *ListHead
  )
{
  KEYBOARD_CONSOLE_IN_EX_NOTIFY *NotifyNode;

  if (ListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  while (!IsListEmpty (ListHead)) {
    NotifyNode = CR (
                   ListHead->ForwardLink,
                   KEYBOARD_CONSOLE_IN_EX_NOTIFY,
                   NotifyEntry,
                   KEYBOARD_CONSOLE_IN_EX_NOTIFY_SIGNATURE
                   );
    RemoveEntryList (ListHead->ForwardLink);
    gBS->FreePool (NotifyNode);
  }

  return EFI_SUCCESS;
}

/**
  The module Entry Point for module VirtualKeyboard.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializeVirtualKeyboard(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;

  //
  // Install driver model protocol(s).
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gVirtualKeyboardControllerDriver,
             ImageHandle,
             &gVirtualKeyboardComponentName,
             &gVirtualKeyboardComponentName2
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
