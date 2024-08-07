/** @file

Copyright (c) 2018 Loongson Technology Corporation Limited (www.loongson.cn).
All intellectual property rights(Copyright, Patent and Trademark) reserved.

Any violations of copyright or other intellectual property rights of the Loongson
Technology Corporation Limited will be held accountable in accordance with the law,
if you (or any of your subsidiaries, corporate affiliates or agents) initiate directly
or indirectly any Intellectual Property Assertion or Intellectual Property Litigation:
(i) against Loongson Technology Corporation Limited or any of its subsidiaries or corporate affiliates,
(ii) against any party if such Intellectual Property Assertion or Intellectual Property Litigation arises
in whole or in part from any software, technology, product or service of Loongson Technology Corporation Limited
or any of its subsidiaries or corporate affiliates, or (iii) against any party relating to the Software.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AS IS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION).

**/

#include <Library/DebugLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/Tpm2CommandLib.h>
#include <IndustryStandard/Tpm20.h>
#include "LsEfi.h"
#include <Library/LsSeLib.h>
VOID LsSeTpmtest(struct LoongsonSe *Se)
{
	UINT8 in[16] = {0x80,01,0x0,0x0,0x0,0xc,0x0,0x0,0x1,0x44,0x00,0x01};
  UINT8 in2[16] = {0x80,01,0x0,0x0,0x0,0xc,0x0,0x0,0x1,0x7b,0x00,0x08};
	UINT8 out[20] = {0};
  UINT8 *size=10;
	SeSendTpmCmd(Se, &in[0], 12, &out[0], &size);
  LsSePrintData(out, size); 
  size=20;
	SeSendTpmCmd(Se, &in2[0], 12, &out[0], &size);
  LsSePrintData(out, size); 
}


VOID
LsSePrintUsage (
  VOID
  )
{
  Print(L"    shell>LsEifApp.efi debug -help  \r\n");
  Print(L" \r\n");
}

EFI_STATUS
CommandRunLsSe (
  UINTN  Argc,
  CHAR16 **Argv
  )
{
  EFI_STATUS      Status;

  // LsSe SeDev;
  // Status        = EFI_SUCCESS;
  if (Argc < 2) {
    LsSePrintUsage ();
    return EFI_INVALID_PARAMETER;
  }
  DEBUG((DEBUG_INFO,"Argc:%d,Arcv %s\n",Argc, Argv[1] ));
  if (StrCmp(Argv[1], L"-init") == 0) {
    LsSeInitHw((LsSe *) (UINTN) PcdGet64 (PcdLsSeBaseAddress));
  }
  if (StrCmp(Argv[1], L"-disable") == 0) {
    // LsSeDisableHw(&SeDev);
  }
  if (StrCmp(Argv[1], L"-test") == 0) {
    LsSeTpmtest((LsSe *) (UINTN) PcdGet64 (PcdLsSeBaseAddress));
  }

  if (StrCmp(Argv[1], L"-selftest") == 0) {
    Status = Tpm2SelfTest ((TPMI_YES_NO)1);
    if(EFI_ERROR(Status))
    {
      Print(L"Tpm2SelfTest %r!\n", Status);
    }
  }

  if (StrCmp(Argv[1], L"-startup") == 0) {
    if (StrCmp(Argv[2], L"clear") == 0) {
      Status = Tpm2Startup (TPM_SU_CLEAR);
    }
    else if (StrCmp(Argv[2], L"state") == 0) {
      Status = Tpm2Startup (TPM_SU_STATE);
    }
    else
      Status=EFI_UNSUPPORTED;
    if(EFI_ERROR(Status))
    {
      Print(L"Tpm2Startup %r! %s\n", Status, Argv[2]);
    }
  }

  if (StrCmp(Argv[1], L"-shutdown") == 0) {
    if (StrCmp(Argv[2], L"clear") == 0) {
      Status = Tpm2Shutdown (TPM_SU_CLEAR);
    }
    else if (StrCmp(Argv[2], L"state") == 0) {
      Status = Tpm2Shutdown (TPM_SU_STATE);// boot mode S3
    }
    else
      Status=EFI_UNSUPPORTED;
    if(EFI_ERROR(Status))
    {
      Print(L"Tpm2Startup %r! %s\n", Status, Argv[2]);
    }
  }



  return EFI_SUCCESS;
}
