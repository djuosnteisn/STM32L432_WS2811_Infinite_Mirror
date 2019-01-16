#include "stm32l432xx.h"
#include "main.h"
#include "patterns.h"
#include "my_gpio.h"
#include "timer.h"

/* macros */
#define NUM_LEDS 									28
#define GAME_START_INDEX					25
#define FLASH_START_DONE_COUNT		7
#define FAIL_DONE_COUNT						12
#define GAME_TIMEOUT							(20 * 30)

/* consts */
////const unsigned char led_incr_rate = 2; //30 Hz
const unsigned char led_incr_rate = 3; //20 Hz
////const unsigned char led_incr_rate = 4; //15 Hz
const unsigned char RATE_1_4_S = 15; //1/4 second
const unsigned char RATE_HALF_S = 30; //1/2 second
const unsigned char RATE_3_4_S = 45; //3/4 second
const unsigned char RATE_1_S = 60; //1 second

/* variables */
//consts
//statics
static PATTERN_STATES pattern_state = IDLE;
static PATTERN_STATES next_state;
static unsigned int led_pattern[NUM_LEDS];
static unsigned char game_index = 0, goal_index_1 = 10, goal_index_2 = 12;

/* functions */
static void patterns_set_led_color(unsigned int led_index, unsigned int color_24_b);
static void patterns_clr_led_color(unsigned int led_index, unsigned int color_24_b);
static void patterns_flash_all(unsigned int color_24_b);

/**************************************************
  patterns_task

	Gets here ever 60 times per second.
  Run various pattern tasks
***************************************************/
void patterns_task(void)
{
	unsigned char i, temp_g, temp_r, temp_b;
	static unsigned char r_index = 0, g_index = 5, b_index = 9;
	static signed char r_dir = 1, g_dir = 1, b_dir = -1;
	static unsigned char flash_start_ctr = 0, idle_ctr = 0, fail_ctr = 0, success_ctr = 0;
	static unsigned short game_ctr = 0;

	switch (pattern_state)
	{
		/* IDLE */
		case IDLE:
			if (!(cticks % RATE_1_4_S))
			{
				if (idle_ctr == 0)
				{
					patterns_flash_all(BRIGHT_ORANGE);
					idle_ctr = 1;
				}
				else
				{
					for (i = 0; i < NUM_LEDS; i++)
					{
						temp_r = (led_pattern[i] & RED_MASK) >> RED_SHIFT;
						temp_g = (led_pattern[i] & GREEN_MASK) >> GREEN_SHIFT;
						if (idle_ctr <= 16)
							temp_g--;
						else
							temp_g++;
						led_pattern[i] = (temp_g << GREEN_SHIFT) | (temp_r << RED_SHIFT);
					}
					++idle_ctr;
					if (idle_ctr > 32)
						idle_ctr = 1;
				}
			}
			break;
		/* FLASH START */
		case FLASH_START:
			if (!(cticks % RATE_HALF_S))
			{
				if (flash_start_ctr % 2)
				{
					if (flash_start_ctr < 5)
						patterns_flash_all(YELLOW);
					else
						patterns_flash_all(BRIGHT_GREEN);
				}
				else
					patterns_flash_all(0);
				++flash_start_ctr;
				
				if (flash_start_ctr == FLASH_START_DONE_COUNT)
				{
					flash_start_ctr = 0;
					game_index = GAME_START_INDEX;
					game_ctr = 0;
					pattern_state = CHASE_GAME;
				}
			}
			break;
		/* FLASH_SUCCESS */
		case FLASH_SUCCESS:
			if (!(cticks % RATE_1_4_S))
			{
				if (success_ctr < FAIL_DONE_COUNT - 4)
				{
					if (success_ctr % 2)
					{
						patterns_flash_all(BRIGHT_WHITE);
					}
					else
					{
						patterns_flash_all(0);
					}
				}
				else if (success_ctr == FAIL_DONE_COUNT - 4)
				{
					next_state = FLASH_SUCCESS;
					pattern_state = FADE_OUT_SLOW;
				}
				else if (success_ctr == FAIL_DONE_COUNT)
				{
					success_ctr = 0;
					pattern_state = IDLE;
				}
				++success_ctr;
			}
			break;
		/* FLASH_FAIL */
		case FLASH_FAIL:
			if (!(cticks % RATE_1_4_S))
			{
				if (fail_ctr < FAIL_DONE_COUNT - 4)
				{
					if (fail_ctr % 2)
					{
						patterns_flash_all(BRIGHT_RED);
					}
					else
					{
						patterns_flash_all(0);
					}
				}
				else if (fail_ctr == FAIL_DONE_COUNT - 4)
				{
					next_state = FLASH_FAIL;
					pattern_state = FADE_OUT_SLOW;
				}
				else if (fail_ctr == FAIL_DONE_COUNT)
				{
					fail_ctr = 0;
					pattern_state = IDLE;
				}
				++fail_ctr;
			}
			break;
		/* CHASE_GAME */
		case CHASE_GAME:
			if (!(cticks % led_incr_rate))
			{
				//clear old pattern
				patterns_clr_led_color(game_index, BRIGHT_GREEN);
				//increment index
				game_index--;
				if (game_index >= NUM_LEDS) //unsigned rollover
					game_index = NUM_LEDS - 1;
				//draw new pattern
				patterns_set_led_color(game_index, BRIGHT_GREEN);
				//redraw white leds
				patterns_set_led_color(goal_index_1, WHITE);
				patterns_set_led_color(goal_index_2, WHITE);
				++game_ctr;
				if (game_ctr >= GAME_TIMEOUT)
				{
					pattern_state = FLASH_FAIL;
				}
			}
			break;
		/* GAME_STOPPED */
		case GAME_STOPPED:
			//determine if the game won or lost
			//clear the old pattern regardless
			patterns_clr_led_color(game_index, BRIGHT_GREEN);
			if (game_index > goal_index_1 && game_index < goal_index_2)
			{
				//it's a win! :)
				pattern_state = FLASH_SUCCESS;
			}
			else
			{
				//it's a loss :(
				pattern_state = FLASH_FAIL;
			}
			break;
		/* CHASE_3_COLOR */
		case CHASE_3_COLOR: //walk an led back and forth
			if (!(cticks % led_incr_rate))
			{
				//clear old patterns
				patterns_clr_led_color(r_index, BRIGHT_RED);
				patterns_clr_led_color(g_index, BRIGHT_GREEN);
				patterns_clr_led_color(b_index, BRIGHT_BLUE);
				//increment indexes
				r_index += r_dir;
				if (r_index >= NUM_LEDS)
				{
					r_index = NUM_LEDS - 1;
					r_dir = -1;
				}
				else if (r_index <= 0)
				{
					r_index = 0;
					r_dir = 1;
				}
				g_index += g_dir;
				if (g_index >= NUM_LEDS && g_dir)
				{
					g_index = NUM_LEDS - 1;
					g_dir = -1;
				}
				else if (g_index <= 0)
				{
					g_index = 0;
					g_dir = 1;
				}
				b_index += b_dir;
				if (b_index >= NUM_LEDS)
				{
					b_index = NUM_LEDS - 1;
					b_dir = -1;
				}
				else if (b_index <= 0)
				{
					b_index = 0;
					b_dir = 1;
				}
				//draw new pattern
				patterns_set_led_color(r_index, BRIGHT_RED);
				patterns_set_led_color(g_index, BRIGHT_GREEN);
				patterns_set_led_color(b_index, BRIGHT_BLUE);
			}
			break;
			/* FADE OUT */
		case FADE_OUT:
			for (i = 0; i < NUM_LEDS; i++)
			{
				temp_g = (led_pattern[i] & GREEN_MASK) >> GREEN_SHIFT;
				temp_r = (led_pattern[i] & RED_MASK) >> RED_SHIFT;
				temp_b = (led_pattern[i] & BLUE_MASK) >> BLUE_SHIFT;
				if (temp_g > 0x80) temp_g -= 0x10;
				else if (temp_g > 0x40) temp_g -= 0x04;
				else if (temp_g > 0x20) temp_g -= 0x02;
				else if (temp_g > 0) temp_g -= 0x01;
				if (temp_r > 0x80) temp_r -= 0x10;
				else if (temp_r > 0x40) temp_r -= 0x04;
				else if (temp_r > 0x20) temp_r -= 0x02;
				else if (temp_r > 0) temp_r -= 0x01;
				if (temp_b > 0x80) temp_b -= 0x10;
				else if (temp_b > 0x40) temp_b -= 0x04;
				else if (temp_b > 0x20) temp_b -= 0x02;
				else if (temp_b > 0) temp_b -= 0x01;
				led_pattern[i] = (temp_g << GREEN_SHIFT) | (temp_r << RED_SHIFT) | (temp_b << BLUE_SHIFT);
			}
			//see if all the LEDs are off
			for (i = 0; i < NUM_LEDS; i++)
			{
				if (led_pattern[i])
					break;
			}
			if (i == NUM_LEDS)
			{	
				idle_ctr = 0;
				pattern_state = next_state;
			}
			break;
			/* FADE OUT FAST */
		case FADE_OUT_FAST:
			for (i = 0; i < NUM_LEDS; i++)
			{
				temp_g = (led_pattern[i] & GREEN_MASK) >> GREEN_SHIFT;
				temp_r = (led_pattern[i] & RED_MASK) >> RED_SHIFT;
				temp_b = (led_pattern[i] & BLUE_MASK) >> BLUE_SHIFT;
				if (temp_g > 0x80) temp_g -= 0x20;
				else if (temp_g > 0x40) temp_g -= 0x08;
				else if (temp_g > 0x20) temp_g -= 0x04;
				else if (temp_g > 0) temp_g -= 0x01;
				if (temp_r > 0x80) temp_r -= 0x20;
				else if (temp_r > 0x40) temp_r -= 0x08;
				else if (temp_r > 0x20) temp_r -= 0x04;
				else if (temp_r > 0) temp_r -= 0x01;
				if (temp_b > 0x80) temp_b -= 0x20;
				else if (temp_b > 0x40) temp_b -= 0x08;
				else if (temp_b > 0x20) temp_b -= 0x04;
				else if (temp_b > 0) temp_b -= 0x01;
				led_pattern[i] = (temp_g << GREEN_SHIFT) | (temp_r << RED_SHIFT) | (temp_b << BLUE_SHIFT);
			}
			//see if all the LEDs are off
			for (i = 0; i < NUM_LEDS; i++)
			{
				if (led_pattern[i])
					break;
			}
			if (i == NUM_LEDS)
			{	
				idle_ctr = 0;
				pattern_state = next_state;
			}
			break;
			/* FADE_OUT_SLOW */
		case FADE_OUT_SLOW:
			for (i = 0; i < NUM_LEDS; i++)
			{
				temp_g = (led_pattern[i] & GREEN_MASK) >> GREEN_SHIFT;
				temp_r = (led_pattern[i] & RED_MASK) >> RED_SHIFT;
				temp_b = (led_pattern[i] & BLUE_MASK) >> BLUE_SHIFT;
				if (temp_g > 0x80) temp_g -= 0x08;
				else if (temp_g > 0x40) temp_g -= 0x02;
				else if (temp_g > 0x20) temp_g -= 0x01;
				else if (temp_g > 0) temp_g -= 0x01;
				if (temp_r > 0x80) temp_r -= 0x08;
				else if (temp_r > 0x40) temp_r -= 0x02;
				else if (temp_r > 0x20) temp_r -= 0x01;
				else if (temp_r > 0) temp_r -= 0x01;
				if (temp_b > 0x80) temp_b -= 0x08;
				else if (temp_b > 0x40) temp_b -= 0x02;
				else if (temp_b > 0x20) temp_b -= 0x01;
				else if (temp_b > 0) temp_b -= 0x01;
				led_pattern[i] = (temp_g << GREEN_SHIFT) | (temp_r << RED_SHIFT) | (temp_b << BLUE_SHIFT);
			}
			//see if all the LEDs are off
			for (i = 0; i < NUM_LEDS; i++)
			{
				if (led_pattern[i])
					break;
			}
			if (i == NUM_LEDS)
			{	
				idle_ctr = 0;
				pattern_state = next_state;
			}
			break;
	}

	if (!timer_running())
	{
		timer_drive_leds(led_pattern, NUM_LEDS);
	}
}

/**************************************************
  patterns_set_led_color

	Or val with the current led value at index.  val
  is a single byte, intended for individual R, G, and B
	access.
***************************************************/
static void patterns_set_led_color(unsigned int index, unsigned int color_24_b)
{
	led_pattern[index] |= color_24_b;
}

/**************************************************
  patterns_clr_led_color

  AND the inverse of val with the current led value at
  index.  val is a single byte, intended for individual
  R, G, and B access
***************************************************/
static void patterns_clr_led_color(unsigned int led_index, unsigned int color_24_b)
{
	led_pattern[led_index] &= ~color_24_b;
}

/**************************************************
  patterns_flash_all

	Set all LEDs to the 24-bit color color_24_b
***************************************************/
static void patterns_flash_all(unsigned int color_24_b)
{
	unsigned int i;
	
	for (i = 0; i < NUM_LEDS; i++)
		led_pattern[i] = color_24_b;
}

/**************************************************
  patterns_set_state

	Set the current state for the pattern task
***************************************************/
void patterns_set_state(PATTERN_STATES state)
{
	pattern_state = state;
}

/**************************************************
  patterns_get_state

	Return the current state for the pattern task
***************************************************/
PATTERN_STATES patterns_get_state(void)
{
	return pattern_state;
}

/**************************************************
  patterns_set_next_state

	Set the current state for the pattern task
***************************************************/
void patterns_set_next_state(PATTERN_STATES state)
{
	next_state = state;
}

/**************************************************
  patterns_get_next_state

	Return the current state for the pattern task
***************************************************/
PATTERN_STATES patterns_get_next_state(void)
{
	return next_state;
}
