#include "LPC11Uxx.h"
#include "sbl.h"
#include "lpc11xx_uart.h"
#include "print.h"
#include "fpm.h"
#include "sbl_master.h"

#define LED_PORT        0           
#define LED             17               
#define LED_MASK       (1 << 17)   

extern void init_spi(void);
extern SBL_RET_CODE SBL_MasterReadFwVersionID(PSBL_FirmVerion_Type pFirmVers);
extern  void delay(uint32_t ms);

uint8_t buf_recv[10] ;

void print_menu(void)
{

	_PRINT_("\r\nSBL Master Demo\r\nMCU: LPC11xx");

    _PRINT_("Commands:");
    _PRINT("    \'r': Read Firmware Version ID\r\n" \
            "    \'u': Upgrade User Application Firmware\r\n"  \
            "    \'s': Upgrade Secondary Boot Loader Firmware\r\n");
}

void StartSendingBlock(uint32_t BlockNum)
{
   _PRINT(".");
}



extern int ml8160_testSPI(uint32_t pollingTimes);

int main(void)
{
	SBL_FirmVerion_Type firm_vers;
	uint8_t ch;
	SBL_RET_CODE ret;
	SBL_CB_Type callbacks;
	
	int i;
	
	SystemInit();  //开启系统时钟
	//SystemCoreClockUpdate();
	
	DebugInit();
	
	// Register Callback
    callbacks.UpgradeStartPFN = StartSendingBlock;
	

	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6) | (1 << 16);   
	LPC_IOCON->PIO0_17 &= ~0x7;
	LPC_IOCON->PIO0_17 |= 0x0;
	
	LPC_GPIO->DIR[LED_PORT] |= (LED_MASK);  
	LPC_GPIO->CLR[LED_PORT] |= LED_MASK;

	init_spi();    //spi1 初始化
	// delay for a while
    for(i = 0; i < 10000; i++);
    // CS LOW
	LPC_IOCON->PIO1_19 &= ~0x7;
	LPC_IOCON->PIO1_19 |= 0x0;	
	LPC_GPIO->DIR[1] |= (1<<19);  	
	LPC_GPIO->SET[1] |= (1<<19);  
	
	while(1)
	{
//		LPC_IOCON->PIO1_19 &= ~0x7;
//		LPC_IOCON->PIO1_19 |= 0x0;	
//		LPC_GPIO->DIR[1] |= (1<<19);  
//	   
//		LPC_GPIO->CLR[1] |= (1<<19);
		
		i = ml8160_testSPI(50);		
		_PRINT_H(i);
		_PRINT("\r\n>");
//        ch = _GET_C;
//        _PRINT_C(ch);
//		_PRINT_(" ");
//		_PRINT_("Read Firmware Version ID...");
//		
//		ret = SBL_MasterReadFwVersionID(&firm_vers);
//		_PRINT_H(ret);
//		if(ret == SBL_OK)
//		{
//			_PRINT_("Firmware Version:");
//			_PRINT("    Major: ");_PRINT_H(firm_vers.Major);
//			_PRINT("\r\n    Minor: ");_PRINT_H(firm_vers.Minor);
//			_PRINT("\r\n    Rev  : ");_PRINT_H(firm_vers.Revision);
//			_PRINT_("");
//		}
		delay(1000);
	}
	
	return 0;

}

void UART_IRQHandler(void)   //串口中断
{
	UART_Receive(buf_recv, 1); //接收串口数据
	
	//UART_Send(buf_recv,1);
}

