#ifndef __TIMER_H__
#define __TIMER_H__

/*************/
/* constants */
/*************/

#define SYSCLK_HZ       80000000            //80MHz system clock
#define APB2CLK2_HZ     20000000           	//20MHz APB2 time clock, see section 6.2.14
#define APB1CLK1_HZ     80000000            //80MHz APB1 clock
#define MICROSEC_TPS    1000000             //1us
#define MILLISEC_TPS		1000								//1ms
#define PATTERNS_TPS		60									//60 FPS
#define WS2811_PERIOD		400000							//2.5us
#define WS2811_CODE_1		833333							//1.2us
#define WS2811_CODE_0		2000000							//0.5us
#define BIT_23					0x00800000					//bit 23
#define CCMR1_PWM1_MODE (TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2)
#define CCMR1_OFF_MODE 	(TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_2)

/***********************/
/* function prototypes */
/***********************/

extern void timer_init(void);
extern void timer_drive_leds(unsigned int *led_buf, unsigned int led_len);
extern void timer_wait_xms(unsigned int x);
extern unsigned char timer_running(void);

/***********/
/* globals */
/***********/

extern unsigned long    cticks;             //1ms counter

#endif /* __TIMER_H__ */
