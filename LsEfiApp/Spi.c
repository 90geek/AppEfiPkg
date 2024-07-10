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
#include <Protocol/LsServiceDxe.h>
#include <Library/PrintLib.h>
#include "LsEfi.h"
VOID
SpiPrintUsage (
  VOID
  )
{
  Print(L".TH spi 0 Spi read or write.\r\n");
  Print(L".SH NAME\r\n");
  Print(L"Read or write spi flash.\r\n");
  Print(L".SH SYNOPSIS\r\n");
  Print(L" \r\n");
  Print(L"SPI [-r StartAddr Size]/[-w data/-w StartAddr data]/[-u file]/[-e]/[-l file StartAddr Size]\n");
  Print(L".SH OPTIONS\r\n");
  Print(L" \r\n");
  Print(L"  -r           - read ls7a spi from address.\r\n");
  Print(L"  -w           - write ls7a spi data.\r\n");
  Print(L"  -u           - update ls3a spi flash.\r\n");
  Print(L"  -e           - erase ls7a spi flash by Sector.\r\n");
  Print(L"  -x           - download correct firmware to Serial ROM for xhci.\r\n");
  Print(L"  -l           - write ls7a spi data from start address to end address\r\n");
  Print(L".SH DESCRIPTION\r\n");
  Print(L" \r\n");
  Print(L"NOTES:\r\n");
  Print(L"  1. This command can update flash you should be careful.\r\n");
  Print(L".SH EXAMPLES\r\n");
  Print(L" \r\n");
  Print(L"  * update the flash:\r\n");
  Print(L"    fs0:> spi -u uefi\r\n");
  Print(L" \r\n");
  Print(L"  * write the ls7a spi flash:\r\n");
  Print(L"    fs0:> spi -w 00:11:22:33:44:55\r\n");
  Print(L"    fs0:> spi -w 0x10 aa:bb:cc:dd:ee:ff\r\n");
  Print(L" \r\n");
  Print(L"  * write the ls7a spi flash:\r\n");
  Print(L"    fs0:> spi -l vbios.bin 0x1000 0x20000 \r\n");
  Print(L" \r\n");
}
EFI_STATUS
CommandRunSpi (
    UINTN  Argc,
    CHAR16 **Argv
  )
{
  EFI_STATUS               Status;
  LIST_ENTRY               *Package;
  CHAR16                   *ProblemParam;
  UINT8                    *Buffer;
  UINT8                    *TempBuffer;
  CONST CHAR16             *Lang;
  UINT32                   BufferSize;
  UINTN                    Offset;
  EFI_LS_SERVICE_PROTOCOL  *LsService = NULL;
  VOID               *FileBuffer;

  Status      = EFI_SUCCESS;
  Buffer      = NULL;

  DEBUG((DEBUG_INFO,"Argc:%d,Arcv %s\n",Argc, Argv[1] ));
  if (Argc < 2) {
    SpiPrintUsage ();
    return EFI_INVALID_PARAMETER;
  }
    if (StrCmp(Argv[1], L"-w") == 0) {
      if (Argc == 2) {
        Lang = Argv[2];
        BufferSize = StrSize(Lang) / 2;
        Buffer = AllocateZeroPool(BufferSize / 3);
        TempBuffer = Buffer;
        for (Offset = 0;Offset < BufferSize;Offset += 3) {
          *TempBuffer = StrHexToUintn (Lang + Offset);
          Print(L"%02x\n",*(TempBuffer++));
        }
        LsService->ChipsetSpi.Erase (&LsService->ChipsetSpi, EfiDataWidthUint8, (BufferSize / 3), 0);
        LsService->ChipsetSpi.Write (&LsService->ChipsetSpi, EfiDataWidthUint8, (BufferSize / 3), 0, Buffer);
      } else if (Argc == 3) {
        UINTN StartAddr =StrHexToUintn((CHAR16*)Argv[2]);
        CONST CHAR16 *Temp=Argv[3];
        BufferSize = StrSize(Temp) / 2;
        Buffer = AllocateZeroPool(BufferSize / 3);
        TempBuffer = Buffer;
        for (Offset = 0;Offset < BufferSize;Offset += 3) {
          *TempBuffer = StrHexToUintn (Temp + Offset);
          Print(L"%02x\n",*(TempBuffer++));
        }
        if ((StartAddr+(BufferSize/3))>0x1000) {
          if (Buffer != NULL) {
            FreePool (Buffer);
          }
          return EFI_INVALID_PARAMETER;
        }
        LsService->ChipsetSpi.Erase (&LsService->ChipsetSpi, EfiDataWidthUint8, (BufferSize / 3), StartAddr);
        LsService->ChipsetSpi.Write (&LsService->ChipsetSpi, EfiDataWidthUint8, (BufferSize / 3), StartAddr, Buffer);
      }
      else {
          return EFI_INVALID_PARAMETER;
      }

      if (Buffer != NULL) {
        FreePool (Buffer);
      }
    } else if (StrCmp(Argv[1], L"-r") == 0) {
      UINTN StartAddr=0,Size =0;
      UINT8 *Buf=NULL;
      UINTN Loop;
      if (Argc != 4) {
        return EFI_INVALID_PARAMETER;
      }

      StartAddr = StrHexToUintn((CHAR16*)Argv[2]);
      Size = StrHexToUintn((CHAR16*)Argv[3]);
      DEBUG ((EFI_D_ERROR, "%a: startaddr 0x%x,Size 0x%x\n", __func__,StartAddr, Size));
      if (Size > 1024*1024) {
        Print(L"%a: Size needs to be less than 1MB,Now Size 0x%x(%lldMB)\n", __func__,Size,(Size>>20));
        return EFI_INVALID_PARAMETER;
      }

      Buf = AllocateZeroPool(Size);
      if (Buf == NULL) {
        DEBUG ((EFI_D_ERROR, "%a: failed to allocate Buf Size 0x%x\n", __func__,Size));
        return EFI_INVALID_PARAMETER;
      }
      LsService->ChipsetSpi.Read (&LsService->ChipsetSpi, EfiDataWidthUint8, Size, StartAddr, Buf);
      for (Loop = 0;Loop < Size;Loop++){
        Print(L"%02x ",Buf[Loop]);
        if(((Loop+1)%16) == 0)
          Print(L"\n");
      }
      Print(L"\n");

      if (Buf != NULL) {
        FreePool (Buf);
      }
    } else if (StrCmp(Argv[1], L"-u") == 0) {

      UINTN Filesize;
      UINTN Pages = 0;

      ZeroMem((VOID *)FileBuffer, sizeof(FileBuffer));

      Status = ReadFileToBuffer(Argv[2], &Filesize, &FileBuffer);
      if (EFI_ERROR(Status)) {
        Print(L"Spi: fd image (%s) is not found.\n", Argv[2]);
        goto Done;
      }

      Print(L"spi\n");
      LsService->CpuSpi.UpDate(&LsService->CpuSpi, EfiDataWidthUint8, Filesize, 0, FileBuffer);

     if (FileBuffer != NULL) {
        Pages = Filesize % 0x1000 == 0? Filesize / 0x1000 : Filesize / 0x1000 + 1;
        if ( Filesize < 0x1000){
          FreePool (FileBuffer);
        } else {
          FreePages (FileBuffer, Pages);
        }
        FileBuffer = NULL;
      }
    } else if (StrCmp(Argv[1], L"-e") == 0) {
      LsService->ChipsetSpi.Erase(&LsService->ChipsetSpi, EfiDataWidthUint8, 0x1000, 0);
    } else if (StrCmp(Argv[1], L"-x") == 0) {
      // if (UsbSpiInit() == SHELL_SUCCESS)
        Print(L"Spi: UsbSpi.\n");
    } else if (StrCmp(Argv[1], L"-t") == 0) {
      // UsbSpiErase();
    } else if (StrCmp(Argv[1], L"-l") == 0) {
      UINTN              Filesize;
      UINTN              ParamCount;
      UINTN              StartAddr = 0,Size = 0,ResultSize = 0 ;
      if (Argc != 5) {
        Print(L"spi\n");
        return (EFI_INVALID_PARAMETER);
      }

      for ( ParamCount = 1
          ; ParamCount<=Argc
          ; ParamCount++
         ){
        DEBUG((EFI_D_ERROR, "ParamCount %d. \n", ParamCount));

        if (ParamCount == 1) {
          ZeroMem((VOID *)FileBuffer, sizeof(FileBuffer));

          Status = ReadFileToBuffer(Argv[2], &Filesize, &FileBuffer);
          if (EFI_ERROR(Status)) {
            Print(L"Spi: fd image (%s) is not found.\n", Argv[2]);
            goto Done;
          }
          DEBUG((EFI_D_WARN, "Load File done.Filesize is = 0x%x\n",Filesize));
        } else if(ParamCount == 2){
            StartAddr = StrHexToUintn((CHAR16*)Argv[ParamCount]);
            DEBUG((EFI_D_WARN, "StarAdd = 0x%x %d\n",StartAddr,StartAddr));
            if(StartAddr > 0x100000){
              DEBUG((EFI_D_WARN, "StarAdd is more than 1M\n"));
              Status = (EFI_INVALID_PARAMETER);
              goto Done;
            }
        }else if(ParamCount == 3){
            Size = StrHexToUintn((CHAR16*)Argv[ParamCount]);
            DEBUG((EFI_D_WARN, "Size = 0x%x %d\n",Size,Size));
            if(StartAddr + Size > 0x100000){
              DEBUG((EFI_D_WARN, "StarAdd + Size is more than 1M\n"));
              Status = (EFI_INVALID_PARAMETER);
              goto Done;
            }
        }
      } // end for
      if(Size < Filesize)
        ResultSize = Size;
      else
        ResultSize = Filesize;
      if(ResultSize > 0x100000){
        DEBUG((EFI_D_WARN, "ResultSize is more than 1M\n"));
        ResultSize = 0x100000;
      }
      LsService->ChipsetSpi.Erase (&LsService->ChipsetSpi, EfiDataWidthUint8, ResultSize, StartAddr);
      LsService->ChipsetSpi.Write(&LsService->ChipsetSpi, EfiDataWidthUint8, ResultSize, StartAddr, FileBuffer);
    } else {
      ASSERT(Buffer == NULL);
      SpiPrintUsage ();
      Status = (EFI_INVALID_PARAMETER);
      goto Done;
    }

Done:
  if (FileBuffer != NULL) {
    FreePool(FileBuffer);
  }

  return Status;
}
