#ifndef __GPIO_SETUP_H__
#define __GPIO_SETUP_H__

/*** PORT enum ***/
typedef enum tagPort
{
    PORT_A,
    PORT_B,
    PORT_C,
} PORT;
/*** MODER Macros ***/
#define MODER_INPUT         0x0
#define MODER_OUTPUT        0x1
#define MODER_ALT_FUNC      0x2
#define MODER_ANA           0x3
#define MODER_NUM_BITS      2
/*** OTYPER Macros ***/
#define OTYPER_PP           0x0
#define OTYPER_OD           0x1
#define OTYPER_NUM_BITS     1
/*** OSPEEDR Macros ***/
#define OSPEEDR_2MHZ        0x0
#define OSPEEDR_25MHZ       0x1
#define OSPEEDR_50MHZ       0x2
#define OSPEEDR_100MHZ      0x3
#define OSPEEDR_NUM_BITS    2
/*** PUPDR Macros ***/
#define PUPDR_OFF           0x0
#define PUPDR_UP            0x1
#define PUPDR_DN            0x2
#define PUPDR_NUM_BITS      2
/*** Alt Func ***/
#define AF0                 0x0
#define AF1                 0x1
#define AF2                 0x2
#define AF3                 0x3
#define AF4                 0x4
#define AF5                 0x5
#define AF6                 0x6
#define AF7                 0x7
#define AF8                 0x8
#define AF9                 0x9
#define AF10                0xa
#define AF11                0xb
#define AF12                0xc
#define AF13                0xd
#define AF14                0xe
#define AF15                0xf
#define AF_NUM_BITS         4
#define AFRH_SHIFT          8

#endif
