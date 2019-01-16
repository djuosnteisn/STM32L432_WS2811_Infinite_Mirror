#include "stm32l432xx.h"
#include "main.h"
#include "my_gpio.h"
#include "timer.h"


void clock_init(void)
{
	/* Main HCLK setup */
	#define PLL_N_VAL 20
	#define PLL_M_VAL 1 //M+1 = 2

	unsigned int temp;

	//set the flash latency to max wait states
	temp = FLASH_ACR_LATENCY_4WS;
	FLASH->ACR |= temp;
	FLASH->ACR &= (temp | ~FLASH_ACR_LATENCY_Msk);
	
	//turn on the HSI16 clock
	RCC->CR |= RCC_CR_HSION;

	/* PLL stuff */
	//make sure PLLs are disabled before modifying them
	//disable PLL by setting PLLON to 0 in RCC_CR
	temp = ~RCC_CR_PLLON;
	RCC->CR &= temp;
	//wait until it's fully stopped, PLLRDY = 0
	while (RCC->CR & RCC_CR_PLLRDY)
		;
	//f(VCO) = f(PLL input) * (PLL N / PLL M)
	//160MHz = 16MHz * (10/1)
	//PLL R = 2 for sysclk of 80MHz
	//see section 6.4.4 on page 198
	//turn off Q and P outputs (I2C and USB)
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLQEN | RCC_PLLCFGR_PLLPEN);
	//set N to 10 and M to 0
	temp = (PLL_N_VAL << RCC_PLLCFGR_PLLN_Pos);
	RCC->PLLCFGR |= temp;
	RCC->PLLCFGR &= (temp | ~RCC_PLLCFGR_PLLN_Msk);
	temp = (PLL_M_VAL << RCC_PLLCFGR_PLLM_Pos);
	RCC->PLLCFGR |= temp;
	RCC->PLLCFGR &= (temp | ~RCC_PLLCFGR_PLLM_Msk);
	//set src to HSI
	temp = RCC_PLLCFGR_PLLSRC_HSI;
	RCC->PLLCFGR |= temp;
	RCC->PLLCFGR &= (temp | ~RCC_PLLCFGR_PLLSRC_Msk);
	//enable our R output (sysclk)
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;
	//re-enable the PLL now
	RCC->CR |= RCC_CR_PLLON;
	
	/* Bus prescalars, AHB and APB1 set to HCLK, no divides */
	RCC->CFGR &= ~(RCC_CFGR_PPRE1 | RCC_CFGR_HPRE);
	/* Use PLL as system clock, and AP2 set to 8MHz */
	RCC->CFGR |= RCC_CFGR_SW_PLL | RCC_CFGR_PPRE2_DIV8;


#ifdef EN_MCO_OUT
	/* Output sysclk to clock out */
	temp = RCC_CFGR_MCOSEL_0;
	temp = (RCC_CFGR_MCOSEL_0 | RCC_CFGR_MCOSEL_2);
	RCC->CFGR |= temp;
	RCC->CFGR &= (temp | ~RCC_CFGR_MCOSEL_Msk);
#endif
}

int main(void)
{
	//init PLL and peripheral clocks
	clock_init();
	//init routines
	gpio_init();
	timer_init();
	//loop forebber
	while (1) ;
}
