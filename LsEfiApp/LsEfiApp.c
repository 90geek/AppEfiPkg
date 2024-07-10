#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Guid/GlobalVariable.h>
#include <Guid/CapsuleReport.h>
#include <Guid/SystemResourceTable.h>
#include <Guid/FmpCapsule.h>
#include "LsEfi.h"
// #include <IndustryStandard/WindowsUxCapsule.h>
// #include <SetupVarData.h>
// #include <NVDataStruc/IpmiSetupDataDef.h>
// #include <NVDataStruc/DevManageSetupDataDef.h>

extern UINTN  Argc;
extern CHAR16 **Argv;

/**

  This function print buffer.

  @return Status
**/

VOID PrintData(unsigned char *buf,int size,int level)
{
    int tmp=0;
    for(tmp=0;tmp<size;tmp++)
    {
            DEBUG((level,"%02x ", buf[tmp]));
            if(((tmp+1)%16) == 0)
                DEBUG((level,"******\n"));
        }
    DEBUG((level,"\n\n"));
}

/**
  Print APP usage.
**/
STATIC VOID
PrintUsage (
  VOID
  )
{
  Print(L"LsEfiApp:  usage\n");
  Print(L"  LsEfiApp <Command...> [-NR]\n");
  Print(L"  LsEfiApp -S <Command>\n");
  Print(L"  -S: Setup Variable Data Command\n");
  Print(L"Parameter:\n");
}

/**
  Command .

  @param[in]  ImageHandle     The image handle.
  @param[in]  SystemTable     The system table.

  @retval EFI_SUCCESS            Command completed successfully.
  @retval EFI_INVALID_PARAMETER  Command usage error.
  @retval EFI_NOT_FOUND          The input file can't be found.
**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                    Status;
  UINT32 Argv_c;
  UINT32 Argv_p;
  // RETURN_STATUS                 RStatus;
  // UINTN                          Index;

  Status = GetArg();
  if (EFI_ERROR(Status)) {
    Print(L"Please use UEFI SHELL to run this application!\n", Status);
    return Status;
  }
  Argv_c=Argc-1;
  Argv_p=1;
  if (Argc < 2) {
    PrintUsage();
    return EFI_INVALID_PARAMETER;
  }
  if (StrCmp(Argv[1], L"-D") == 0) {
    return Status;
  }

  if (StrCmp(Argv[1], L"debug") == 0) {
    // Argv_c--;
    // Argv_p++;
    // CommandRunDebug (Argv_c, Argv+Argv_p);
    return Status;
  }

  if (StrCmp(Argv[1], L"spi") == 0) {
    CommandRunSpi (Argv_c, Argv+Argv_p);
    return Status;
  }

  if (StrCmp(Argv[1], L"debug") == 0) {
    // Argv_c--;
    // Argv_p++;
    // CommandRunDebug (Argv_c, Argv+Argv_p);
    return Status;
  }

#ifdef EDKII_SHELL_TOOL
  if (StrCmp(Argv[1], L"usbinfo") == 0) {
    UsbInfoEntryPoint (ImageHandle, SystemTable);
    return Status;
  }

  if (StrCmp(Argv[1], L"hoblist") == 0) {
    InitializeHobList (ImageHandle, SystemTable);
    return Status;
  }
  if (StrCmp(Argv[1], L"pcddump") == 0) {
    PcdDumpEntrypoint (Argv_c, Argv+Argv_p);
    return Status;
  }

  if (StrCmp(Argv[1], L"esrt") == 0) {
    EsrtFmpDumpEntrypoint (ImageHandle, SystemTable);
    return Status;
  }

  if (StrCmp(Argv[1], L"memattr") == 0) {
    MemoryAttributesDumpEntrypoint (ImageHandle, SystemTable);
    return Status;
  }

  if (StrCmp(Argv[1], L"meminfo") == 0) {
    MemoryTypeInfoEntrypoint (ImageHandle, SystemTable);
    return Status;
  }

#endif

  if (StrCmp(Argv[1], L"-S") == 0) {

      // LsSetupVariableData  *Buffer=NULL;
      // UINTN Size  = sizeof (LsSetupVariableData);
      // DEBUG((DEBUG_INFO,"Size =%d\n",Size));
      // Buffer= AllocateRuntimeZeroPool(Size);
      // if(Buffer == NULL) {
      //   DEBUG((DEBUG_ERROR,"AllocateRuntimeZeroPool Failed!\n"));
      //   return EFI_LOAD_ERROR;
      // }

      // GetSetuptVariableData (Buffer);
      // DEBUG((DEBUG_INFO,"BmcIpmiConfigData:\n"));
      // Size  = sizeof (BmcIpmiConfigData);
      // PrintData((unsigned char *)&Buffer->IpmiData,(int)Size,(int)DEBUG_INFO);
      // DEBUG((DEBUG_INFO,"DevMgrData:\n"));
      // Size  = sizeof (DEVICE_MANAGER_CONFIGURATION);
      // PrintData((unsigned char *)&Buffer->DevMgrData,(int)Size,(int)DEBUG_INFO);

    if (Argc >= 3) {
      if (StrCmp(Argv[2], L"default") == 0) {

        // Buffer->IpmiData.PowerRes = 1;

        // Buffer->DevMgrData.SataController0 = 0;
        // Buffer->DevMgrData.SataController1 = 0;
        // Buffer->DevMgrData.SataController2 = 0;
        // Buffer->DevMgrData.UsbController0 = 0;
        // Buffer->DevMgrData.UsbController1 = 0;
        // Buffer->DevMgrData.UsbController1 = 0;
        // Buffer->DevMgrData.EthernetController0 = 0;
        // Buffer->DevMgrData.EthernetController1 = 0;
        // Buffer->DevMgrData.WakeOnLine = 1;
        // Buffer->DevMgrData.PcieWake = 1;
        // Buffer->DevMgrData.UsbWake = 1;
        // Buffer->DevMgrData.DVFS = 1;
      }
    }

    // SetSetuptVariableData (Buffer);
    // if (Buffer != NULL)
    // {
    //   FreePool(Buffer);
    //   Buffer = NULL;
    // }
    return EFI_SUCCESS;
  }

  if (StrCmp(Argv[1], L"-U") == 0) {
    // TestUpdateSetupVariable();
  }

  Status = EFI_SUCCESS;

Done:

  return Status;
}
