#include "fpm.h"
#include <string.h>
#include <stdio.h>

#include "sbl.h"

#include "lpc11uxx.h"


extern  void delay(uint32_t ms);
#define SINGLE_FINGER_SCORE                     (3000)

unsigned char g_enrollFtrBuf[FTR_BUFFER_LENGTH] = {0};
unsigned char *g_pFtrBuf = g_enrollFtrBuf + FTR_COUNT_LENGTH;
unsigned int g_ftrLength = 0;

ENROLL_PARA g_stEnrollPara;

unsigned int g_maxEnrollNum = 3;

extern unsigned int g_cancelFlag;


int SPI8BitToGD32Bit(const unsigned char * data)
{
    unsigned char buff[4];

    buff[0] = *(data + 1);
    buff[1] = *(data + 0);
    buff[2] = *(data + 3);
    buff[3] = *(data + 2);

    return *(int*)buff;
}

void GD32BitToSPI8Bit(int big, unsigned char * little)
{
    little[1] = (unsigned char)((big & 0xff) >> 0);
    little[0] = (unsigned char)((big & 0xff00) >> 8);
    little[3] = (unsigned char)((big & 0xff0000) >> 16);
    little[2] = (unsigned char)((big & 0xff000000) >> 24);
}

int SPI8BitToGD8Bit(unsigned char * data, unsigned int length)
{
	unsigned char tmp;
    unsigned int i = 0;
	
    if(length%4)
        return -1;

    for(i = 0; i < length/4; i++) 
	{
        tmp = *(data + i * 4);
        *(data + i * 4) = *(data + i * 4 + 1);
        *(data + i * 4 + 1) = tmp;

        tmp = *(data + i * 4 + 2);
        *(data + i * 4 + 2) = *(data + i * 4 + 3);
        *(data + i * 4 + 3) = tmp;
    }

    return 0;
}

int GD8BitToSPI8Bit(unsigned char * data, unsigned int length)
{
	unsigned char tmp;
    unsigned int i = 0;
	
    if(length%4)
        return -1;

    for(i = 0; i < length/4; i++) 
	{
        tmp = *(data + i * 4);
        *(data + i * 4) = *(data + i * 4 + 1);
        *(data + i * 4 + 1) = tmp;

        tmp = *(data + i * 4 + 2);
        *(data + i * 4 + 2) = *(data + i * 4 + 3);
        *(data + i * 4 + 3) = tmp;
    }

    return 0;
}

static int fpm_get32BitCheckSum(const unsigned char * data, unsigned int len)
{
    int sum = 0;
    unsigned int i;
	
    for(i = 0; i < len/4; i++) 
	{
        sum += SPI8BitToGD32Bit(data + i * 4);
    }

    return sum;
}

static int fpm_is32BitCheckSumRight(const unsigned char *data, unsigned int len)
{
	int sum, check;
	
    if(len%4)
        return FP_PARM_ERROR;

    sum = fpm_get32BitCheckSum(data, len - 4);
    check =  SPI8BitToGD32Bit(data + len -4);
	
	if(sum != check)
        return FP_CHECK_SUM_ERROR;
    
    return FP_OK;
}

static void fpm_readSPIDummy(unsigned int num)
{
	//int i = 0;
	//unsigned char dummy = DUMMY_BYTE;
	//SPI1_CS_LOW();

    //for(i = 0; i < num; i++)
    //    SPI1_DMA_Sendbuffer(&dummy,1);

	//SPI1_CS_HIGH();
}

static int fpm_pollingResult(unsigned char addr_h, unsigned char addr_l, unsigned int whichBit)
{
    int bit, offset = 0;
	unsigned char recvBuff[2];
    int ret;
	int pollingTime = 0;
	
	commandStruct cmdMesg;
    cmdMesg.addr_h = addr_h;
    cmdMesg.addr_l = addr_l;
    cmdMesg.comd   = READ_REGISTER_COMD;
	
    if (whichBit > 15)
    {
        return FP_PARM_ERROR;
    }
    else if (whichBit > 7) 
	{
        bit = whichBit - 8;
        offset = 0;
    }
	else
	{
        bit = whichBit;
        offset = 1;
    }
    
    while(1) 
	{		
        //SPI1_CS_LOW();
        //SPI1_DMA_Recvbuffer((unsigned char *)&cmdMesg, sizeof(cmdMesg),recvBuff , sizeof(recvBuff));
        //SPI1_CS_HIGH();

        ret = FPM_CHECK_BIT(recvBuff[offset],bit);
        if(FP_OK == ret)
		{
            return FP_OK;
		}

        delay(POLL_TIME_MS); /* sleep ms */

        pollingTime++;
		if(DEFAULT_POLL_TIMES == pollingTime)
		{
    		printf("polling timeout\r\n");
            (void)fpm_abortCommand();
			return FP_TIME_OUT_ERROR;
    	}
	}	
}


void l11uxx_spi_sendByte(writeRegisterStruct data)
{

	while (!(LPC_SSP1->SR & (0x1 << 1))); //check if transmit buffer full
	LPC_SSP1->DR = data.addr_h;
	while (LPC_SSP1->SR & (0x1 << 4)); //wait while SPI busy //ATTN, locks processor!
	
	while (!(LPC_SSP1->SR & (0x1 << 1))); //check if transmit buffer full
	LPC_SSP1->DR = data.addr_l;
	while (LPC_SSP1->SR & (0x1 << 4)); //wait while SPI busy //ATTN, locks processor!
	
	while (!(LPC_SSP1->SR & (0x1 << 1))); //check if transmit buffer full
	LPC_SSP1->DR = READ_REGISTER_COMD;
	while (LPC_SSP1->SR & (0x1 << 4)); //wait while SPI busy //ATTN, locks processor!
}


int l11uxx_spi_receiveByte(void)
{
	int data = 0;
	
	while (!(LPC_SSP1->SR & (0x1 << 1))); //check if transmit buffer full
	LPC_SSP1->DR = 0xff; //send out empty packet to move data
	while (LPC_SSP1->SR & (0x1 << 4)); //wait while SPI busy
	data = LPC_SSP1->DR;

	return data;

}

static int fpm_readRegister(unsigned char addr_h, unsigned char addr_l, unsigned char *data_h, unsigned char *data_l)
{
    unsigned char readBuff[2]; 

    writeRegisterStruct writeRegisterMesg;
    writeRegisterMesg.addr_h = addr_h;
    writeRegisterMesg.addr_l = addr_l;
    writeRegisterMesg.comd   = READ_REGISTER_COMD;

//    SPI1_CS_LOW();
//    SPI1_DMA_Recvbuffer((unsigned char *)&writeRegisterMesg, sizeof(writeRegisterMesg), readBuff, sizeof(readBuff));
//    SPI1_CS_HIGH();
	
	LPC_GPIO->CLR[1] |= (1<<19);
	l11uxx_spi_sendByte(writeRegisterMesg);
	readBuff[0] = l11uxx_spi_receiveByte();
	readBuff[1] = l11uxx_spi_receiveByte();
	LPC_GPIO->SET[1] |= (1<<19);

    *data_h = readBuff[1];
    *data_l = readBuff[0];

	return FP_OK;
}


int ml8160_testSPI(uint32_t pollingTimes)
{
    uint8_t i = 0;
    uint8_t readBuffer[2];

    for(i = 0; i < pollingTimes; i++)
    {
        fpm_readRegister(0x1E, 0xCC, &readBuffer[0], &readBuffer[1]);  
        if(0x00 == readBuffer[0] && 0xA3 == readBuffer[1])
        {
            return 0;
        }

        delay(2);
    }

    return -1; 
}


static int fpm_readRegisterBit(unsigned char addr_h, unsigned char addr_l, unsigned int whichBit)
{
    int bit, offset = 0;
	unsigned char recvBuff[2];
	SBL_TRANSER_Type transfer;
	int16_t ret;
	commandStruct cmdMesg;
	
    cmdMesg.addr_h = addr_h;
    cmdMesg.addr_l = addr_l;
    cmdMesg.comd   = READ_REGISTER_COMD;

    if(whichBit > 15)
        return FP_PARM_ERROR;
    else if(whichBit > 7)
    {
        bit = whichBit - 8;
        offset = 0;
    }
    else
    {
        bit = whichBit;
        offset = 1;
    }
    
    //SPI1_CS_LOW();
    //SPI1_DMA_Recvbuffer((unsigned char*)&cmdMesg, sizeof(cmdMesg) ,recvBuff,sizeof(recvBuff));
    //SPI1_CS_HIGH();
	LPC_GPIO->CLR[1] |= (1<<19);
	//l11uxx_spi_sendByte(cmdMesg);
	recvBuff[0] = l11uxx_spi_receiveByte();
	recvBuff[1] = l11uxx_spi_receiveByte();
	LPC_GPIO->SET[1] |= (1<<19);
	
//	transfer.TxBuf = (unsigned char*)&cmdMesg;
//    transfer.TxLen = sizeof(cmdMesg);
//    transfer.RxBuf = recvBuff;
//    transfer.RxLen = sizeof(recvBuff);

//	ret = SBL_ReadWrite(&transfer);
//	if(ret != SBL_OK)
//		return ret;


    if(FPM_CHECK_BIT(recvBuff[offset],bit) == FP_OK)
    {
        return FP_OK;
    }
    else
    {
        return FP_RESULT_ERROR;
    }
}

static int fpm_writeRegister(unsigned char addr_h, unsigned char addr_l, unsigned char data_h, unsigned char data_l)
{
    unsigned char readBuff[1]; 

	SBL_TRANSER_Type transfer;
	int16_t ret;

    writeRegisterStruct writeRegisterMesg;
    writeRegisterMesg.addr_h = addr_h;
    writeRegisterMesg.addr_l = addr_l;
    writeRegisterMesg.comd   = WRITE_REGISTER_COMD;
    writeRegisterMesg.data_h = data_h;
    writeRegisterMesg.data_l = data_l;

    //SPI1_CS_LOW();
    //SPI1_DMA_Recvbuffer((unsigned char *)&writeRegisterMesg, sizeof(writeRegisterMesg), readBuff, sizeof(readBuff));
    //SPI1_CS_HIGH();
	
	    
    transfer.TxBuf = (unsigned char *)&writeRegisterMesg;
    transfer.TxLen = sizeof(writeRegisterMesg);
    transfer.RxBuf = readBuff;
    transfer.RxLen = sizeof(readBuff);

	ret = SBL_ReadWrite(&transfer);
	if(ret != SBL_OK)
		return ret;
	delay(10);

	return FP_OK;
}

static int fpm_writeRegisterBit(unsigned char addr_h, unsigned char addr_l, unsigned int whichBit)
{
	unsigned char data_h;
    unsigned char data_l;
	
    if(whichBit > 15)
    {
        return FP_PARM_ERROR;
    }

    if(whichBit > 7)
    {
        data_h = FPM_BIT(whichBit - 8);
        data_l = 0;
    }
    else
    {
        data_h = 0;
        data_l = FPM_BIT(whichBit);
    }

    return fpm_writeRegister(addr_h, addr_l, data_h, data_l);
}

static int fpm_switchSPIToDeltaC()
{
    int ret;
    ret = fpm_writeRegister(0x1E,0x28,0x49,0x01);
    if(FP_OK != ret)
        return ret;

    ret = fpm_writeRegister(0x1E,0x2A,0x10,0x40);
    if(FP_OK != ret)
        return ret;

    ret = fpm_writeRegister(0x1E,0xBC,0x80,0x00);
    if(FP_OK != ret)
        return ret;

    return FP_OK;
}

static int fpm_switchSPIToRegister()
{
    int ret;
    ret = fpm_writeRegister(0x1E,0x28,0x41,0x01);
    if(FP_OK != ret)
        return ret;

    ret = fpm_writeRegister(0x1E,0x2A,0x50,0x40);
    if(FP_OK != ret)
        return ret;

    ret = fpm_writeRegister(0x1E,0xBC,0X00,0x00);
    if(FP_OK != ret)
        return ret;

    return FP_OK;
}

static int fpm_readDeltaC(unsigned char *buffer, unsigned int readLen, unsigned short offset16Bit)
{
	unsigned char offset_hight = offset16Bit/0x100;
    unsigned char offset_low   = offset16Bit%0x100;

    commandStruct cmdMesg;
	
    int ret = fpm_switchSPIToDeltaC();
    if(FP_OK != ret)
        return ret;

	cmdMesg.addr_h = offset_hight;
    cmdMesg.addr_l = offset_low;
    cmdMesg.comd = READ_K_DATA_COMD;
	
    fpm_readSPIDummy(4);

    //SPI1_CS_LOW();  
    //SPI1_DMA_Recvbuffer((unsigned char *)&cmdMesg , sizeof(cmdMesg), buffer, readLen);
    //SPI1_CS_HIGH();

	return fpm_switchSPIToRegister();
}

static int fpm_writeDeltaC(unsigned char *buffer, unsigned int writeLen, unsigned short offset16Bit)
{
	commandStruct cmdMesg;
	unsigned char tempBuffer[sizeof(cmdMesg)];
	int i = 0;
	unsigned char offset_hight = offset16Bit/0x100;
    unsigned char offset_low   = offset16Bit%0x100;

    int ret = fpm_switchSPIToDeltaC();
	if(FP_OK != ret)
        return ret;

    cmdMesg.addr_h = offset_hight;
    cmdMesg.addr_l = offset_low;
    cmdMesg.comd = WRITE_K_DATA_COMD;	   

    fpm_readSPIDummy(4);    

	for(i = 0 ; i < sizeof(cmdMesg); i++)
	{
		*(tempBuffer + i) = *((unsigned char *)&cmdMesg + i);
	}

//	SPI1_CS_LOW();
//    SPI1_DMA_Sendbuffer(tempBuffer, sizeof(cmdMesg));
//	SPI1_DMA_Sendbuffer(buffer , writeLen);
//	SPI1_CS_HIGH();
	
	return fpm_switchSPIToRegister();
}

static int fpm_clearSysStatus(void)
{
    return fpm_writeRegister(SYS_STATUS_ADDR_H, SYS_STATUS_ADDR_L, 0x00, 0x00);
}

static int fpm_clearAlgoStatus(void)
{
    return fpm_writeRegister(ALGO_STATUS_ADDR_H, ALGO_STATUS_ADDR_L, 0x00, 0x00);
}

/*
 * ml8160_pollIntPinHigh ： 将指纹sensor的INT管脚拉高
 *
 */
static void fpm_pollIntPinHigh(void)
{
//    uint8_t hByte, lByte;

//    fpm_readRegister(0x14,0x94, &hByte, &lByte);
//    lByte |= 0x04;
//    fpm_writeRegister(0x14,0x94, hByte, lByte);
}



/*****************************************************************************
*	App
*****************************************************************************/

int fpm_abortCommand(void)
{
    int ret = fpm_writeRegisterBit(ALGO_CMD_ADDR_H, ALGO_CMD_ADDR_L, ALGO_ABORT_BIT);
    if(ret != FP_OK)
        return ret;

    return fpm_readRegisterBit(ALGO_STATUS_ADDR_H, ALGO_STATUS_ADDR_L, ALGO_ABORT_BIT);
}

int fpm_readyForCommand()
{
    return fpm_pollingResult(SYS_STATUS_ADDR_H, SYS_STATUS_ADDR_L, SYS_RDY_FOR_CMD_BIT);
}


/*
 * ml8160_buttonScan ： buttonScan模式
 *
 * return : ml8160_ret_timeOutError ： 超时，可能是SPI问题或者sensor工作不正常
 *          ml8160_ret_ok ： 成功
 */
int fpm_buttonScan(void)
{
    int ret = fpm_readyForCommand();
    if(0 != ret) return ret;

    fpm_pollIntPinHigh();
    fpm_writeRegisterBit(SYS_CMD_ADDR_H, SYS_CMD_ADDR_L , 1);

    return 0;
}


int fpm_getUID(unsigned char *buffer)
{
    int ret = fpm_readyForCommand();
    if(ret != FP_OK)
        return ret;

    ret = fpm_writeRegisterBit(SYS_CMD_ADDR_H, SYS_CMD_ADDR_L, SYS_GET_UID_BIT);
    if(FP_OK != ret)
        return ret;

    ret = fpm_pollingResult(SYS_STATUS_ADDR_H, SYS_STATUS_ADDR_L, SYS_GET_UID_BIT);
    if( ret != FP_OK)
        return ret;

    ret = fpm_readDeltaC(buffer, 36, 0);
    if(FP_OK != ret)
        return ret;

    return fpm_is32BitCheckSumRight(buffer, 36);
}

int fpm_checkFingerLeave()
{   
    int ret = fpm_readyForCommand();
    if( ret != FP_OK)
        return ret;

    ret = fpm_clearSysStatus();
    if( ret != FP_OK)
        return ret;

    ret = fpm_writeRegisterBit(SYS_CMD_ADDR_H, SYS_CMD_ADDR_L, SYS_FINGER_LEAVE_BIT);
    if( ret != FP_OK)
        return ret;

    ret = fpm_readyForCommand();
    if( ret != FP_OK)
        return ret;

    return fpm_readRegisterBit(SYS_STATUS_ADDR_H, SYS_STATUS_ADDR_L, SYS_FINGER_LEAVE_BIT);
}

int fpm_enrollStart(unsigned int *result, unsigned int *progress)
{
	unsigned char buffer[12];
    int ret = fpm_readyForCommand();
    if( ret != FP_OK)
        return ret;

    ret = fpm_clearAlgoStatus();
    if (ret != FP_OK)
        return ret;

    ret = fpm_writeRegisterBit(ALGO_CMD_ADDR_H, ALGO_CMD_ADDR_L, ALGO_FIRST_ENROLLMENT_BIT);
    if( ret != FP_OK)
        return ret;

    ret = fpm_pollingResult(ALGO_STATUS_ADDR_H, ALGO_STATUS_ADDR_L, ALGO_FIRST_ENROLLMENT_BIT);
    if( ret != FP_OK)
        return ret;
	
    ret = fpm_readDeltaC(buffer, 12, 0);
    if( ret != FP_OK)
        return ret;

    ret = fpm_is32BitCheckSumRight(buffer, 12);
    if( ret != FP_OK)
        return ret;

    *result = SPI8BitToGD32Bit(buffer);
    *progress = SPI8BitToGD32Bit(buffer + 4);

    return FP_OK;
}

int fpm_enrollContinue(unsigned int * result, unsigned int * progress)
{
	unsigned char buffer[12];
	int ret;

    ret = fpm_clearAlgoStatus();
    if (ret != FP_OK)
        return ret;
    
    ret = fpm_writeRegisterBit(ALGO_CMD_ADDR_H, ALGO_CMD_ADDR_L, ALGO_ENROLLMENT_BIT);
    if( ret != FP_OK)
        return ret;

    ret = fpm_pollingResult(ALGO_STATUS_ADDR_H, ALGO_STATUS_ADDR_L, ALGO_ENROLLMENT_BIT);
    if( ret != FP_OK)
	{
		(void)fpm_abortCommand();
        return ret;
	}

    do
    {
        while(fpm_readDeltaC(buffer, 12, 0) != FP_OK);
    }while(fpm_is32BitCheckSumRight(buffer, 12) != FP_OK);

    *result = SPI8BitToGD32Bit(buffer);
    *progress = SPI8BitToGD32Bit(buffer + 4);

    return FP_OK;
}

int fpm_readFtr(unsigned char * ftr, unsigned int * ftrLen)
{
    int ret, pLen, len;
	ret	= fpm_readDeltaC(ftr, 8, 6);
    if( ret != FP_OK)
        return ret;

    pLen = SPI8BitToGD32Bit(ftr);
    len = SPI8BitToGD32Bit(ftr+4);
    if(pLen - 4 != len)
        return FP_RESULT_ERROR;

    ret = fpm_readDeltaC(ftr, pLen + 8, 6);
    if( ret != FP_OK)
        return ret;

    *ftrLen = pLen + 8;

    return fpm_is32BitCheckSumRight(ftr, pLen + 8);
}

int fpm_matchStart(void)
{
	unsigned char authenticationResult[4];
	int ret = 0;

	ret = fpm_writeRegisterBit(ALGO_CMD_ADDR_H, ALGO_CMD_ADDR_L, ALGO_AUTHENTICATION_BIT);
	if( ret != FP_OK)
	{
		printf("in matchStart writeRegisterBit error! %d\r\n", ret);
		return ret;
	}

	ret = fpm_pollingResult(ALGO_STATUS_ADDR_H, ALGO_STATUS_ADDR_L, ALGO_AUTHENTICATION_BIT);
	if(ret != FP_OK)
	{
		return ret;
	}
			
	ret = fpm_readDeltaC(authenticationResult, 4, 0);
    if( ret != FP_OK)
	{
		printf("in matchStart readDeltaC error! %d\r\n", ret);
		return ret;
	}
			
	if(SPI8BitToGD32Bit(authenticationResult) != 0)
	{
		printf("in matchStart SPI8BitToGD32Bit error! %d\r\n", ret);
		return FP_RESULT_ERROR;
	}
    
	return ret;
}

int fpm_matchContinue(unsigned char * ftrPack, unsigned int length,unsigned short offset)
{
    int ret;
	ret = fpm_writeDeltaC(ftrPack, length, offset);

    return ret;
}

int fpm_matchEnd(unsigned char * matchMsg, unsigned char * result)
{
	/*************************************************************
	* ftrCount | ftrBufferIndex | resever | resever | checkSum |
	*     1      |        1           |    1      |      1     |       4       |
	*
	*ftrBufferIndex是匹配哪个buffer的ftr，如果是0，就是之前的流程，保持不变
	*如果是1就是匹配buffer1里的ftr，如果是2就是匹配buffer2里的ftr
	*************************************************************/

    int ret = fpm_writeDeltaC(matchMsg, 8, 0);
	if( ret != FP_OK)
	{
		printf("in matchStart writeDeltaC error! %d\r\n", ret);
		return ret;
	}

	ret = fpm_clearSysStatus();
	if( ret != FP_OK)
    {   
        printf("in matchContinue clearSysStatus error!%d\r\n", ret);
        return ret;
    }
        
	ret = fpm_writeRegisterBit(ALGO_CMD_ADDR_H, ALGO_CMD_ADDR_L, ALGO_MATCH_BIT);
	if( ret != FP_OK)
    {    
        printf("in matchContinue writeRegisterBit error!%d\r\n", ret);
		return ret;
    }
	
	ret = fpm_readyForCommand();
	if( ret != FP_OK)
    {
        printf("in matchContinue readyForCommand error!%d\r\n", ret);
		return ret;
    }

	ret = fpm_pollingResult(ALGO_STATUS_ADDR_H, ALGO_STATUS_ADDR_L, ALGO_MATCH_BIT);
	if(ret != FP_OK)
	{
		printf("in matchContinue pollingResult error!%d\r\n", ret);
		return ret;
	}
        
	ret = fpm_readDeltaC(result, 16, 0);
	if( ret != FP_OK)
    {
        printf("in matchContinue readDeltaC error!%d\r\n", ret);
		return ret;
    }
    
	ret = SPI8BitToGD8Bit(result, 16);
	if( ret != FP_OK)
	{
		printf("in matchContinue SPI8BitToGD8Bit error!%d\r\n", ret);
		return ret;
	}

    return ret;
}


int fpm_decryptResult(unsigned char *result, unsigned char *pass_flag, unsigned short *score, unsigned int *index,unsigned char *update_flag)
{
	//1Pass@ 分数16Bit @ 最大分数id 8Bit @@@
	if(('1' == result[0]) && ('P' == result[1]) && ('a' == result[2]) && ('s' == result[3]) && ('s' == result[4]))
	{
		*pass_flag = 1;
	}
	else if(('0' == result[0]) && ('D' == result[1]) && ('e' == result[2])  && ('n' == result[3]) && ('y' == result[4]))
	{
		*pass_flag = 0;
	}
	else 
	{
		printf("result error:%X-%X-%X-%X-%X \r\n", result[0], result[1], result[2], result[3], result[4]);
		*pass_flag = 2;
	}
	
	*score = ((result[7] * 0x100) + result[6]);
	
	*index = result[9];

    *update_flag = result[11];
	
	return FP_OK;	
}


int fpm_updateFtr(unsigned char *ftr,unsigned int *ftrlen)
{
    int ret = 0;
    unsigned int ftrPackLen,ftrDataLen;
    
	ret = fpm_readyForCommand();
	if( ret != FP_OK)
		return ret;

	ret = fpm_clearSysStatus();
	if( ret != FP_OK)
		return ret;

	ret = fpm_writeRegisterBit(ALGO_CMD_ADDR_H, ALGO_CMD_ADDR_L, ALGO_UPDATE_BIT);
	if( ret != FP_OK)
		return ret;

    ret = fpm_readyForCommand();
	if( ret != FP_OK)
		return ret;

    ret = fpm_readRegisterBit(ALGO_STATUS_ADDR_H, ALGO_STATUS_ADDR_L, ALGO_UPDATE_BIT);
    if( ret != FP_OK)
		return ret;

    ret = fpm_readDeltaC(ftr, 8, 0);
    if( ret != FP_OK)
		return ret;

    ftrPackLen = SPI8BitToGD32Bit(ftr);
    ftrDataLen = SPI8BitToGD32Bit(ftr + 4);
	if (ftrPackLen-4 != ftrDataLen)
		return FP_READWRITE_ERROR;

    *ftrlen = ftrPackLen + 8;

    ret = fpm_readDeltaC(ftr, ftrPackLen + 8 , 0);
    if( ret != FP_OK)
		return ret;

    return fpm_is32BitCheckSumRight(ftr , ftrPackLen + 8);
}


int fpm_oneShot()
{
	int ret = 0;

	ret = fpm_readyForCommand();
	if( ret != FP_OK)
		return ret;

	ret = fpm_clearSysStatus();
	if( ret != FP_OK)
		return ret;

	ret = fpm_writeRegisterBit(SYS_CMD_ADDR_H, SYS_CMD_ADDR_L, SYS_ONE_SHOT_BIT);
	if( ret != FP_OK)
		return ret;

	ret = fpm_pollingResult(SYS_STATUS_ADDR_H, SYS_STATUS_ADDR_L, SYS_ONE_SHOT_BIT);
	if( ret != FP_OK)
		return ret;

	return ret;
}


int fpm_getOneShot(unsigned char *readBuffer, unsigned int line)
{
    int ret;
    unsigned short offset;
    offset = line * 128;
	ret = fpm_readDeltaC(readBuffer, 128, offset/2);		
	if( ret != FP_OK)
        return ret;
    
	ret = SPI8BitToGD8Bit(readBuffer, 128);
	if( ret != FP_OK)
        return ret;

	return ret;
}

int fpm_configer(configureStruct * configure)
{
    unsigned char buffer[sizeof(configureStruct) + 4];

    memcpy(buffer, configure, sizeof(configureStruct));

    GD8BitToSPI8Bit(buffer, sizeof(configureStruct));
    GD32BitToSPI8Bit(fpm_get32BitCheckSum(buffer, sizeof(configureStruct)), buffer + sizeof(configureStruct));

    int ret = fpm_readyForCommand();
	if(FP_OK != ret)
		return ret;

    ret = fpm_writeDeltaC(buffer, sizeof(configureStruct) + 4, 0);
    if( ret != FP_OK)
        return ret;

    ret = fpm_writeRegisterBit(SYS_CMD_ADDR_H, SYS_CMD_ADDR_L, SYS_CONFIGURE_BIT);
    if( ret != FP_OK)
        return ret;

    return fpm_pollingResult(SYS_STATUS_ADDR_H, SYS_STATUS_ADDR_L, SYS_CONFIGURE_BIT);
}

static int fpm_switchSPIToBG()
{
    int ret = 0;
	
    ret = fpm_writeRegister(0x1E,0x28,0x11,0x01);
    if(FP_OK != ret)
        return ret;
	
    ret = fpm_writeRegister(0x1E,0x2A,0x50,0x40);
    if(FP_OK != ret)
        return ret;
	
    ret = fpm_writeRegister(0x1E,0xBC,0xA0,0x00);
    if(FP_OK != ret)
        return ret;

    return FP_OK;
}

static int fpm_readBG(unsigned char *buffer, unsigned int readLen, unsigned short offset16Bit)
{
	commandStruct cmdMesg;
    int ret = fpm_switchSPIToBG();
    if(FP_OK != ret)
        return ret;

    fpm_readSPIDummy(4);

    unsigned char offset_hight = offset16Bit/0x100;
    unsigned char offset_low   = offset16Bit%0x100;

    cmdMesg.addr_h = offset_hight;
    cmdMesg.addr_l = offset_low;
    cmdMesg.comd = READ_K_DATA_COMD;

    //SPI1_CS_LOW();
    //SPI1_DMA_Recvbuffer((unsigned char *)&cmdMesg,sizeof(cmdMesg), buffer, readLen);
    //SPI1_CS_HIGH();

    return fpm_switchSPIToRegister();
}

static int fpm_writeBG(unsigned char *buffer, unsigned int writeLen, unsigned short offset16Bit)
{
    int i = 0;
	commandStruct cmdMesg;
	unsigned char tempBuffer[sizeof(cmdMesg)];
	unsigned char offset_hight = offset16Bit/0x100;
    unsigned char offset_low   = offset16Bit%0x100;
	
    int ret = fpm_switchSPIToBG();
    if(FP_OK != ret)
        return ret;
	
	cmdMesg.addr_h = offset_hight;
    cmdMesg.addr_l = offset_low;
    cmdMesg.comd = WRITE_K_DATA_COMD;

    fpm_readSPIDummy(4);
	
	for(i = 0 ; i < sizeof(cmdMesg); i++)
	{
		*(tempBuffer + i) = *((unsigned char *)&cmdMesg + i);
	}

	//SPI1_CS_LOW();
    //SPI1_DMA_Sendbuffer(tempBuffer,sizeof(cmdMesg));
	//SPI1_DMA_Sendbuffer(buffer,writeLen);
	//SPI1_CS_HIGH();
	
    return fpm_switchSPIToRegister();
}

int fpm_getBGData(unsigned char * bgData)
{
	int ret = 0;
	unsigned int tmp = 0;
	unsigned int offset = 0; 
	unsigned int sum = 0;
	unsigned int i = 0;
	
	unsigned char BGleft[28] = {0};
	
	//unsigned char BGFlag[4] = {'B', 'G', 'O', 'K'};

	ret = fpm_readyForCommand();
	if( ret != FP_OK)
		return ret;

    ret = fpm_clearSysStatus();
    if( ret != FP_OK)
        return ret;

	ret = fpm_writeRegisterBit(SYS_CMD_ADDR_H, SYS_CMD_ADDR_L, SYS_GET_BG_DATA_BIT);
	if( ret != FP_OK)
		return ret;

	ret = fpm_pollingResult(SYS_STATUS_ADDR_H, SYS_STATUS_ADDR_L, SYS_GET_BG_DATA_BIT);
	if( ret != FP_OK)
		return ret;
	
	for(i = 0; i < BG_SUB_TIMES; i++)
	{
		ret = fpm_readBG(bgData, BG_SUB_LENGTH, offset/2);
		if( ret != FP_OK)
			return ret;

//		ret = fileSys_saveBG(bgData, BG_SUB_LENGTH, offset);
//		if( ret != FP_OK)
//			return ret;

        sum += fpm_get32BitCheckSum(bgData, BG_SUB_LENGTH);
        
		offset += BG_SUB_LENGTH;
	}

    ret = fpm_readDeltaC(BGleft, 28, 0);
    if( ret != FP_OK)
        return ret;

	tmp = SPI8BitToGD32Bit(BGleft);
	if(sum != tmp)
	{
		printf("bg checksum error! sum:%d, tmp:%d\r\n", sum, tmp);
		return FP_CHECK_SUM_ERROR;
	}

    ret = fpm_is32BitCheckSumRight(BGleft + 4, 24);
	if( ret != FP_OK)
        return ret;
	
//	ret = fileSys_saveBG(BGleft, 28, BG_LENGTH);
//	if( ret != FP_OK)
//		return ret;
	
//	ret = fileSys_saveBG(BGFlag, 4, BG_LENGTH+28);
//	if( ret != FP_OK)
//		return ret;
	
	return ret;
}

int fpm_isBGexist(void)
{
	//int ret = 0;
	unsigned char BGFlag[4] = {0};
	
//	ret = fileSys_readBG(BGFlag, 4, BG_LENGTH+28);
//	if( ret != FP_OK)
//		return ret;
	
	if( ('B' == BGFlag[0]) && ('G' == BGFlag[1]) && ('O' == BGFlag[2]) && ('K' == BGFlag[03]) )
	{
		return 1;
	}
	else 
    {   
		return 0;
    }
}

int fpm_setBGData(unsigned char * bgData )
{
    int ret = 0;
	
	int i = 0;
	
	unsigned int offset = 0;
	
	unsigned char BGleft[28] = {0};
	
	ret = fpm_readyForCommand();
    if( ret != FP_OK)
        return ret;
	
	for(i = 0; i < BG_SUB_TIMES; i++)
	{   
	    memset(bgData , 0 ,FTR_BUFFER_LENGTH);
//	    SPI2_CS_LOW();
        
        #if 0
//		ret = fileSys_readBG(bgData, BG_SUB_LENGTH, offset);
        #else
//        ret = fileSys_noDelayReadBG(bgData , BG_SUB_LENGTH , offset);
        #endif
		if( ret != FP_OK)
			return ret;
        
		ret = fpm_writeBG(bgData, BG_SUB_LENGTH, offset/2);
		if( ret != FP_OK)
			return ret;
        
//        SPI2_CS_HIGH();
        
		offset += BG_SUB_LENGTH;
	}
    
//	ret = fileSys_readBG(BGleft, 28, BG_LENGTH);
//	if( ret != FP_OK)
//		return ret;
    
	ret = fpm_writeDeltaC(BGleft, 28, 0);
    if( ret != FP_OK)
        return ret;

    ret = fpm_writeRegisterBit(SYS_CMD_ADDR_H, SYS_CMD_ADDR_L, SYS_SET_BG_DATA_BIT);
    if( ret != FP_OK)
        return ret;

    return fpm_pollingResult(SYS_STATUS_ADDR_H, SYS_STATUS_ADDR_L, SYS_SET_BG_DATA_BIT);
}


int fpm_eraseBGData(void)
{
    //return fileSys_eraseBG();
	return 0;
}


int fpm_AfisInit(unsigned int enrollNum)
{
    int ret;
    configureStruct configure;
	configure.isNeedFtrAes = 0;
	configure.isNeedMatchAes = 0;
	configure.reserved1 = 0;
    configure.ExtractAfterOneShot = 0;
	configure.enrollNum = enrollNum;
    configure.rotationLimit = 180; //360识别
    configure.singleFinger_l = 0;
    configure.singleFinger_h = 0;

    ret = fpm_configer(&configure);
    if (FP_OK != ret)
    {
        printf("confinger fail\r\n");
        return ret;
    }

    return ret;
}


int fpm_init(void)
{
	int ret = 0;

    ret = fpm_AfisInit(g_maxEnrollNum);
	if(FP_OK != ret)
	{
		printf("configer fail \r\n");
		return ret;
	}
    else
    {
        printf("configer OK\r\n");
    }

	if(1 == fpm_isBGexist())
	{
		ret = fpm_setBGData(g_enrollFtrBuf);
		if(0 != ret)
        {
			printf("setBGData error \r\n");	
        }
        else
        {
		    printf("set BG OK! \r\n");
        }
    }
	else
	{
		ret = fpm_getBGData(g_enrollFtrBuf);
		if(0 != ret)
        {      
			printf("getBGData error \r\n");	
        }
		else
        {      
		    printf("get BG OK! \r\n");
        }
		ret = fpm_setBGData(g_enrollFtrBuf);
		if(0 != ret)
        {
			printf("setBGData error \r\n");	
        }
        else
        {
		    printf("set BG OK! \r\n");
        }
	}

	return ret;
}


/*********************************************
* 以下新增是晟元协议使用
*********************************************/
int fpm_extration(int whitchPlace)
{
	int ret = 0;
	unsigned char whitchBuffer[8] = {0};
	int readData = 0xff;

	ret = fpm_readyForCommand();
	if(FP_OK != ret)
		return ret;

	GD32BitToSPI8Bit(whitchPlace, whitchBuffer);
	GD32BitToSPI8Bit(whitchPlace, whitchBuffer + 4);

	ret = fpm_writeDeltaC(whitchBuffer, sizeof(whitchBuffer), 0);
	if(FP_OK != ret)
		return ret;
	
	ret = fpm_writeRegisterBit(ALGO_CMD_ADDR_H, ALGO_CMD_ADDR_L, ALGO_EXTRATION_BIT);
    if(ret != FP_OK)
        return ret;

	ret = fpm_readyForCommand();
	if(FP_OK != ret)
		return ret;

	ret = fpm_pollingResult(ALGO_STATUS_ADDR_H, ALGO_STATUS_ADDR_L, ALGO_EXTRATION_BIT);
	if(FP_OK != ret)
		return ret;

	memset(whitchBuffer, 0, sizeof(whitchBuffer));

	ret = fpm_readDeltaC(whitchBuffer, sizeof(whitchBuffer), 0);
	if(FP_OK != ret)
		return ret;

	readData = SPI8BitToGD32Bit(whitchBuffer);
	readData += SPI8BitToGD32Bit(whitchBuffer + 4);
	if(0 != readData)
		return FP_EXTRATION_ERROR;
	
	return ret;
}

int fpm_mergeFtr(unsigned char *ftr, unsigned int * ftrLen)
{
	int ret, code, pLen, len;
	int i = 0;

	ret = fpm_writeRegisterBit(ALGO_CMD_ADDR_H, ALGO_CMD_ADDR_L, ALGO_MERGE_FTR_BIT);
    if(ret != FP_OK)
        return ret;

	ret = fpm_readyForCommand();
	if(FP_OK != ret)
		return ret;

	ret = fpm_pollingResult(ALGO_STATUS_ADDR_H, ALGO_STATUS_ADDR_L, ALGO_MERGE_FTR_BIT);
	if(FP_OK != ret)
		return ret;

	ret	= fpm_readDeltaC(ftr, 16, 0);
    if( ret != FP_OK)
        return ret;

	for(i = 0; i < 16; i += 4)
	{
	    code = SPI8BitToGD32Bit(ftr+i);
		if(0 != code )
			return FP_MERGE_FTR_ERROR;
	}

	//16-n byte
	ret	= fpm_readDeltaC(ftr, 8, 8);
    if( ret != FP_OK)
        return ret;

    pLen = SPI8BitToGD32Bit(ftr);
    len = SPI8BitToGD32Bit(ftr+4);
    if(pLen - 4 != len)
        return FP_RESULT_ERROR;

	ret = fpm_readDeltaC(ftr, pLen + 8, 8);
    if( ret != FP_OK)
        return ret;
	
	*ftrLen = pLen + 8;

    return fpm_is32BitCheckSumRight(ftr, pLen + 8);
}


