#ifndef __PATTERNS_H__
#define __PATTERNS_H__

//macros
//LED byte color shifts
#define GREEN_SHIFT	16
#define RED_SHIFT		8
#define BLUE_SHIFT 	0
//color masks
#define GREEN_MASK		0x00FF0000
#define RED_MASK			0x0000FF00
#define BLUE_MASK			0x000000FF
//24-bit colors
#define LIGHT_WHITE						(1 << GREEN_SHIFT | 1 << RED_SHIFT | 1 << BLUE_SHIFT)
#define WHITE 								(63 << GREEN_SHIFT | 63 << RED_SHIFT | 63 << BLUE_SHIFT)
#define BRIGHT_WHITE 					(127 << GREEN_SHIFT | 127 << RED_SHIFT | 127 << BLUE_SHIFT)
#define LIGHT_GREEN						(1 << GREEN_SHIFT)
#define GREEN									(63 << GREEN_SHIFT)
#define BRIGHT_GREEN					(127 << GREEN_SHIFT)
#define LIGHT_RED 						(1 << RED_SHIFT)
#define RED										(63 << RED_SHIFT)
#define BRIGHT_RED						(127 << RED_SHIFT)
#define LIGHT_BLUE						(1 << BLUE_SHIFT)
#define BLUE									(63 << BLUE_SHIFT)
#define BRIGHT_BLUE						(127 << BLUE_SHIFT)
#define YELLOW								(RED | 24 << GREEN_SHIFT)
#define YELLOW_ORANGE					(RED | 16 << GREEN_SHIFT)
#define BRIGHT_YELLOW_ORANGE	(BRIGHT_RED | 32 << GREEN_SHIFT)
#define ORANGE								(RED | 8 << GREEN_SHIFT)
#define BRIGHT_ORANGE					(BRIGHT_RED | 16 << GREEN_SHIFT)
#define RED_ORANGE						(RED | LIGHT_GREEN)
#define BRIGHT_RED_ORANGE 		(BRIGHT_RED | 2 << GREEN_SHIFT)

//types
typedef enum
{
	IDLE,
	FLASH_START,
	FLASH_SUCCESS,
	FLASH_FAIL,
	CHASE_GAME,
	CHASE_3_COLOR,
	GAME_STOPPED,
	FADE_OUT,
	FADE_OUT_FAST,
	FADE_OUT_SLOW,
} PATTERN_STATES;

typedef enum
{
	FAIL_INIT,
	FAIL_DRIP,
	SUCCESS_INIT,
} SUBSTATE_STATES;


extern void patterns_task(void);
extern void patterns_set_state(PATTERN_STATES state);
extern PATTERN_STATES patterns_get_state(void);
extern void patterns_set_next_state(PATTERN_STATES next_state);
extern PATTERN_STATES patterns_get_next_state(void);

#endif // __PATTERNS_H__
