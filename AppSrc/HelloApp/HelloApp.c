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
#include <IndustryStandard/WindowsUxCapsule.h>

extern UINTN  Argc;
extern CHAR16 **Argv;

//
// Define
//

/**

  This function parse application ARG.

  @return Status
**/
EFI_STATUS
GetArg (
  VOID
  );

/**
  Print APP usage.
**/
VOID
PrintUsage (
  VOID
  )
{
  Print(L"HelloApp:  usage\n");
  Print(L"  HelloApp <...> [-NR]\n");
  Print(L"  HelloApp -P <>\n");
  Print(L"  -P: Print Hello\n");
  Print(L"Parameter:\n");
}

/**
  Hello .

  @param[in]  ImageHandle     The image handle.
  @param[in]  SystemTable     The system table.

  @retval EFI_SUCCESS            Hello completed successfully.
  @retval EFI_INVALID_PARAMETER  Hello usage error.
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

  Status = GetArg();
  if (EFI_ERROR(Status)) {
    Print(L"Please use UEFI SHELL to run this application!\n", Status);
    return Status;
  }
  if (Argc < 1) {
    PrintUsage();
    return EFI_INVALID_PARAMETER;
  }
  if (StrCmp(Argv[1], L"-P") == 0) {
    Print(L"Hello\n");
    return Status;
  }
  Status = EFI_SUCCESS;
  PrintUsage();

  return Status;
}
