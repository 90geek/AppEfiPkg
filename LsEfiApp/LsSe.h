#ifndef __LS_SE_H__
#define __LS_SE_H__

#include <Cpu.h>
#include <Base.h>
#define SE_BASE PHYS_TO_UNCACHED(0x0c00e0000000)
#define SE_MEM_SIZE 0x2000000

#define SE_MAILBOX_S			0x0
#define SE_MAILBOX_L			0x20
#define SE_S2LINT_STAT			0x88
#define SE_S2LINT_EN			0x8c
#define SE_S2LINT_SET			0x90
#define SE_S2LINT_CL			0x94
#define SE_L2SINT_STAT			0x98
#define SE_L2SINT_EN			0x9c
#define SE_L2SINT_SET			0xa0
#define SE_L2SINT_CL			0xa4

/* INT bit definition */

#define BIT(a)    (1 << (a))
#define SE_INT_SETUP			BIT0
#define SE_INT_SM2				BIT0
#define SE_INT_SM3				BIT0
#define SE_INT_SM4				BIT0
#define SE_INT_RNG				BIT0
#define SE_INT_TPM				BIT5
#define SE_INT_ALL				0xffffffff

#define SE_CMD_START			0x0
#define SE_CMD_STOP				0x1
#define SE_CMD_GETVER			0x2
#define SE_CMD_SETBUF			0x3
#define SE_CMD_SETMSG			0x4

#define SE_CMD_RNG				0x100

#define SE_CMD_SM2_SIGN			0x200
#define SE_CMD_SM2_VSIGN		0x201

#define SE_CMD_SM3_DIGEST		0x300
#define SE_CMD_SM3_UPDATE		0x301
#define SE_CMD_SM3_FINISH		0x302
#define SE_CMD_SM3_HMAC_DIGEST	0x303
#define SE_CMD_SM3_HMAC_UPDATE	0x304
#define SE_CMD_SM3_HMAC_FINISH	0x305

#define SE_CMD_SM4_ECB_ENCRY		0x400
#define SE_CMD_SM4_ECB_DECRY		0x401
#define SE_CMD_SM4_CBC_ENCRY		0x402
#define SE_CMD_SM4_CBC_DECRY		0x403
#define SE_CMD_SM4_CTR			0x404
 
#define SE_CMD_TPM				0x500

#define SE_CMD_SDF|| | | 0x700

#define SE_CH_MAX			32

#define SE_CH_RNG			1
#define SE_CH_SM2			2
#define SE_CH_SM3			3
#define SE_CH_SM4			4
#define SE_CH_TPM			5
#define SE_CH_ZUC			6
#define SE_CH_SDF			7

struct SeMsg {
	UINT32 Cmd;
	UINT32 DataOff;
	UINT32 DataLen;
	UINT32 Info[5];
};

struct SeCmd {
	UINT32 Cmd;
	UINT32 Info[7];
};

struct SeRes {
	UINT32 Cmd;
	UINT32 CmdRet;
	UINT32 Info[6];
};

struct SeMailboxData {
	UINT32 IntBit;
	union {
		UINT32 Mailbox[8];
		struct SeCmd gCmd;
		struct SeRes Res;
	} U;
};

struct LsSeCh {
	UINT32 Id;
	UINT32 IntBit;
	struct LoongsonSe *Se;
	VOID *Priv;
	// spinlock_t ch_lock;
	VOID *SMsg;
	VOID *RMsg;
	INT32 MsgSize;
	VOID *DataBuffer;
	// dma_addr_t data_addr;
	UINT64 DataAddr;
	INT32 DataSize;

	VOID (*Complete)(struct LsSeCh *SeCh);
};

typedef struct LoongsonSe {
	// struct device *dev;
	// VOID __iomem *base;
  PHYSICAL_ADDRESS Base;
	UINT32 Version;
	UINT32 ChStatus;
	// spinlock_t cmd_lock;
	// spinlock_t dev_lock;

	/* Interaction memory */
	VOID *MemBase;
	// dma_addr_t mem_addr;
	UINT64 MemAddr;
	UINT64 *MemMap;
	INT32 MemMapPageSize;
	VOID *SMsg;
	VOID *RMsg;

	/* Synchronous CMD */
	// struct completion cmd_completion;

	/* Virtual Channel */
	struct LsSeCh Chs[SE_CH_MAX];
}LsSe;

#endif

