#include "stm32l432xx.h"
#include "main.h"
#include "gpio.h"



/***********************/
/* function prototypes */
/***********************/

unsigned int get_shift(unsigned int pin, char bits_per_func);

/************************/
/* function definitions */
/************************/

/**************************************************
  gpio_init

  configure all pins
 *************************************************/

void gpio_init(void)
{
	//clock enable for gpio port A
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	//test pin
	gpio_config_pin(PORT_A, TEST_OUTPUT, MODER_OUTPUT, OTYPER_PP, OSPEEDR_100MHZ, PUPDR_OFF, AF0);

#ifdef EN_MCO_OUT
	//MCO
	gpio_config_pin(PORT_A, MCO_OUTPUT, MODER_ALT_FUNC, OTYPER_PP, OSPEEDR_100MHZ, PUPDR_OFF, AF0);
#endif
}


/**************************************************
  gpio_config_pin

  configure a specific pin

  input: port, pin, pin mode, output type, output speed, pull up / down
 *************************************************/
void
gpio_config_pin(PORT port, unsigned int pin, char mode, char otype, char ospeed, char pull, char alt_func)
{
    unsigned int temp;

    switch (port)
    {
        case PORT_A:
            /* configure pin modes, 2 bits per pin */
            temp = 0x3 << get_shift(pin, MODER_NUM_BITS);
            GPIOA->MODER &= ~temp;
            temp = mode << get_shift(pin, MODER_NUM_BITS);
            GPIOA->MODER |= temp;
            /* configure output type, 1 bit per pin */
            temp = 0x1 << get_shift(pin, OTYPER_NUM_BITS);
            GPIOA->OTYPER &= ~temp;
            temp = otype << get_shift(pin, OTYPER_NUM_BITS);
            GPIOA->OTYPER |= temp;
            /* configure speed, 2 bits per pin */
            temp = 0x3 << get_shift(pin, OSPEEDR_NUM_BITS);
            GPIOA->OSPEEDR &= ~temp;
            temp = ospeed << get_shift(pin, OSPEEDR_NUM_BITS);
            GPIOA->OSPEEDR |= temp;
            /* configure pull up/down, 2 bits per pin */
            temp = 0x3 << get_shift(pin, PUPDR_NUM_BITS);
            GPIOA->PUPDR &= ~temp;
            temp = pull << get_shift(pin, PUPDR_NUM_BITS);
            GPIOA->PUPDR |= temp;
            /* configure alternate function */
            if (mode == MODER_ALT_FUNC)
            {
                if (pin <= 0xFF) //use AFRL
                {
                    temp = 0xf << get_shift(pin, AF_NUM_BITS);
                    GPIOA->AFR[0] &= ~temp;
                    temp = alt_func << get_shift(pin, AF_NUM_BITS);
                    GPIOA->AFR[0] |= temp;
                }
                else //use AFRH
                {
                    temp = 0xf << get_shift(pin >> AFRH_SHIFT, AF_NUM_BITS);
                    GPIOA->AFR[1] &= ~temp;
                    temp = alt_func << get_shift(pin >> AFRH_SHIFT, AF_NUM_BITS);
                    GPIOA->AFR[1] |= temp;
                }
            }
            break;
        case PORT_B:
            /* configure pin modes, 2 bits per pin */
            temp = 0x3 << get_shift(pin, MODER_NUM_BITS);
            GPIOB->MODER &= ~temp;
            temp = mode << get_shift(pin, MODER_NUM_BITS);
            GPIOB->MODER |= temp;
            /* configure output type, 1 bit per pin */
            temp = 0x1 << get_shift(pin, OTYPER_NUM_BITS);
            GPIOB->OTYPER &= ~temp;
            temp = otype << get_shift(pin, OTYPER_NUM_BITS);
            GPIOB->OTYPER |= temp;
            /* configure speed, 2 bits per pin */
            temp = 0x3 << get_shift(pin, OSPEEDR_NUM_BITS);
            GPIOB->OSPEEDR &= ~temp;
            temp = ospeed << get_shift(pin, OSPEEDR_NUM_BITS);
            GPIOB->OSPEEDR |= temp;
            /* configure pull up/down, 2 bits per pin */
            temp = 0x3 << get_shift(pin, PUPDR_NUM_BITS);
            GPIOB->PUPDR &= ~temp;
            temp = pull << get_shift(pin, PUPDR_NUM_BITS);
            GPIOB->PUPDR |= temp;
            /* configure alternate function */
            if (mode == MODER_ALT_FUNC)
            {
                if (pin <= 0xFF) //use AFRL
                {
                    temp = 0xf << get_shift(pin, AF_NUM_BITS);
                    GPIOB->AFR[0] &= ~temp;
                    temp = alt_func << get_shift(pin, AF_NUM_BITS);
                    GPIOB->AFR[0] |= temp;
                }
                else //use AFRH
                {
                    temp = 0xf << get_shift(pin >> AFRH_SHIFT, AF_NUM_BITS);
                    GPIOB->AFR[1] &= ~temp;
                    temp = alt_func << get_shift(pin >> AFRH_SHIFT, AF_NUM_BITS);
                    GPIOB->AFR[1] |= temp;
                }
            }
            break;
        case PORT_C:
            /* configure pin modes, 2 bits per pin */
            temp = 0x3 << get_shift(pin, MODER_NUM_BITS);
            GPIOC->MODER &= ~temp;
            temp = mode << get_shift(pin, MODER_NUM_BITS);
            GPIOC->MODER |= temp;
            /* configure output type, 1 bit per pin */
            temp = 0x1 << get_shift(pin, OTYPER_NUM_BITS);
            GPIOC->OTYPER &= ~temp;
            temp = otype << get_shift(pin, OTYPER_NUM_BITS);
            GPIOC->OTYPER |= temp;
            /* configure speed, 2 bits per pin */
            temp = 0x3 << get_shift(pin, OSPEEDR_NUM_BITS);
            GPIOC->OSPEEDR &= ~temp;
            temp = ospeed << get_shift(pin, OSPEEDR_NUM_BITS);
            GPIOC->OSPEEDR |= temp;
            /* configure pull up/down, 2 bits per pin */
            temp = 0x3 << get_shift(pin, PUPDR_NUM_BITS);
            GPIOC->PUPDR &= ~temp;
            temp = pull << get_shift(pin, PUPDR_NUM_BITS);
            GPIOC->PUPDR |= temp;
            /* configure alternate function */
            if (mode == MODER_ALT_FUNC)
            {
                if (pin <= 0xFF) //use AFRL
                {
                    temp = 0xf << get_shift(pin, AF_NUM_BITS);
                    GPIOC->AFR[0] &= ~temp;
                    temp = alt_func << get_shift(pin, AF_NUM_BITS);
                    GPIOC->AFR[0] |= temp;
                }
                else //use AFRH
                {
                    temp = 0xf << get_shift(pin >> AFRH_SHIFT, AF_NUM_BITS);
                    GPIOC->AFR[1] &= ~temp;
                    temp = alt_func << get_shift(pin >> AFRH_SHIFT, AF_NUM_BITS);
                    GPIOC->AFR[1] |= temp;
                }
            }
            break;
        default:
            break;
    }
}

/**************************************************
  get_shift

  get the amount to shift a pin setup parameter, based
  off the pin value and number of bits per parameter val

  input: pin value, bits per parameter valu
 *************************************************/
unsigned int
get_shift(unsigned int pin, char bits_per_func)
{
    char i;

    if (!pin)
        return 0;

    for (i = 0; i < 32; i++)
    {
        if (pin & 0x1)
            break;
        pin >>= 1;
    }

    return i * bits_per_func;
}

