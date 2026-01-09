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
#include <Guid/SystemResourceTable.h>
#include <Protocol/SimpleTextInEx.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/ShellLib.h>

extern UINTN  Argc;
extern CHAR16 **Argv;
CHAR16 VariableName[] = L"ResetCountApp";

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

///
/// What type of answer is requested.
///
typedef enum {
  AppEfiPromptResponseTypeQuitContinueOrTimeOut,
  AppEfiPromptResponseTypeAnyKeyInput,
  AppEfiPromptResponseTypeMax
} APP_EFI_PROMPT_REQUEST_TYPE;
#define TICKS_PER_MS      10000U
#define TICKS_PER_SECOND  10000000U

EFI_STATUS
WaitUserInput (
  IN APP_EFI_PROMPT_REQUEST_TYPE  Type,
  IN CHAR16   *Prompt OPTIONAL,
  IN OUT VOID *Response OPTIONAL
)
{
  EFI_INPUT_KEY Key;
  CHAR16 Buffer[128];
  UINTN Len;
  EFI_STATUS Status;

  switch (Type) {
    case AppEfiPromptResponseTypeQuitContinueOrTimeOut:
    EFI_EVENT TimeoutEvent = NULL;
    EFI_EVENT DescendEvent = NULL;
    UINT8 Timeout = 5;
    Print(L"Enter 'q' to quit, any other key to continue:");
    //
    // Create and start a timer as timeout event.
    //
    Status = gBS->CreateEvent (
                    EVT_TIMER,
                    TPL_CALLBACK,
                    NULL,
                    NULL,
                    &TimeoutEvent
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = gBS->SetTimer (
                    TimeoutEvent,
                    TimerRelative,
                    MultU64x32 (Timeout, TICKS_PER_SECOND)
                    );
    if (EFI_ERROR (Status)) {
      goto ON_EXIT;
    }

    //
    // Create and start a periodic timer as descend event by second.
    //
    Status = gBS->CreateEvent (
                    EVT_TIMER,
                    TPL_CALLBACK,
                    NULL,
                    NULL,
                    &DescendEvent
                    );
    if (EFI_ERROR (Status)) {
      goto ON_EXIT;
    }

    Status = gBS->SetTimer (
                    DescendEvent,
                    TimerPeriodic,
                    TICKS_PER_SECOND
                    );
    if (EFI_ERROR (Status)) {
      goto ON_EXIT;
    }
    while (EFI_ERROR (gBS->CheckEvent (TimeoutEvent))) {
      if (!EFI_ERROR (gBS->CheckEvent (DescendEvent))) {
        Print(L"\b%d", Timeout--);
      }
      if (gST->ConIn->ReadKeyStroke (gST->ConIn, &Key) == EFI_NOT_READY) {
        gBS->Stall (10 * TICKS_PER_MS);
        continue;
      }
      if ((Key.UnicodeChar == L'Q') || (Key.UnicodeChar == L'q')) {
        Status = EFI_ABORTED;
        goto ON_EXIT;
      }
    }
    Status = EFI_SUCCESS;

    ON_EXIT:
      Print(L"\r\n");
      if (DescendEvent != NULL) {
        gBS->CloseEvent (DescendEvent);
      }

      if (TimeoutEvent != NULL) {
        gBS->CloseEvent (TimeoutEvent);
      }
      break;
    case AppEfiPromptResponseTypeAnyKeyInput:
    Print(L"Enter 'q' to quit, any other key/delay 5s to continue:\r\n\r\n");
    while (1) {
      Print(L"> ");
      Len = 0;
      ZeroMem(Buffer, sizeof(Buffer));
      while (1) {
        gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
        if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
          Print(L"\r\n");
          break;
        }
            if (Key.UnicodeChar == CHAR_BACKSPACE) {
                if (Len > 0) {
                    Len--;
                    Print(L"\b \b");
                }
                continue;
            }
            if (Key.UnicodeChar >= 0x20 && Len < 127) {
                Buffer[Len++] = Key.UnicodeChar;
                Print(L"%c", Key.UnicodeChar);
            }
        }
        Buffer[Len] = 0;
        if (!StrCmp(Buffer, L"exit")) {
            Print(L"Bye\r\n");
            break;
        }
        Print(L"You typed: %s\r\n", Buffer);
    }
      break;
    default:
      ASSERT (FALSE);
  }
  return Status ;
}

/**
  Print APP usage.
**/
VOID
PrintUsage (
  VOID
  )
{
  Print(L"ResetCountApp:  usage\n");
  Print(L"  ResetCountApp <...> [Count]\n");
  Print(L"  ResetCountApp -help \n");
  Print(L"  ResetCountApp -info \n");
  Print(L"  ResetCountApp -clean \n");
  Print(L"  ResetCountApp -run \n");
  Print(L"  ResetCountApp -setcount count \n");
  Print(L"  -info: Print ResetCount\n");
  Print(L"  -clean: clean ResetCount\n");
  Print(L"  -run: run ResetCount, no stop\n");
  Print(L"  -setcount: run ResetCount to count stop\n");
  Print(L"Parameter:\n");
}


/**
  ResetCount .

  @param[in]  ImageHandle     The image handle.
  @param[in]  SystemTable     The system table.

  @retval EFI_SUCCESS            ResetCount completed successfully.
  @retval EFI_INVALID_PARAMETER  ResetCount usage error.
  @retval EFI_NOT_FOUND          The input file can't be found.
**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;
  UINTN Count;
  UINTN SetCount = 0;
  UINTN Size = sizeof(UINTN);

  Status = GetArg();
  if (EFI_ERROR(Status)) {
    Print(L"Please use UEFI SHELL to run this application!\n", Status);
    return Status;
  }
  if (Argc < 2) {
    PrintUsage();
    return EFI_INVALID_PARAMETER;
  }

  if (StrCmp(Argv[1], L"-help") == 0) {
    PrintUsage();
    return EFI_INVALID_PARAMETER;
  }

  if (StrCmp(Argv[1], L"-info") == 0) {
    Status = gRT->GetVariable (
            VariableName,
            &gResetCountAppGuid ,
            NULL,
            &Size,
            &Count
            );
    if (EFI_ERROR (Status)) { // First
      Print(L"No reset count %r!!!\n\n",Status);
      Count=0;
    }
    Print(L"ResetCount is %d \n",Count);
    return Status;
  }

  if (StrCmp(Argv[1], L"-clean") == 0) {
    Print(L"ResetCount is %d, will clean to 0\n",Count);
    Count=0;
    Status = gRT->SetVariable (
            VariableName,
            &gResetCountAppGuid ,
            EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
            sizeof(UINTN),
            &Count
            );
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_INFO, "%a %d %r\n",__func__, __LINE__, Status));
      return Status;
    }
    return Status;
  }

  if ((StrCmp(Argv[1], L"-run") == 0) || (StrCmp(Argv[1], L"-setcount") == 0)) {
    Status = gRT->GetVariable (
              VariableName,
              &gResetCountAppGuid ,
              NULL,
              &Size,
              &Count
              );
    if (EFI_ERROR (Status)) { // First
      DEBUG((DEBUG_INFO, "%a %d Status %r\n",__func__, __LINE__,Status));
    }
    Count++;
    Status = gRT->SetVariable (
                VariableName,
                &gResetCountAppGuid ,
                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                sizeof(UINTN),
                &Count
                );
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_INFO, "%a %d Status %r\n",__func__, __LINE__,Status));
      return Status;
    }
  }

  if (StrCmp(Argv[1], L"-setcount") == 0) {
    if (Argc > 2) {
      SetCount =StrHexToUintn((CHAR16*)Argv[2]);
      if(Count >= SetCount)
      {
        Print(L"ResetCount %d end!\n",Count);
        Status = EFI_SUCCESS;
        return Status;
      }
    }
  }

  Status = WaitUserInput (AppEfiPromptResponseTypeQuitContinueOrTimeOut, NULL, NULL);
  if (EFI_ERROR (Status)) {
    Print(L"Cancel %d reset\n",Count);
    Status = EFI_SUCCESS;
    return Status;
  }
  Print(L"Start %d reset\n",Count);
  gBS->Stall (3 * 1000*1000);
  gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);

  return Status;
}
