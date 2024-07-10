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
#include <Library/DynDebugLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include "LsEfi.h"
#define DEBUG_ADDR              ((CACHED_MEMORY_ADDR) | 0x80)
extern UINT8 ModuleNum ;
extern DP_INFO DebugData[];

typedef struct{
  UINT8 Hvalue;
  UINT8 Dvalue;
}HEX_TO_DEC;

HEX_TO_DEC ConverData[] =
{
  {0x0,0},{0x1,1},{0x2,2},{0x3,3},{0x4,4},{0x5,5},{0x6,6},{0x7,7},{0x8,8},{0x9,9},
  {0x10,10},{0x11,11},{0x12,12},{0x13,13},{0x14,14},{0x15,15},{0x16,16},{0x17,17},{0x18,18},{0x19,19},
  {0x20,20},{0x21,21},{0x22,22},{0x23,23},{0x24,24},{0x25,25},{0x26,26},{0x27,27},{0x28,28},{0x29,29},
  {0x30,30},{0x31,31},{0x32,32},{0x33,33},{0x34,34},{0x35,35},{0x36,36},{0x37,37},{0x38,38},{0x39,39},
  {0x40,40},{0x41,41},{0x42,42},{0x43,43},{0x44,44},{0x45,45},{0x46,46},{0x47,47},{0x48,48},{0x49,49},
  {0x50,50},{0x51,51},{0x52,52},{0x53,53},{0x54,54},{0x55,55},{0x56,56},{0x57,57},{0x58,58},{0x59,59},
  {0x60,60},{0x61,61},{0x62,62},{0x63,63},{0x64,64}
};
VOID
DebugPrintUsage (
  VOID
  )
{
  Print(L".TH Turn on/off the log of DynDebug function.\r\n");
  Print(L".SH NAME\r\n");
  Print(L" \r\n");
  Print(L".SH SYNOPSIS\r\n");
  Print(L" \r\n");
  Print(L"LsEifApp.efi debug -on/off module index\n");
  Print(L".SH OPTIONS\r\n");
  Print(L" \r\n");
  Print(L"  -on             - turn on DynDebug log.\r\n");
  Print(L"  -off            - turn off DynDebug log.\r\n");
  Print(L".SH DESCRIPTION\r\n");
  Print(L" \r\n");
  Print(L"NOTES:\r\n");
  Print(L"  \r\n");
  Print(L".SH EXAMPLES\r\n");
  Print(L" \r\n");
  Print(L"  * turn on module 0 DynDebug. \r\n");
  Print(L"    shell> LsEifApp.efi debug -on 0.\r\n");
  Print(L" \r\n");
  Print(L"  * turn off module 1 DynDebug. \r\n");
  Print(L"    shell> LsEifApp.efi debug -on 1.\r\n");
  Print(L" \r\n");
  Print(L" \r\n");
  Print(L"    shell>LsEifApp.efi debug -help  \r\n");
  Print(L" \r\n");
}

UINT8 GetId(UINT8 Index)
{
  UINT8 i = 0,Id = 0;

  for(i = 0; i <= 64; i++){
    if(ConverData[i].Hvalue == Index){
      Id = ConverData[i].Dvalue;
      break;
    }
  }
  return Id;
}
EFI_STATUS
CommandRunDebug (
    UINTN  Argc,
    CHAR16 **Argv
  )
{
  EFI_STATUS          Status;
  CHAR16              Buff[0x20];
  UINT64              Index = 0,Data = 0;
  UINT64              Size = 0, i =0;
  CHAR16              VariableName[] = L"DebugSwitch";
  Status              = EFI_SUCCESS;
  Size                = sizeof(UINT64);

  DEBUG((DEBUG_INFO,"Argc:%d,Arcv %s\n",Argc, Argv[1] ));
  if (Argc < 2) {
    DebugPrintUsage ();
    return EFI_INVALID_PARAMETER;
  }
  if (StrCmp(Argv[1], L"-on") == 0) {
    Index = StrHexToUintn((CHAR16*)Argv[2]);
    DEBUG((DEBUG_INFO,"Index:%d\n",Index));

    if(Index > 0x64){
      return EFI_INVALID_PARAMETER;
    }

      Index = GetId(Index);
      DEBUG((DEBUG_INFO,"Index:%d\n",Index));

      Data = 0;
      Status = gRT->GetVariable (
                  VariableName,
                  &gEfiDebugSwitchGuid,
                  NULL,
                  &Size,
                  &Data
                  );
      DEBUG((DEBUG_INFO,"Variable Data 1:0x%llx\n",Data));

      Data = *((volatile UINT64 *)DEBUG_ADDR);
      DEBUG((DEBUG_INFO,"Write before Data:0x%llx\n",Data));

      if(Index >= 0 && Index <= 63){
        Data |=(0x1ULL << Index);
      }else if(Index == 64){
        Data = 0xFFFFFFFFFFFFFFFF;
      }else{
        return EFI_INVALID_PARAMETER;
      }

      *((volatile UINT64 *)DEBUG_ADDR) = Data;

      DEBUG((DEBUG_INFO,"Write After Data:0x%llx\n",*((volatile UINT64 *)DEBUG_ADDR)));

      Status = gRT->SetVariable (
                  VariableName,
                  &gEfiDebugSwitchGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  Size,
                  &Data
                  );

      ASSERT_EFI_ERROR(Status);
      Data = 0;
      Status = gRT->GetVariable (
                  VariableName,
                  &gEfiDebugSwitchGuid,
                  NULL,
                  &Size,
                  &Data
                  );
      DEBUG((DEBUG_INFO,"Variable Data 2:0x%llx\n",Data));
      Print(L"debug on Success [%llx]\n",Data);
      ASSERT_EFI_ERROR(Status);

    } else if (StrCmp(Argv[1], L"-off") == 0) {
      Index = StrHexToUintn((CHAR16*)Argv[2]);
      DEBUG((DEBUG_INFO,"Index:%d\n",Index));

      if(Index > 0x64){
        return EFI_INVALID_PARAMETER;
      }

      Index = GetId(Index);
      DEBUG((DEBUG_INFO,"Index:%d\n",Index));

      Data = 0;
      Status = gRT->GetVariable (
                  VariableName,
                  &gEfiDebugSwitchGuid,
                  NULL,
                  &Size,
                  &Data
                  );
      DEBUG((DEBUG_INFO,"Variable Data 1:0x%llx\n",Data));

      Data = *((volatile UINT64 *)DEBUG_ADDR);
      DEBUG((DEBUG_INFO,"Write before Data:0x%llx\n",Data));

      if(Index >= 0 && Index <= 63){
        Data &= ~(0x1ULL << Index);
      }else if(Index == 64){
        Data = 0x0;
      }else{
        return EFI_INVALID_PARAMETER;
      }

      *((volatile UINT64 *)DEBUG_ADDR) = Data;
      DEBUG((DEBUG_INFO,"Write After Data:0x%llx\n",*((volatile UINT64 *)DEBUG_ADDR)));

      Status = gRT->SetVariable (
                  VariableName,
                  &gEfiDebugSwitchGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  Size,
                  &Data
                  );

      ASSERT_EFI_ERROR(Status);
      Data = 0;
      Status = gRT->GetVariable (
                  VariableName,
                  &gEfiDebugSwitchGuid,
                  NULL,
                  &Size,
                  &Data
                  );
      DEBUG((DEBUG_INFO,"Variable Data 2:0x%llx\n",Data));
      ASSERT_EFI_ERROR(Status);
      Print(L"debug off Success [%llx]\n",Data);

    } else if (StrCmp(Argv[1], L"-help") == 0) {
      Print(L"**********************************************************************\n");
      Print(L"  ModuleId:\n");
      Print(L"\n");

      for(i = 0; i < ModuleNum; i++){
         AsciiStrToUnicodeStr(DebugData[i].Name,Buff);
         Print(L"  %s [%d]\n",Buff,DebugData[i].ModuleId);
      }
      Print(L"**********************************************************************\n");
      Print(L"\n");
      Print(L"**********************************************************************\n");
      Print(L"  turn  on/off modules DynDebug....\n");
      Print(L"  -debug   on/off modules \n");
      Print(L"  modules valid num is 0-64, 64 means turn on/off 0-63 all modules\n");
      Print(L"  Example:\n");
      Print(L"  [1] -debug on/off 7  [only module 7]\n");
      Print(L"  [2] -debug on/off 64 [all module]\n");
      Print(L"**********************************************************************\n");
      Print(L"\n");

      Data = *(volatile UINT64 *)(DEBUG_ADDR);
      Status = gRT->GetVariable (
                  VariableName,
                  &gEfiDebugSwitchGuid,
                  NULL,
                  &Size,
                  &Data
                  );
      Print(L"  Current Data:0x%llx\n",Data);
      ASSERT(Data == *(volatile UINT64 *)(DEBUG_ADDR));

    }else {
      return EFI_INVALID_PARAMETER;
    }

  return EFI_SUCCESS;
}
