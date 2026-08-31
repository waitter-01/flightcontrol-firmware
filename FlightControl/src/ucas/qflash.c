/*
 * qflash.c
 *
 *  Created on: 2024Äê5ÔÂ20ÈÕ
 *      Author: wanghaobo
 */
#include "qflash.h"

XQspiPs QspiInstance;

FlashInfo Flash_Config = {0x40000, 0x100, 512, 0x20000, 0x4000000, 0x01, 0x20, 0xFFFC0000, 1};

int QspiFlash_Init(void)
{
	int Status;
	XQspiPs_Config *QspiConfig;
	u8 WriteBuffer[4];
	u8 ReadBuffer[4];

	QspiConfig = XQspiPs_LookupConfig(XPAR_PS7_QSPI_0_DEVICE_ID);
	if (NULL == QspiConfig) {
		return XST_FAILURE;
	}

	Status = XQspiPs_CfgInitialize(&QspiInstance, QspiConfig,
					QspiConfig->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XQspiPs_SelfTest(&QspiInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XQspiPs_SetClkPrescaler(&QspiInstance, XQSPIPS_CLK_PRESCALE_8);

	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION |
					     XQSPIPS_MANUAL_START_OPTION |
					     XQSPIPS_HOLD_B_DRIVE_OPTION);

	XQspiPs_SetSlaveSelect(&QspiInstance);

	Status = FlashReadID(&QspiInstance, WriteBuffer, ReadBuffer);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


int FlashReadID(XQspiPs *QspiPtr, u8 *WriteBfrPtr, u8 *ReadBfrPtr)
{
	int Status;

	/*
	 * Read ID in Auto mode.
	 */
	WriteBfrPtr[COMMAND_OFFSET]   = XQSPIPS_FLASH_OPCODE_RDID;
	WriteBfrPtr[ADDRESS_1_OFFSET] = 0x23;		/* 3 dummy bytes */
	WriteBfrPtr[ADDRESS_2_OFFSET] = 0x08;
	WriteBfrPtr[ADDRESS_3_OFFSET] = 0x09;

	Status = XQspiPs_PolledTransfer(QspiPtr, WriteBfrPtr, ReadBfrPtr,RD_ID_SIZE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	if((ReadBfrPtr[1] != Flash_Config.ManufacturerID) || (ReadBfrPtr[3] != Flash_Config.DeviceIDMemSize))
	{
		return XST_FAILURE;
	}
	//xil_printf("FlashID=0x%x 0x%x 0x%x\n\r", ReadBfrPtr[1], ReadBfrPtr[2],ReadBfrPtr[3]);

	return XST_SUCCESS;
}

void FlashErase(u32 Address, u32 ByteCount)
{
	u8 WriteBfrPtr[4];
	u8 WriteEnableCmd = { XQSPIPS_FLASH_OPCODE_WREN };
	u8 ReadStatusCmd[] = { XQSPIPS_FLASH_OPCODE_RDSR1, 0 };  /* Must send 2 bytes */
	u32 NumSect;
	int Sector;
	u32 BankSel;
	u8 BankInitFlag = 1;
	u8 FlashStatus[2];

	if (ByteCount == (Flash_Config.NumSect * Flash_Config.SectSize) )
	{
		BulkErase(&QspiInstance, WriteBfrPtr);
		//DieErase(&QspiInstance, WriteBfrPtr);

		return;
	}

	NumSect = ByteCount/Flash_Config.SectSize + 1;

	if( ((Address + ByteCount) & Flash_Config.SectMask) ==
			((Address + (NumSect * Flash_Config.SectSize)) & Flash_Config.SectMask) )
	{
		NumSect++;
	}

	for (Sector = 0; Sector < NumSect; Sector++)
	{
		if((BankInitFlag) && (Flash_Config.FlashDeviceSize > SIXTEENMB))
		{
			/*
			 * Reset initial bank select flag
			 */
			BankInitFlag = 0;
			/*
			 * Calculate initial bank
			 */
			BankSel = Address/SIXTEENMB;
			/*
			 * Select bank
			 */
			SendBankSelect(&QspiInstance, WriteBfrPtr, BankSel);
		}

		if((BankSel != Address/SIXTEENMB) && (Flash_Config.FlashDeviceSize > SIXTEENMB))
		{
			/*
			 * Calculate initial bank
			 */
			BankSel = Address/SIXTEENMB;
			/*
			 * Select bank
			 */
			SendBankSelect(&QspiInstance, WriteBfrPtr, BankSel);
		}

		XQspiPs_PolledTransfer(&QspiInstance, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));

		WriteBfrPtr[COMMAND_OFFSET]   = XQSPIPS_FLASH_OPCODE_SE;
		WriteBfrPtr[ADDRESS_1_OFFSET] = (u8)(Address >> 16);
		WriteBfrPtr[ADDRESS_2_OFFSET] = (u8)(Address >> 8);
		WriteBfrPtr[ADDRESS_3_OFFSET] = (u8)(Address & 0xFF);

		XQspiPs_PolledTransfer(&QspiInstance, WriteBfrPtr, NULL, SEC_ERASE_SIZE);

		while (1)
		{
			/*
			 * Poll the status register of the device to determine
			 * when it completes, by sending a read status command
			 * and receiving the status byte
			 */
			XQspiPs_PolledTransfer(&QspiInstance, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));

			/*
			 * If the status indicates the write is done, then stop
			 * waiting, if a value of 0xFF in the status byte is
			 * read from the device and this loop never exits, the
			 * device slave select is possibly incorrect such that
			 * the device status is not being read
			 */
			if ((FlashStatus[1] & 0x01) == 0) {
				break;
			}
		}

		Address += Flash_Config.SectSize;

	}
	//XQspiPs_SetSlaveSelect(&QspiInstance);
}

void BulkErase(XQspiPs *QspiPtr, u8 *WriteBfrPtr)
{
	u8 WriteEnableCmd = { XQSPIPS_FLASH_OPCODE_WREN };
	u8 ReadStatusCmd[] = { XQSPIPS_FLASH_OPCODE_RDSR1, 0 };  /* Must send 2 bytes */
	u8 FlashStatus[2];

	/*
	 * Send the write enable command to the Flash so that it can be
	 * written to, this needs to be sent as a separate transfer
	 * before the erase
	 */
	XQspiPs_PolledTransfer(QspiPtr, &WriteEnableCmd, NULL,
			  sizeof(WriteEnableCmd));

	/*
	 * Setup the bulk erase command
	 */
	WriteBfrPtr[COMMAND_OFFSET]   = XQSPIPS_FLASH_OPCODE_BE;

	/*
	 * Send the bulk erase command; no receive buffer is specified
	 * since there is nothing to receive
	 */
	XQspiPs_PolledTransfer(QspiPtr, WriteBfrPtr, NULL,
				BULK_ERASE_SIZE);

	/*
	 * Wait for the erase command to the Flash to be completed
	 */
	while (1) {
		/*
		 * Poll the status register of the device to determine
		 * when it completes, by sending a read status command
		 * and receiving the status byte
		 */
		XQspiPs_PolledTransfer(QspiPtr, ReadStatusCmd,
					FlashStatus,
					sizeof(ReadStatusCmd));

		/*
		 * If the status indicates the write is done, then stop
		 * waiting; if a value of 0xFF in the status byte is
		 * read from the device and this loop never exits, the
		 * device slave select is possibly incorrect such that
		 * the device status is not being read
		 */
		if ((FlashStatus[1] & 0x01) == 0) {
			break;
		}
	}
}

int SendBankSelect(XQspiPs *QspiPtr, u8 *WriteBfrPtr, u32 BankSel)
{

	WriteBfrPtr[COMMAND_OFFSET]   = XQSPIPS_FLASH_OPCODE_BRWR;
	WriteBfrPtr[ADDRESS_1_OFFSET] = BankSel;
	/*
	* Send the Extended address register write command
	* written, no receive buffer required
	*/
	XQspiPs_PolledTransfer(QspiPtr, WriteBfrPtr, NULL,BANK_SEL_SIZE);

	return XST_SUCCESS;
}

void FlashWrite(XQspiPs *QspiPtr, u32 Address, u32 ByteCount, u8 Command,
				u8 *WriteBfrPtr)
{
	u8 WriteEnableCmd = { XQSPIPS_FLASH_OPCODE_WREN };
	u8 ReadStatusCmd[] = { XQSPIPS_FLASH_OPCODE_RDSR1, 0 };  /* Must send 2 bytes */
	u8 FlashStatus[2];
	u32 RealAddr;
	u32 BankSel;
	u8 ReadFlagSRCmd[] = {XQSPIPS_FLASH_OPCODE_READ_FLAG_SR, 0};
	u8 FlagStatus[2];

	/*
	 * Translate address based on type of connection
	 * If stacked assert the slave select based on address
	 */
	RealAddr = Address;
	/*
	 * Bank Select
	 */
	if(Flash_Config.FlashDeviceSize > SIXTEENMB) {
		/*
		 * Calculate bank
		 */
		BankSel = RealAddr/SIXTEENMB;
		/*
		 * Select bank
		 */
		SendBankSelect(QspiPtr, WriteBfrPtr, BankSel);
	}

	/*
	 * Send the write enable command to the Flash so that it can be
	 * written to, this needs to be sent as a separate transfer before
	 * the write
	 */
	XQspiPs_PolledTransfer(QspiPtr, &WriteEnableCmd, NULL,
				sizeof(WriteEnableCmd));


	/*
	 * Setup the write command with the specified address and data for the
	 * Flash
	 */
	/*
	 * This will ensure a 3B address is transferred even when address
	 * is greater than 128Mb.
	 */
	WriteBfrPtr[COMMAND_OFFSET]   = Command;
	WriteBfrPtr[ADDRESS_1_OFFSET] = (u8)((RealAddr & 0xFF0000) >> 16);
	WriteBfrPtr[ADDRESS_2_OFFSET] = (u8)((RealAddr & 0xFF00) >> 8);
	WriteBfrPtr[ADDRESS_3_OFFSET] = (u8)(RealAddr & 0xFF);

	/*
	 * Send the write command, address, and data to the Flash to be
	 * written, no receive buffer is specified since there is nothing to
	 * receive
	 */
	XQspiPs_PolledTransfer(QspiPtr, WriteBfrPtr, NULL,
				ByteCount + OVERHEAD_SIZE);

	/*
	 * Wait for the write command to the Flash to be completed, it takes
	 * some time for the data to be written
	 */
	while (1) {
		/*
		 * Poll the status register of the Flash to determine when it
		 * completes, by sending a read status command and receiving the
		 * status byte
		 */
		XQspiPs_PolledTransfer(QspiPtr, ReadStatusCmd, FlashStatus,
					sizeof(ReadStatusCmd));

		/*
		 * If the status indicates the write is done, then stop waiting,
		 * if a value of 0xFF in the status byte is read from the
		 * device and this loop never exits, the device slave select is
		 * possibly incorrect such that the device status is not being
		 * read
		 */
		if ((FlashStatus[1] & 0x01) == 0) {
			break;
		}
	}
}

void FlashRead(XQspiPs *QspiPtr, u32 Address, u32 ByteCount, u8 Command,
				u8 *WriteBfrPtr, u8 *ReadBfrPtr)
{
	u32 RealAddr;
	u32 RealByteCnt;
	u32 BankSel;
	u32 BufferIndex;
	u32 TotalByteCnt;
	u8 ShiftSize;

	/*
	 * Retain the actual byte count
	 */
	TotalByteCnt = ByteCount;

	while(((signed long)(ByteCount)) > 0) {

		/*
		 * Translate address based on type of connection
		 * If stacked assert the slave select based on address
		 */
		RealAddr = Address;

		/*
		 * Select bank
		 */
		if(Flash_Config.FlashDeviceSize > SIXTEENMB) {
			BankSel = RealAddr/SIXTEENMB;
			SendBankSelect(QspiPtr, WriteBfrPtr, BankSel);
		}

		/*
		 * If data to be read spans beyond the current bank, then
		 * calculate RealByteCnt in current bank. Else
		 * RealByteCnt is the same as ByteCount
		 */
		if((Address & BANKMASK) != ((Address+ByteCount) & BANKMASK)) {
			RealByteCnt = (Address & BANKMASK) + SIXTEENMB - Address;
		}else {
			RealByteCnt = ByteCount;
		}


		/*
		 * Setup the write command with the specified address and data for the
		 * Flash
		 */
		WriteBfrPtr[COMMAND_OFFSET]   = Command;
		WriteBfrPtr[ADDRESS_1_OFFSET] = (u8)((RealAddr & 0xFF0000) >> 16);
		WriteBfrPtr[ADDRESS_2_OFFSET] = (u8)((RealAddr & 0xFF00) >> 8);
		WriteBfrPtr[ADDRESS_3_OFFSET] = (u8)(RealAddr & 0xFF);

		if ((Command == XQSPIPS_FLASH_OPCODE_FAST_READ) || (Command == XQSPIPS_FLASH_OPCODE_DUAL_READ) ||
		    (Command == XQSPIPS_FLASH_OPCODE_QUAD_READ)) {
			RealByteCnt += DUMMY_SIZE;
		}
		/*
		 * Send the read command to the Flash to read the specified number
		 * of bytes from the Flash, send the read command and address and
		 * receive the specified number of bytes of data in the data buffer
		 */
		XQspiPs_PolledTransfer(QspiPtr, WriteBfrPtr,
				&(ReadBfrPtr[TotalByteCnt - ByteCount]),
				RealByteCnt + OVERHEAD_SIZE);

		/*
		 * To discard the first 5 dummy bytes, shift the data in read buffer
		 */
		if((Command == XQSPIPS_FLASH_OPCODE_FAST_READ) || (Command == XQSPIPS_FLASH_OPCODE_DUAL_READ) ||
			    (Command == XQSPIPS_FLASH_OPCODE_QUAD_READ)){
			ShiftSize = OVERHEAD_SIZE + DUMMY_SIZE;
		}else{
			ShiftSize =  OVERHEAD_SIZE;
		}

		for(BufferIndex = (TotalByteCnt - ByteCount);
				BufferIndex < (TotalByteCnt - ByteCount) + RealByteCnt;
				BufferIndex++) {
			ReadBfrPtr[BufferIndex] = ReadBfrPtr[BufferIndex + ShiftSize];
		}

		/*
		 * Increase address to next bank
		 */
		Address = (Address & BANKMASK) + SIXTEENMB;
		/*
		 * Decrease byte count by bytes already read.
		 */
		if ((Command == XQSPIPS_FLASH_OPCODE_FAST_READ) || (Command == XQSPIPS_FLASH_OPCODE_DUAL_READ) ||
		    (Command == XQSPIPS_FLASH_OPCODE_QUAD_READ)) {
			ByteCount = ByteCount - (RealByteCnt - DUMMY_SIZE);
		}else {
			ByteCount = ByteCount - RealByteCnt;
		}

	}
}

void FlashWriteData(u32 Address, u32 ByteCount, u8 *WriteBfrPtr)
{
	int Page;
	int PAGE_COUNT,PAGE_NUM;
	u8 WriteBuff[512 + OVERHEAD_SIZE];

	PAGE_COUNT = ByteCount/Flash_Config.PageSize;
	PAGE_NUM = ByteCount%Flash_Config.PageSize;
	for (Page = 0; Page < PAGE_COUNT; Page++)
	{
		memcpy(WriteBuff+OVERHEAD_SIZE,WriteBfrPtr+(Page * Flash_Config.PageSize),Flash_Config.PageSize);
		FlashWrite(&QspiInstance,(Page * Flash_Config.PageSize) + Address,Flash_Config.PageSize,XQSPIPS_FLASH_OPCODE_PP, WriteBuff);
	}

	if(PAGE_NUM > 0)
	{
		memcpy(WriteBuff+OVERHEAD_SIZE,WriteBfrPtr+(Page * Flash_Config.PageSize),PAGE_NUM);
		FlashWrite(&QspiInstance,(Page * Flash_Config.PageSize) + Address,PAGE_NUM,XQSPIPS_FLASH_OPCODE_PP, WriteBuff);
	}
}

void FlashReadData(u32 Address, u32 ByteCount, u8 *ReadBfrPtr)
{
	u8 WriteBuffer[4];

	FlashRead(&QspiInstance, Address, ByteCount, XQSPIPS_FLASH_OPCODE_QUAD_READ,WriteBuffer, ReadBfrPtr);
}

