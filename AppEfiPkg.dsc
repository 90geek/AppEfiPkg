## @file
#
#  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2 of the License, or (at your option) any later version.
#
##

[Defines]
  PLATFORM_NAME                  = AppEfiPkg
  PLATFORM_GUID                  = A3F2C5D1-9E47-4C8B-9D9E-6E4B6F8D2A91
  PLATFORM_VERSION               = 0.98
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/AppEfiPkg
  SUPPORTED_ARCHITECTURES        = X64|AARCH64|RISCV64|LOONGARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT

!include MdePkg/MdeLibs.dsc.inc

[PcdsFixedAtBuild.common]
  # DEBUG_ASSERT_ENABLED       0x01
  # DEBUG_PRINT_ENABLED        0x02
  # DEBUG_CODE_ENABLED         0x04
  # CLEAR_MEMORY_ENABLED       0x08
  # ASSERT_BREAKPOINT_ENABLED  0x10
  # ASSERT_DEADLOOP_ENABLED    0x20
!if $(TARGET) == RELEASE
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x23
!else
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2f
!endif

  #  DEBUG_INIT      0x00000001  // Initialization
  #  DEBUG_WARN      0x00000002  // Warnings
  #  DEBUG_LOAD      0x00000004  // Load events
  #  DEBUG_FS        0x00000008  // EFI File system
  #  DEBUG_POOL      0x00000010  // Alloc & Free (pool)
  #  DEBUG_PAGE      0x00000020  // Alloc & Free (page)
  #  DEBUG_INFO      0x00000040  // Informational debug messages
  #  DEBUG_DISPATCH  0x00000080  // PEI/DXE/SMM Dispatchers
  #  DEBUG_VARIABLE  0x00000100  // Variable
  #  DEBUG_BM        0x00000400  // Boot Manager
  #  DEBUG_BLKIO     0x00001000  // BlkIo Driver
  #  DEBUG_NET       0x00004000  // SNP Driver
  #  DEBUG_UNDI      0x00010000  // UNDI Driver
  #  DEBUG_LOADFILE  0x00020000  // LoadFile
  #  DEBUG_EVENT     0x00080000  // Event messages
  #  DEBUG_GCD       0x00100000  // Global Coherency Database changes
  #  DEBUG_CACHE     0x00200000  // Memory range cachability changes
  #  DEBUG_VERBOSE   0x00400000  // Detailed debug messages that may
  #                              // significantly impact boot performance
  #  DEBUG_ERROR     0x80000000  // Error
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x8000004F

[LibraryClasses.common]
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  # NULL|MdePkg/Library/BaseStackCheckLib/BaseStackCheckLib.inf
  #
  # Even on RELEASE. You want to see the logging.
  #
  DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  # MauUtilsLib|MultiArchUefiPkg/Library/MauUtilsLib/MauUtilsLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  Tpm2CommandLib|SecurityPkg/Library/Tpm2CommandLib/Tpm2CommandLib.inf
  Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibTcg2/Tpm2DeviceLibTcg2.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
  IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf
  OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
  RngLib|MdePkg/Library/BaseRngLibTimerLib/BaseRngLibTimerLib.inf

# [LibraryClasses.common.PEIM]
  # PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  # PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  # PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  # MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  # HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  # ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf

# [LibraryClasses.common.DXE_SMM_DRIVER]
  # HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  # MemoryAllocationLib|MdePkg/Library/SmmMemoryAllocationLib/SmmMemoryAllocationLib.inf
  # SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  # SmmMemLib|MdePkg/Library/SmmMemLib/SmmMemLib.inf
  
[LibraryClasses.common.UEFI_APPLICATION]
  # UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  # ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  # DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
  # PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf


[BuildOptions]

[Components]

  AppEfiPkg/AppSrc/HelloApp/HelloApp.inf
  AppEfiPkg/AppSrc/ResetCountApp/ResetCountApp.inf
  AppEfiPkg/AppSrc/HobList/HobList.inf
  AppEfiPkg/AppSrc/PcdDump/PcdDump.inf

  AppEfiPkg/AppSrc/Gcd/Gcd.inf
  AppEfiPkg/AppSrc/MemoryTypeInfo/MemoryTypeInfo.inf
  AppEfiPkg/AppSrc/MemoryAttributesDump/MemoryAttributesDump.inf
  # AppEfiPkg/AppSrc/HstiWsmtDump/HstiWsmtDump.inf
  AppEfiPkg/AppSrc/EsrtFmpDump/EsrtFmpDump.inf

  # AppEfiPkg/AppSrc/SmmProfileDump/SmmProfileDump.inf
  # AppEfiPkg/AppSrc/VtdDump/VtdDump.inf

  AppEfiPkg/AppSrc/EdkiiCoreDatabaseDump/DxeCoreDump/DxeCoreDumpApp.inf
  # AppEfiPkg/AppSrc/EdkiiCoreDatabaseDump/SmmCoreDump/SmmCoreDump.inf
  # AppEfiPkg/AppSrc/EdkiiCoreDatabaseDump/SmmCoreDump/SmmCoreDumpApp.inf
  # AppEfiPkg/AppSrc/EdkiiCoreDatabaseDump/PeiCoreDump/PeiCoreDump.inf
  # AppEfiPkg/AppSrc/EdkiiCoreDatabaseDump/PeiCoreDump/PeiCoreDumpApp.inf

  # AppEfiPkg/AppSrc/SmiPerf/SmiPerf.inf
  # AppEfiPkg/AppSrc/GetVariablePerf/GetVariablePerf.inf

  # AppEfiPkg/AppSrc/Tcg2DumpLog/Tcg2DumpLog.inf

  # AppEfiPkg/AppSrc/GetPciOprom/GetPciOprom.inf

  # AppEfiPkg/AppSrc/UsbInfo/UsbInfo.inf
  # AppEfiPkg/AppSrc/AtaInfo/AtaInfo.inf
  # AppEfiPkg/AppSrc/ScsiInfo/ScsiInfo.inf

  # AppEfiPkg/AppSrc/PatchMicrocode/PatchMicrocode.inf

  # AppEfiPkg/AppSrc/StackUsage/StackUsage.inf

  # AppEfiPkg/AppSrc/DbxEnroll/DbxEnroll.inf

  # AppEfiPkg/AppSrc/InitSerial/InitSerial.inf {
  # <LibraryClasses>
  #   PlatformHookLib|MdeModulePkg/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf
  #   SerialPortLib|PcAtChipsetPkg/Library/SerialIoLib/SerialIoLib.inf
  #   SerialPortLib|MdeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf
  # }
  # AppEfiPkg/AppSrc/DummyRt/DummyRt.inf

  # AppEfiPkg/AppSrc/BootOption/BootOption.inf

  # AppEfiPkg/AppSrc/DumpVirtioPciDev/DumpVirtioPciDev.inf



  # MultiArchUefiPkg/Application/EmulatorTest/EmulatorTest.inf
  # MultiArchUefiPkg/Application/LoadOpRom/LoadOpRom.inf
  # MultiArchUefiPkg/Application/SetCon/SetCon.inf {
  #   <LibraryClasses>
  #     HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
  #     FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  #     HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  #     SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
  #     PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  #     UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  # }

