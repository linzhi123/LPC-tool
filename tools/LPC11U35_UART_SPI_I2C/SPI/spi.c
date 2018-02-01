#include "LPC11Uxx.h"
#include "lpc11xx_ssp.h"

/*********************************************************************//**
*!< Macro defines for I/O configuration registers
***********************************************************************/
#define PINSEL_PIO_FUNC(n)		(n&0x07)		/**< Pin function selected */
#define PINSEL_PIO_MODE(n)		((n&0x03)<<3)	/**< Mode function selected */
#define PINSEL_PIO_HYS			(1<<5)			/**< Hysteresis */
#define PINSEL_PIO_DMODE		(1<<7)			/**< Digital function mode, applied for pin that has ADC functionality */
#define PINSEL_PIO_MASK			(0x3FF)			/**< IOCON_PIOn mask value */


#define SBL_CLKPWR_PCONP_PCSSP                (1<<8)
#define SSP_CPHA_USED                         (SSP_CPHA_SECOND)
#define SSP_CPOL_USED                         (SSP_CPOL_LO)
#define SSP_CLK_RATE_USED                     (1000000)
#define SSP_DATA_BIT_USED                     (SSP_DATABIT_8)
#define SSP_DATA_WORD                         (0)
#define SSP_MODE_USED                         (SSP_MASTER_MODE)

#define SYSCTL_IRC_FREQ (12000000)

#define CRYSTAL_MAIN_FREQ_IN (12000000)

typedef enum CHIP_SYSCTL_MAIN_CLK {
	SYSCTL_MAINCLKSRC_IRC = 0,		/*!< Internal oscillator */
	SYSCTL_MAINCLKSRC_PLLIN,		/*!< System PLL input */
	SYSCTL_MAINCLKSRC_LFOSC,		/*!< LF oscillator rate (11Axx only) */
	SYSCTL_MAINCLKSRC_WDTOSC = SYSCTL_MAINCLKSRC_LFOSC,	/*!< Watchdog oscillator rate */
	SYSCTL_MAINCLKSRC_PLLOUT,		/*!< System PLL output */
} CHIP_SYSCTL_MAIN_CLK_T;

typedef enum CHIP_WDTLFO_OSC {
	WDTLFO_OSC_ILLEGAL,
	WDTLFO_OSC_0_60,	/*!< 0.6 MHz watchdog/LFO rate */
	WDTLFO_OSC_1_05,	/*!< 1.05 MHz watchdog/LFO rate */
	WDTLFO_OSC_1_40,	/*!< 1.4 MHz watchdog/LFO rate */
	WDTLFO_OSC_1_75,	/*!< 1.75 MHz watchdog/LFO rate */
	WDTLFO_OSC_2_10,	/*!< 2.1 MHz watchdog/LFO rate */
	WDTLFO_OSC_2_40,	/*!< 2.4 MHz watchdog/LFO rate */
	WDTLFO_OSC_2_70,	/*!< 2.7 MHz watchdog/LFO rate */
	WDTLFO_OSC_3_00,	/*!< 3.0 MHz watchdog/LFO rate */
	WDTLFO_OSC_3_25,	/*!< 3.25 MHz watchdog/LFO rate */
	WDTLFO_OSC_3_50,	/*!< 3.5 MHz watchdog/LFO rate */
	WDTLFO_OSC_3_75,	/*!< 3.75 MHz watchdog/LFO rate */
	WDTLFO_OSC_4_00,	/*!< 4.0 MHz watchdog/LFO rate */
	WDTLFO_OSC_4_20,	/*!< 4.2 MHz watchdog/LFO rate */
	WDTLFO_OSC_4_40,	/*!< 4.4 MHz watchdog/LFO rate */
	WDTLFO_OSC_4_60		/*!< 4.6 MHz watchdog/LFO rate */
} CHIP_WDTLFO_OSC_T;


typedef enum CHIP_SYSCTL_PLL_SRC {
	SYSCTL_PLLSRC_IRC = 0,		/*!< Internal oscillator in */
	SYSCTL_PLLSRC_MAINOSC,		/*!< Crystal (main) oscillator in */
#if defined(CHIP_LPC11AXX)
	SYSCTL_PLLSRC_EXT_CLKIN,	/*!< External clock in (11Axx only) */
#else
	SYSCTL_PLLSRC_RESERVED1,	/*!< Reserved */
#endif
	SYSCTL_PLLSRC_RESERVED2,	/*!< Reserved */
} CHIP_SYSCTL_PLL_SRC_T;


/* Inprecise clock rates for the watchdog oscillator */
static const uint32_t wdtOSCRate[WDTLFO_OSC_4_60 + 1] = {
	0,					/* WDT_OSC_ILLEGAL */
	600000,				/* WDT_OSC_0_60 */
	1050000,			/* WDT_OSC_1_05 */
	1400000,			/* WDT_OSC_1_40 */
	1750000,			/* WDT_OSC_1_75 */
	2100000,			/* WDT_OSC_2_10 */
	2400000,			/* WDT_OSC_2_40 */
	2700000,			/* WDT_OSC_2_70 */
	3000000,			/* WDT_OSC_3_00 */
	3250000,			/* WDT_OSC_3_25 */
	3500000,			/* WDT_OSC_3_50 */
	3750000,			/* WDT_OSC_3_75 */
	4000000,			/* WDT_OSC_4_00 */
	4200000,			/* WDT_OSC_4_20 */
	4400000,			/* WDT_OSC_4_40 */
	4600000				/* WDT_OSC_4_60 */
};


extern void delay(uint32_t ms);

static uint32_t Chip_Clock_GetWDTLFORate(uint32_t reg)
{
	uint32_t div;
	CHIP_WDTLFO_OSC_T clk;

	/* Get WDT oscillator settings */
	clk = (CHIP_WDTLFO_OSC_T) ((reg >> 5) & 0xF);
	div = reg & 0x1F;

	/* Compute clock rate and divided by divde value */
	return wdtOSCRate[clk] / ((div + 1) << 1);
}

static inline uint32_t Chip_SYSCTL_GetIntOscRate(void)
{
	return SYSCTL_IRC_FREQ;
}

static inline uint32_t Chip_SYSCTL_GetMainOscRate(void)
{
	return CRYSTAL_MAIN_FREQ_IN;
}

/* Return estimated watchdog oscillator rate */
uint32_t Chip_Clock_GetWDTOSCRate(void)
{
	return Chip_Clock_GetWDTLFORate(LPC_SYSCON->WDTOSCCTRL);
}

/* Return System PLL input clock rate */
uint32_t Chip_Clock_GetSystemPllInClockRate(void)
{
	uint32_t clkRate;

	switch ((CHIP_SYSCTL_PLL_SRC_T) (LPC_SYSCON->SYSPLLCLKSEL & 0x3)) {
	case SYSCTL_PLLSRC_IRC:
		clkRate = Chip_SYSCTL_GetIntOscRate();
		break;

	case SYSCTL_PLLSRC_MAINOSC:
		clkRate = Chip_SYSCTL_GetMainOscRate();
		break;

	default:
		clkRate = 0;
	}

	return clkRate;
}

static uint32_t Chip_Clock_GetPllFreq(uint32_t PllReg, uint32_t inputRate)
{
	uint32_t msel = ((PllReg & 0x1F) + 1);

	return inputRate * msel;
}

/* Return System PLL output clock rate */
uint32_t Chip_Clock_GetSystemPllOutClockRate(void)
{
	return Chip_Clock_GetPllFreq(LPC_SYSCON->SYSPLLCTRL,
								 Chip_Clock_GetSystemPllInClockRate());
}

/* Return main clock rate */
uint32_t Chip_Clock_GetMainClockRate(void)
{
	uint32_t clkRate = 0;

	switch ((CHIP_SYSCTL_MAIN_CLK_T) (LPC_SYSCON->MAINCLKSEL & 0x3)) {
	case SYSCTL_MAINCLKSRC_IRC:
		clkRate = Chip_SYSCTL_GetIntOscRate();
		break;

	case SYSCTL_MAINCLKSRC_PLLIN:
		clkRate = Chip_Clock_GetSystemPllInClockRate();
		break;

#if defined(CHIP_LPC11AXX)
	case SYSCTL_MAINCLKSRC_LFOSC:
		clkRate = Chip_Clock_GetLFOOSCRate();
		break;

#else
	case SYSCTL_MAINCLKSRC_WDTOSC:
		clkRate = Chip_Clock_GetWDTOSCRate();
		break;
#endif

	case SYSCTL_MAINCLKSRC_PLLOUT:
		clkRate = Chip_Clock_GetSystemPllOutClockRate();
		break;
	}

	return clkRate;
}

static inline uint32_t Chip_Clock_GetSSP1ClockDiv(void)
{
	return LPC_SYSCON->SSP1CLKDIV;
}


void IP_SSP_Set_ClockRate(uint32_t clk_rate, uint32_t prescale)
{
	uint32_t temp;
	temp = LPC_SSP1->CR0 & (~(SSP_CR0_SCR(0xFF)));
	LPC_SSP1->CR0 = temp | (SSP_CR0_SCR(clk_rate));
	LPC_SSP1->CPSR = prescale;
}

/* Set the clock frequency for SSP interface */
void Chip_SSP_Set_BitRate(uint32_t bit_rate)
{
	uint32_t main_clk, ssp_div;
	uint32_t ssp_clk, cr0_div, cmp_clk, prescale;

	main_clk = Chip_Clock_GetMainClockRate();

	ssp_div = Chip_Clock_GetSSP1ClockDiv();

	ssp_clk = main_clk / ssp_div;

	cr0_div = 0;
	cmp_clk = 0xFFFFFFFF;
	prescale = 2;

	while (cmp_clk > bit_rate) {
		cmp_clk = ssp_clk / ((cr0_div + 1) * prescale);
		if (cmp_clk > bit_rate) {
			cr0_div++;
			if (cr0_div > 0xFF) {
				cr0_div = 0;
				prescale += 2;
			}
		}
	}

	IP_SSP_Set_ClockRate(cr0_div, prescale);
}

void init_spi(void)
{
	uint32_t tmp;	

	// initialize pinsel for SSP1
	/* MISO1: P1.22*/
	LPC_IOCON->PIO0_22 &= ~(0x1F); //配置PIO0_21为MISO1	
	LPC_IOCON->PIO0_22 |= PINSEL_PIO_FUNC(3) | PINSEL_PIO_MODE(2);
	
	/* MOSI1: P1.21 */
	LPC_IOCON->PIO0_21 &= ~(0x1F); //配置PIO0_21为MOSI1
	LPC_IOCON->PIO0_21 |= PINSEL_PIO_FUNC(2) | PINSEL_PIO_MODE(2);

	/* P1.15 function 2 is SSP clock, need to combined with IOCONSCKLOC register setting */
	LPC_IOCON->PIO1_15 &= ~(0x1F); //配置PIO1_15为sck1
	LPC_IOCON->PIO1_15 |= PINSEL_PIO_FUNC(3) | PINSEL_PIO_MODE(2);

	/* SSP SSEL */
	LPC_IOCON->PIO1_19 &= ~0x7;
	LPC_IOCON->PIO1_19 |= 0x0;	
	LPC_GPIO->DIR[1] |= (1<<19);  	
	LPC_GPIO->SET[1] |= (1<<19);  

	//1.l
	LPC_SYSCON->SYSAHBCLKCTRL |= 1<<18; //开spi1时钟
	//1.2
	LPC_SYSCON->SSP1CLKDIV = 0x1;  //spi1分频器
	//1.3
	LPC_SYSCON->PRESETCTRL |= 1<<2;  //重置信号
	
	LPC_SSP1->CR1 = (LPC_SSP1->CR1 & ~(1 << 2)) | (0<<2);  //Master mod
	
	LPC_SSP1->CR0 = (LPC_SSP1->CR0 & ~0xFF) | (0x7<<0) | (0<<4) | (0<<6) | (1<<7); //8 data bit FRF->SPI CPOL CPHA
	
	Chip_SSP_Set_BitRate(100000);  //set SPI rate 100KHZ
	
    LPC_SSP1->CR1 = (1<<1);	  //SPI Enable
}












