#ifndef __GPIO_H__
#define __GPIO_H__

#include "gpio_setup.h"

/**********/
/* macros */
/**********/
/*** Pin Macros ***/
//Port A
////#define PWM_OUTPUT          0x0200  //pin A.9
#define PWM_OUTPUT          0x0001  //pin A.0
#define TEST_OUTPUT					0x0002	//pin A.1
#define BTN_INPUT						0x0008	//pin A.3
#define MCO_OUTPUT					0x0100	//PIN A.8

/*** Utility Macros ***/
#define GPIO_PA_SET(x)      (GPIOA->BSRR = (uint16_t)x)
#define GPIO_PA_CLR(x)      (GPIOA->BSRR = (uint16_t)x << 16)
#define GPIO_PA_STATE(mask) (GPIOA->IDR & (mask))
#define GPIO_PB_SET(x)      (GPIOB->BSRRL = (uint16_t)x)
#define GPIO_PB_CLR(x)      (GPIOB->BSRRH = (uint16_t)x)
#define GPIO_PB_STATE(mask) (GPIOB->IDR & (mask))
#define GPIO_PC_SET(x)      (GPIOC->BSRRL = (uint16_t)x)
#define GPIO_PC_CLR(x)      (GPIOC->BSRRH = (uint16_t)x)
#define GPIO_PC_STATE(mask) (GPIOC->IDR & (mask))

/*** Interrupt Macros ***/
#define DEBOUNCE						30


void gpio_init(void);

/*************/
/* functions */
/*************/
void gpio_init(void);
void gpio_config_pin(PORT port, unsigned int pin, char mode, char otype, char ospeed, char pull, char alt_func);

#endif //__GPIO_H__
