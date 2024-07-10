/** @file

Copyright (c) 2018 Loongson Technology Corporation Limited (www.loongson.cn).
All intellectual property rights(Copyright, Patent and Trademark) reserved.

Any violations of copyright or other intellectual property rights of the Loongson
Technology Corporation Limited will be held accountable in accordance with the law,
if you (or any of your subsidiaries, corporate affiliates or agents) initiate directly or indirectly any Intellectual Property Assertion or Intellectual Property Litigation: (i) against Loongson Technology Corporation Limited or any of its subsidiaries or corporate affiliates, (ii) against any party if such Intellectual Property Assertion or Intellectual Property Litigation arises
in whole or in part from any software, technology, product or service of Loongson Technology Corporation Limited
or any of its subsidiaries or corporate affiliates, or (iii) against any party relating to the Software.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AS IS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION).

**/
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>

#ifndef __LS_EFI_H__
#define __LS_EFI_H__

#define EDKII_SHELL_TOOL
/**
  This function parse application ARG.

  @return Status
**/
EFI_STATUS
GetArg (
  VOID
  );

/**
  Read a file.

  @param[in]  FileName        The file to be read.
  @param[out] BufferSize      The file buffer size
  @param[out] Buffer          The file buffer

  @retval EFI_SUCCESS    Read file successfully
  @retval EFI_NOT_FOUND  File not found
**/
EFI_STATUS
ReadFileToBuffer (
  IN  CHAR16                               *FileName,
  OUT UINTN                                *BufferSize,
  OUT VOID                                 **Buffer
  );

EFI_STATUS
EFIAPI
CommandRunDebug (
    UINTN  Argc,
    CHAR16 **Argv
  );

EFI_STATUS
CommandRunSpi (
    UINTN  Argc,
    CHAR16 **Argv
  );

#ifdef EDKII_SHELL_TOOL
// EdkiiShellTool
EFI_STATUS
EFIAPI
InitializeHobList (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

EFI_STATUS
EFIAPI
UsbInfoEntryPoint (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  );

EFI_STATUS
PcdDumpEntrypoint (
    UINTN  Argc,
    CHAR16 **Argv
  );

EFI_STATUS
EFIAPI
EsrtFmpDumpEntrypoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
EFIAPI
MemoryAttributesDumpEntrypoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
EFIAPI
MemoryTypeInfoEntrypoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );
#endif
#endif
