/*
  timer.c

  Implements timer used as timebase for
  execution of periodic program tasks.

  CHANGE LOG
  ==========

*/

#include "stm32l432xx.h"
#include "main.h"
#include "timer.h"
#include "my_gpio.h"
#include "patterns.h"

unsigned long cticks;
static unsigned int s_led_count = 0, s_led_len = 0, s_led_bit_mask;
static const unsigned int *s_led_buf;
static unsigned int timer_get_pw(void);
static void timer_inc_led_vars(void);

/**************************************************
  timer_init

  initialize the 1msec timer
***************************************************/

void timer_init(void)
{
	//setup our pwm pin, PA9, timer 2 ch 1, alt function AF1
	gpio_config_pin(PORT_A, PWM_OUTPUT, MODER_ALT_FUNC, OTYPER_PP, OSPEEDR_100MHZ, PUPDR_OFF, AF1);

	//enable clock for Timer 2
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	//enable interrupt with nvic
	NVIC_EnableIRQ(TIM2_IRQn);
	//configure the timer peripheral
	//our timer period and dutycycle
	TIM2->ARR = (APB1CLK1_HZ / WS2811_PERIOD); //reload value is our full period
	TIM2->CCR1 = (APB1CLK1_HZ / WS2811_PERIOD) / 2; //initialize to 1/2 the total period
	//prescalar not necessary at these speeds
	TIM2->PSC = 0;
	//output pin setup
	TIM2->CCMR1 = CCMR1_PWM1_MODE; //ch 1 = pwm mode 1, up-counting
	TIM2->SR = 0; //clear update event flag
	TIM2->CR1 = TIM_CR1_URS | TIM_CR1_ARPE; //upcounting, edge-aligned, pretty normal operation, preload
	TIM2->DIER = TIM_DIER_UIE; //update interrupt enable (also might want to try CC1IE)
	TIM2->CCER = TIM_CCER_CC1E; //enable capture/compare output ch 1
	TIM2->CNT = 0; //zero the counter
	
	/* SysTick setup */
	SysTick_Config(SYSCLK_HZ / PATTERNS_TPS);
}


/**************************************************
  timer_wait_xms

  (busy) wait x ms
***************************************************/

void timer_wait_xms(unsigned int x)
{
    /* use general-purpose, 16-bit, Timer 16 */
    //enable clock for Timer 16
    RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
    //set prescalar for 1us resolution
    TIM16->PSC = (APB2CLK2_HZ / MILLISEC_TPS);
    //set CCR1 with x
    TIM16->CCR1 = x;
    //create an update event clear counter and load prescalar
    TIM16->EGR |= TIM_EGR_UG;
    //clear any previous match flag
    TIM16->SR &= ~TIM_SR_CC1IF;
    //enable the timer and wait for a match
    TIM16->CR1 = 1;
    while (!(TIM16->SR & TIM_SR_CC1IF))
        ;
    //turn timer off
    TIM16->CR1 &= ~TIM_CR1_CEN;
    //disable clock for Timer 2
    RCC->APB2ENR &= ~RCC_APB2ENR_TIM16EN;
}


/**************************************************
  timer_drive_leds

	Uses TIM 2 Ch 1 for PWM generation.

	This PWM will be used to drive a WS2811 driver LED
	strip.  We will interrupt on each timer match (2.5us),
	and use that opportunity to load the duty cycle for
	the next bit.
	WS2811 waveform is as follows:
	0 code high time: 0.5us
	0 code low time: 2.0us
	1 code high time: 1.2us
	1 code low time: 1.3us
	LED WORD: 24 bits, R7R6...R1R0G7G6...G1G0B7B6...B1B0

	See section 26.4 (page 779)
***************************************************/

void timer_drive_leds(unsigned int *led_buf, unsigned int led_len)
{
	//init various WS2811 variables
	s_led_bit_mask = BIT_23;
	s_led_count = 0;
	s_led_buf = led_buf;
	s_led_len = led_len;
	//prep the timer for the first bit before starting
	TIM2->CCR1 = timer_get_pw();
	TIM2->CCMR1 = CCMR1_PWM1_MODE;
	//increment pointer, bit mask and count
	timer_inc_led_vars();
	//start the timer
////	TIM2->EGR = TIM_EGR_UG; //create an update event
	TIM2->CR1 |= TIM_CR1_CEN; //enable the timer
}


/**************************************************
  TIM2_IRQHandler

	This IRQ is used to vary the dutycycle to encode
	necessary 1's and 0's for the WS2811 interface.
***************************************************/

void TIM2_IRQHandler(void)
{
	if (TIM2->SR & TIM_SR_UIF)
	{
		//clear interrupt
		TIM2->SR &= ~TIM_SR_UIF;
		//see if we're done with our transfers yet
		if (s_led_count == s_led_len)
		{
			//done, force output low
			TIM2->CCMR1 = CCMR1_OFF_MODE;
			//done, turn off the timer
			TIM2->CR1 &= ~TIM_CR1_CEN; //DISable the timer
		}
		else
		{
			//not done, update duty cycle
			TIM2->CCR1 = timer_get_pw();
		}
		//increment pointer, bit mask and count
		timer_inc_led_vars();
	}
}


/**************************************************
  timer_get_pw

  return the pulse width value based off current bit
	in our led stream
***************************************************/

unsigned int timer_get_pw(void)
{
	unsigned int ret, temp;

	//determine pulse width based off current bit
	temp = *s_led_buf;
	if (s_led_bit_mask & temp)
		ret = APB1CLK1_HZ / WS2811_CODE_1;
	else
		ret = APB1CLK1_HZ / WS2811_CODE_0;
	
	return ret;
}


/**************************************************
  timer_inc_led_vars

	Increment various LED variables
***************************************************/

void timer_inc_led_vars(void)
{
		s_led_bit_mask >>= 1;
		if (s_led_bit_mask == 0)
		{
			s_led_bit_mask = BIT_23;
			s_led_buf++;
			s_led_count++;
		}
}


/**************************************************
  timer_running

	return 1 if the timer is currently running,
	0 otherwise
***************************************************/

unsigned char timer_running(void)
{
	if (TIM2->CR1 & TIM_CR1_CEN)
		return 1;

	return 0;
}


/**************************************************
  SysTick_Handler

  increment cticks TPS times per second
***************************************************/

void SysTick_Handler(void)
{
	++cticks;   //60 Hz

	/* Timer Tasks */
	patterns_task();
}
