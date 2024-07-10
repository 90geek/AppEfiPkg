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
#include "LsEfi.h"
#include <LsSe.h>

LsSe SeDev;
UINT32 LsSeReadl(UINT64 Addr)
{
  return MmioRead32(SE_BASE+Addr);
}
UINT32 LsSeWritel(UINT32 Val,UINT64 Addr)
{
  return MmioWrite32(SE_BASE+Addr, Val);
}
VOID SeEnableInt(struct LoongsonSe *Se, UINT32 IntBit)
{
  UINT32 Tmp;

  if (!IntBit)
    return;
  Tmp = LsSeReadl(SE_S2LINT_EN);
  Tmp |= IntBit;
  LsSeWritel(Tmp, SE_S2LINT_EN);
}

VOID SeDisableInt(struct LoongsonSe *Se, UINT32 IntBit)
{
  UINT32 Tmp;

  if (!IntBit)
    return;

  Tmp = LsSeReadl(SE_S2LINT_EN);
  Tmp &= ~(IntBit);
  LsSeWritel(Tmp, SE_S2LINT_EN);
}

EFI_STATUS
SeSendRequeset(struct LoongsonSe *Se,
    struct SeMailboxData *Req)
{
  EFI_STATUS Status=EFI_SUCCESS;
  INT32 i;

  if (!Se || !Req)
    return EFI_INVALID_PARAMETER;

  if (LsSeReadl(SE_L2SINT_STAT) ||
      !(LsSeReadl(SE_L2SINT_EN) & Req->IntBit))
    return EFI_NOT_READY;

  for (i = 0; i < ARRAY_SIZE(Req->U.Mailbox); i++)
    LsSeWritel(Req->U.Mailbox[i], SE_MAILBOX_S + i * 4);

  LsSeWritel(Req->IntBit, SE_L2SINT_SET);

  // Status = readl_relaxed_poll_timeout_atomic(Se->Base + SE_L2SINT_STAT, Status,
  //       !(Status & Req->IntBit), 10, 10000);

  return Status;
}

EFI_STATUS
SeGetResponse(struct LoongsonSe *Se,
    struct SeMailboxData *Res)
{
  INT32 i;

  if (!Se || !Res)
    return EFI_INVALID_PARAMETER;

  while ((LsSeReadl(SE_S2LINT_STAT) & Res->IntBit) == 0)
  {
    DEBUG((DEBUG_INFO,"0x%x Res->IntBit==0x%x\n",LsSeReadl(SE_S2LINT_STAT),Res->IntBit));
  }
    // return EFI_NOT_READY;

  for (i = 0; i < ARRAY_SIZE(Res->U.Mailbox); i++)
    Res->U.Mailbox[i] = LsSeReadl(SE_MAILBOX_L + i * 4);

  LsSeWritel(Res->IntBit, SE_S2LINT_CL);

  return 0;
}

EFI_STATUS
LsSeGetRes(struct LoongsonSe *Se, UINT32 IntBit, UINT32 Cmd,
    struct SeMailboxData *Res)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Res->IntBit = IntBit;

  if (SeGetResponse(Se, Res)) {
    DEBUG((DEBUG_INFO, "INT32 0x%x get Response fail.\n", IntBit));
    return EFI_NO_RESPONSE;
  }

  /* Check response */
  if (Res->U.Res.Cmd == Cmd)
    Status = 0;
  else {
    DEBUG((DEBUG_INFO, "Response Cmd is 0x%x, not expect Cmd 0x%x.\n",
        Res->U.Res.Cmd, Cmd));
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}

EFI_STATUS
SeSendGenlCmd(struct LoongsonSe *Se, struct SeMailboxData *Req,
    struct SeMailboxData *Res, INT32 Retry)
{
  EFI_STATUS Status;
  INT32 cnt = 0;

try_again:
  if (cnt++ >= Retry) {
    Status = EFI_TIMEOUT;
    goto out;
  }

  DEBUG((DEBUG_INFO, "%d time send Cmd 0x%x\n", cnt, Req->U.gCmd.Cmd));

  Status = SeSendRequeset(Se, Req);
  if (EFI_ERROR(Status))
    goto try_again;

//   if (!wait_for_completion_timeout(&Se->cmd_completion,
//       msecs_to_jiffies(0x1000))) {
    // SeEnableInt(Se, Req->IntBit);
    // goto try_again;
  // }

  MicroSecondDelay(0x100);

  Status = LsSeGetRes(Se, Req->IntBit, Req->U.gCmd.Cmd, Res);
  if (EFI_ERROR(Status) || Res->U.Res.CmdRet) {
    SeEnableInt(Se, Req->IntBit);
    goto try_again;
  }

out:
  SeEnableInt(Se, Req->IntBit);

  return Status;
}

EFI_STATUS
LsSeSetMsg(struct LsSeCh *Ch)
{
	struct LoongsonSe *Se = Ch->Se;
	struct SeMailboxData Req = {0};
	struct SeMailboxData Res = {0};
  EFI_STATUS Status;

	Req.IntBit = SE_INT_SETUP;
	Req.U.gCmd.Cmd = SE_CMD_SETMSG;
	/* MSG off */
	Req.U.gCmd.Info[0] = Ch->Id;
	Req.U.gCmd.Info[1] = Ch->SMsg - Se->MemBase;
	Req.U.gCmd.Info[2] = Ch->MsgSize;

	DEBUG((DEBUG_INFO, "Set Channel %d msg off 0x%x, msg size %d\n", Ch->Id,
			Req.U.gCmd.Info[1], Req.U.gCmd.Info[2]));

	Status = SeSendGenlCmd(Se, &Req, &Res, 5);
	if (Res.U.Res.CmdRet)
		return Res.U.Res.CmdRet;

	return Status;
}
enum irqreturn {
        IRQ_NONE                = (0 << 0),
        IRQ_HANDLED             = (1 << 0),
        IRQ_WAKE_THREAD         = (1 << 1),
};

typedef enum irqreturn irqreturn_t;

irqreturn_t LoongsonSeIrq(INT32 Irq, VOID *DevId)
{
	struct LoongsonSe *Se = (struct LoongsonSe *)DevId;
	struct LsSeCh *Ch;
	UINT32 IntStatus;

	IntStatus = LsSeReadl(SE_S2LINT_STAT);

	DEBUG((DEBUG_INFO, "%s int status is 0x%x\n", __func__, IntStatus));

	SeDisableInt(Se, IntStatus);

	if (IntStatus & SE_INT_SETUP) {
		// complete(&Se->Cmd_completion);
		IntStatus &= ~SE_INT_SETUP;
	}

	while (IntStatus) {
		// int id = __ffs(IntStatus);
		int id = IntStatus;
		Ch = &Se->Chs[id];
		// if (Ch->complete)
			// Ch->complete(Ch);
		IntStatus &= ~BIT(id);
		LsSeWritel(BIT(id), SE_S2LINT_CL);
	}

	return IRQ_HANDLED;
}

EFI_STATUS
LsSeInitHw(struct LoongsonSe *Se)
{
	struct SeMailboxData Req = {0};
	struct SeMailboxData Res = {0};
	// struct device *Dev = Se->Dev;
  EFI_STATUS Status, Retry = 5;
	UINT64 Size;

	Size = SE_MEM_SIZE;

	// if (Size & (Size - 1)) {
	// 	Size = roundup_pow_of_two(size);
	// 	se_mem_size = size;
	// }

	SeEnableInt(Se, SE_INT_SETUP);

	/* Start engine */
  gBS->SetMem (&Req, sizeof(struct SeMailboxData), 0);
	gBS->SetMem (&Res, sizeof(struct SeMailboxData), 0);
	// memset(&Req, 0, sizeof(struct SeMailboxData));
	// memset(&Res, 0, sizeof(struct SeMailboxData));
	Req.IntBit = SE_INT_SETUP;
	Req.U.gCmd.Cmd = SE_CMD_START;
	Status = SeSendGenlCmd(Se, &Req, &Res, Retry);
	if (Status)
		return Status;

	/* Get Version */
  gBS->SetMem (&Req, sizeof(struct SeMailboxData), 0);
	gBS->SetMem (&Res, sizeof(struct SeMailboxData), 0);
	Req.IntBit = SE_INT_SETUP;
	Req.U.gCmd.Cmd = SE_CMD_GETVER;
	Status = SeSendGenlCmd(Se, &Req, &Res, Retry);
	if (Status)
		return Status;

	Se->Version = Res.U.Res.Info[0];

	/* Setup data buffer */
  Se->MemBase = AllocateZeroPool(Size);
  DEBUG((DEBUG_INFO,"Se->MemBase 0x%llx,size 0x%x\n",Se->MemBase,Size));
  if (Se->MemBase == NULL) {
    DEBUG ((EFI_D_ERROR, "%a: failed to allocate Buf Size 0x%x\n", __func__,Size));
    return EFI_INVALID_PARAMETER;
  }
	// Se->MemBase = dmam_alloc_coherent(dev, size,
			// &se->mem_addr, GFP_KERNEL);
	// if (!se->mem_base)
		// return -ENOMEM;

	// memset(se->mem_base, 0, size);

  gBS->SetMem (&Req, sizeof(struct SeMailboxData), 0);
	gBS->SetMem (&Res, sizeof(struct SeMailboxData), 0);
	// memset(&Req, 0, sizeof(struct SeMailboxData));
	// memset(&Res, 0, sizeof(struct SeMailboxData));
	Req.IntBit = SE_INT_SETUP;
	Req.U.gCmd.Cmd = SE_CMD_SETBUF;
	/* MMAP */
	Req.U.gCmd.Info[0] = (Se->MemAddr & 0xffffffff) | 0x80;
	Req.U.gCmd.Info[1] = Se->MemAddr >> 32;
	/* MASK */
	Req.U.gCmd.Info[2] = ~(Size - 1);
	Req.U.gCmd.Info[3] = 0xffffffff;

	DEBUG((DEBUG_INFO,"Set win mmap 0x%llx, mask 0x%llx\n",
			((UINT64)Req.U.gCmd.Info[1] << 32) | Req.U.gCmd.Info[0],
			((UINT64)Req.U.gCmd.Info[3] << 32) | Req.U.gCmd.Info[2]));

	Status = SeSendGenlCmd(Se, &Req, &Res, Retry);
	if (Status)
		return Status;

	Se->MemMapPageSize = EFI_SIZE_TO_PAGES (Size);
	// Se->MemMap = bitmap_zalloc(se->mem_map_size, GFP_KERNEL);
	// if (!se->mem_map)
	// 	return -ENOMEM;
  Se->MemMap = AllocatePages (Se->MemMapPageSize);
  if (Se->MemMap == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  DEBUG((DEBUG_INFO,"Se->MemMap=0x%llx\n",Se->MemMap));

	DEBUG((DEBUG_INFO, "SE module setup down, shared memory size is 0x%x bytes, "
					"memory page size is 0x%x bytes\n",
					Size, Se->MemMapPageSize));

	return Status;
}

VOID LsSeDisableHw(struct LoongsonSe *Se)
{
	struct SeMailboxData Req = {0};
	struct SeMailboxData Res = {0};
	INT32 Retry = 5;

	/* Stop engine */
	Req.IntBit = SE_INT_SETUP;
	Req.U.gCmd.Cmd = SE_CMD_STOP;
	SeSendGenlCmd(Se, &Req, &Res, Retry);

	SeDisableInt(Se, SE_INT_ALL);
  FreePages (Se->MemMap,Se->MemMapPageSize);
	// kfree(Se->MemMap);
}

EFI_STATUS
SeSendChRequeset(struct LsSeCh *Ch)
{
	struct LoongsonSe *Se;
  EFI_STATUS Status=EFI_SUCCESS;
	UINT32 IntBit;

	if (!Ch)
    return EFI_INVALID_PARAMETER;

	Se = Ch->Se;
	IntBit = Ch->IntBit;

	if ((LsSeReadl(SE_L2SINT_STAT) & IntBit) ||
			!(LsSeReadl(SE_L2SINT_EN) & IntBit))
    return EFI_NOT_READY;

	SeEnableInt(Se, IntBit);
	LsSeWritel(IntBit, SE_L2SINT_SET);

	// Status = readl_relaxed_poll_timeout_atomic(Se->base + SE_L2SINT_STAT, status,
				// !(status & IntBit), 10, 10000);

	return Status;
}

struct LsSeCh *SeFindCh(INT32 Id)
{
	struct LoongsonSe *Se = &SeDev;
	if (SeChStatus(Se, BIT(Id)))
		return &Se->Chs[Id];
	else
		return NULL;
}

struct LsSeCh *SeInitCh(UINT8 Id, INT32 DataSize, INT32 MsgSize, VOID *Priv,
		VOID (*Complete)(struct lsSe_Ch *Se_Ch))
{
	struct LoongsonSe *Se = &SeDev;
	struct LsSeCh *Ch;
	// UINT64 flag;
	// INT32 DataFirst, DataNr;
	// INT32 MsgFirst, MsgNr;
	INT32 DataFirst;
	INT32 MsgFirst;

	if (!Se) {
		DEBUG((DEBUG_INFO,"SE has bot been initialized\n"));
		return NULL;
	}

	if (Id == 0 || Id > SE_CH_MAX) {
		DEBUG((DEBUG_INFO, "Channel number %d is invalid\n", Id));
		return NULL;
	}

	if (SeChstatus(Se, BIT(Id))) {
		DEBUG((DEBUG_INFO, "Channel number %d has been initialized\n", Id));
		return NULL;
	}

	// spin_lock_irqsave(&Se->DevLock, flag);

	Ch = &SeDev.Chs[Id];
	Ch->Se = Se;
	Ch->Id = Id;
	Ch->IntBit = BIT(Id);
	Se->ChStatus |= BIT(Id);

	// DataNr = round_up(DataSize, Se_mem_page) / Se_mem_page;
	// DataFirst = bitmap_find_next_zero_area(Se->mem_map, Se->mem_map_size, 0, data_nr, 0);
	// if (DataFirst >= Se->MemMapPageSize) {
	// 	DEBUG((DEBUG_INFO, "Insufficient memory space\n"));
	// 	// spin_unlock_irqrestore(&Se->dev_lock, flag);
	// 	return NULL;
	// }
	// bitmap_Set(Se->MemMap, DataFirst, data_nr);
	// Ch->DataBuffer = Se->MemBase + DataFirst * se_mem_page;
	// Ch->DataAddr = Se->MemAddr + DataFirst * se_mem_page;
	// Ch->DataSize = DataSize;
  DataFirst=0;
	Ch->DataBuffer = Se->MemBase;
	Ch->DataAddr = Se->MemAddr;
	Ch->DataSize = DataSize;

	// msg_nr = round_up(MsgSize, se_mem_page) / se_mem_page;
	// msg_first = bitmap_find_next_zero_area(se->mem_map, se->mem_map_size, 0, msg_nr, 0);
	// if (MsgFirst >= Se->MemMapPageSize) {
	// 	DEBUG((se->dev, "Insufficient memory space\n");
	// 	bitmap_clear(se->mem_map, DataFirst, data_nr);
	// 	// spin_unlock_irqrestore(&se->dev_lock, flag);
	// 	return NULL;
	// }

	// bitmap_set(Se->MemMap, MsgFirst, MsgNr);
	// Ch->SMsg = Se->MemBase + MsgFirst * se_mem_page;
	// Ch->RMsg = Ch->SMsg + MsgSize / 2;
	// Ch->MsgSize = MsgSize;
  MsgFirst=DataSize;
	Ch->SMsg = Se->MemBase + MsgFirst;
	Ch->RMsg = Ch->SMsg + MsgSize / 2;
	Ch->MsgSize = MsgSize;

	Ch->Complete = Complete;
	Ch->Priv = Priv;
	// spin_lock_init(&Ch->Ch_lock);
	// spin_unlock_irqrestore(&se->dev_lock, flag);
	if (LsSeSetMsg(Ch)) {
		DEBUG((DEBUG_INFO, "Channel %d setup message address failed\n", Id));
		return NULL;
	}
	SeEnableInt(Se, Ch->IntBit);
	return Ch;
}

void SeDeinitCh(struct LsSeCh *Ch)
{
	struct LoongsonSe *Se = &SeDev;
	// unsigned long flag;
	// INT32 First, Nr;
	INT32 Id = Ch->Id;

	if (!Se) {
		DEBUG((DEBUG_INFO,"SE has bot been initialized\n"));
		return;
	}

	if (Id == 0 || Id > SE_CH_MAX) {
		DEBUG((DEBUG_INFO, "Channel number %d is invalid\n", Id));
		return;
	}

	if (!SeChStatus(Se, BIT(Id))) {
		DEBUG((DEBUG_INFO, "Channel number %d has not been initialized\n", Id));
		return;
	}

	// spin_lock_irqsave(&se->dev_lock, flag);

	Se->ChStatus &= ~BIT(Ch->Id);

	// First = (Ch->data_buffer - Se->mem_base) / ;
	// Nr = round_up(Ch->DataSize, se_mem_page) / se_mem_page;
	// bitmap_clear(se->mem_map, First, Nr);

	// First = (Ch->smsg - se->mem_base) / se_mem_page;
	// Nr = round_up(Ch->MsgSize, se_mem_page) / se_mem_page;
	// bitmap_clear(Se->mem_map, First, Nr);

	// spin_unlock_irqrestore(&se->dev_lock, flag);

	SeDisableInt(Se, Ch->IntBit);
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

  Status        = EFI_SUCCESS;
  if (Argc < 2) {
    LsSePrintUsage ();
    return EFI_INVALID_PARAMETER;
  }
  DEBUG((DEBUG_INFO,"Argc:%d,Arcv %s\n",Argc, Argv[1] ));
  if (StrCmp(Argv[1], L"-init") == 0) {
    LsSeInitHw(&SeDev);
  }

  if (StrCmp(Argv[1], L"-disable") == 0) {
    LsSeDisableHw(&SeDev);
  }

  return EFI_SUCCESS;
}
