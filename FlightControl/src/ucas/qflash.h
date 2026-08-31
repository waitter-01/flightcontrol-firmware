/*
 * qflash.h
 *
 *  Created on: 2024Äê5ÔÂ20ÈÕ
 *      Author: wanghaobo
 */

#ifndef SRC_QFLASH_H_
#define SRC_QFLASH_H_

#include "xqspips.h"
#include "xparameters.h"

#define COMMAND_OFFSET		0 /* Flash instruction */
#define ADDRESS_1_OFFSET	1 /* MSB byte of address to read or write */
#define ADDRESS_2_OFFSET	2 /* Middle byte of address to read or write */
#define ADDRESS_3_OFFSET	3 /* LSB byte of address to read or write */
#define DATA_OFFSET		4 /* Start of Data for Read/Write */
#define DUMMY_OFFSET		4 /* Dummy byte offset for fast, dual and quad
				     reads */
#define DUMMY_SIZE		1 /* Number of dummy bytes for fast, dual and
				     quad reads */
#define RD_ID_SIZE		4 /* Read ID command + 3 bytes ID response */
#define BULK_ERASE_SIZE		1 /* Bulk Erase command size */
#define SEC_ERASE_SIZE		4 /* Sector Erase command + Sector address */
#define BANK_SEL_SIZE	2 /* BRWR or EARWR command + 1 byte bank value */
#define RD_CFG_SIZE		2 /* 1 byte Configuration register + RD CFG command*/
#define WR_CFG_SIZE		3 /* WRR command + 1 byte each Status and Config Reg*/
#define DIE_ERASE_SIZE	4	/* Die Erase command + Die address */

#define OVERHEAD_SIZE		4
#define BANKMASK 0xF000000

typedef struct{
	u32 SectSize;		/* Individual sector size or
						 * combined sector size in case of parallel config*/
	u32 NumSect;		/* Total no. of sectors in one/two flash devices */
	u32 PageSize;		/* Individual page size or
						 * combined page size in case of parallel config*/
	u32 NumPage;		/* Total no. of pages in one/two flash devices */
	u32 FlashDeviceSize;	/* This is the size of one flash device
						 * NOT the combination of both devices, if present
						 */
	u8 ManufacturerID;	/* Manufacturer ID - used to identify make */
	u8 DeviceIDMemSize;	/* Byte of device ID indicating the memory size */
	u32 SectMask;		/* Mask to get sector start address */
	u8 NumDie;			/* No. of die forming a single flash */

}FlashInfo;
//{0x40000, 0x100, 512, 0x20000, 0x4000000, 0x01, 0x20, 0xFFFC0000, 1};


#define SIXTEENMB 0x1000000


int QspiFlash_Init(void);
int FlashReadID(XQspiPs *QspiPtr, u8 *WriteBfrPtr, u8 *ReadBfrPtr);
void BulkErase(XQspiPs *QspiPtr, u8 *WriteBfrPtr);
int SendBankSelect(XQspiPs *QspiPtr, u8 *WriteBfrPtr, u32 BankSel);
void FlashErase(u32 Address, u32 ByteCount);
void FlashWriteData(u32 Address, u32 ByteCount, u8 *WriteBfrPtr);
void FlashReadData(u32 Address, u32 ByteCount, u8 *ReadBfrPtr);

#endif /* SRC_QFLASH_H_ */
