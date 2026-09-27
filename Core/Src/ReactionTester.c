/**
  ******************************************************************************
  * @file           : ReactionTester.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 BYU-Idaho
  * All rights reserved.
  *
  ******************************************************************************
  * @copyright  BYU-Idaho
  * @date       2023
  * @version    F23
  * @note       For course ECEN-361
  * @author     Lynn Watson
  ******************************************************************************
  */
/* This is for Part-3 of Lab-02, ECEN-361
 * Student to only change parts between the comment blocks:
	  ***** STUDENT TO FILL IN START
 *
 */

#include "main.h"
#include "stm32l4xx_it.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "MultiFunctionShield.h"
extern TIM_HandleTypeDef htim3;  // Points to the timer structure   Timer3 is the Reaction Timer
extern void MX_TIM3_Init(void);	// To reset the timer
extern bool got_start_button;
extern bool got_stop_button;
extern bool got_fastest_button;
extern int best_reaction_time_in_millisec;


// Globals
#define upper_limit_millisec_to_wait  7000;  //Give the user up to 7 seconds to wonder

int rand_millisec;
int last_reaction_time_in_millisec = 0;
bool started_doing_reaction_timers = false;

// Extra credit: cheat detection and flashing the result
#define flash_period_millisec  500	// Result is on for this long, then off for this long

bool cheated = false;				// Stop was pushed before the "GO" lights came on
bool flashing_result = false;		// Flash the last result until Start is pushed again
bool result_showing = true;
uint32_t last_flash_tick = 0;
static void start_flashing_result(void);

void show_a_random_number()
	{
	if (!started_doing_reaction_timers)
		{
		rand_millisec =  rand() % upper_limit_millisec_to_wait;
		MultiFunctionShield_Display(rand_millisec);
		HAL_Delay(2000);  // this is how long before the counter on the 7-Seg display
		}
	}

void got_start()
	{
	/* Here's the code to do when the Start Button is pushed
		 When Start is pressed:
		 1.) Display the Waiting "----"
		 2.) Wait for a random number of millisec's
		 3.) Turn on all the 7-Seg lights (that's "GO"
		 4.) Start the Reaction timer. (Hint: Use the same function you used to start the other timers)
		*/
		started_doing_reaction_timers = true;
	    Clear_LEDs();
		rand_millisec =  rand() % upper_limit_millisec_to_wait;

	  /**************** STUDENT TO FILL IN START HERE ********************/
		flashing_result = false;		// Stop flashing the last result
		cheated = false;
		got_stop_button = false;		// Ignore any Stop pushed before Start
		// Step 1
		Display_Waiting();				// "----"
		// Step 2 -- wait, but catch Stop being pushed too early (cheating)
		uint32_t wait_start_tick = HAL_GetTick();
		while ((HAL_GetTick() - wait_start_tick) < (uint32_t) rand_millisec)
			{
			if (got_stop_button)
				{
				cheated = true;
				return;					// main() sees got_stop_button and calls got_stop()
				}
			}
		// Step 3
		Display_All();					// "8888" means GO
		// Step 4
		HAL_TIM_Base_Start_IT(&htim3);	// Reaction timer starts counting
	  /**************** STUDENT TO FILL IN END  HERE ********************/
	}
void got_stop()
{
		/* Here's the code for the STOP button --
		 * When pushed:
		 1.) Stop the random timer (Hint: There is a similar function to the one you used to start the timer)
		 2.) Read the value of timer
		 3.) Display the value
		 */
		// 1.) Stop the timer


	  /**************** STUDENT TO FILL IN START HERE ********************/
      // 1.) Stop the random timer // Random timer is timer3
		HAL_TIM_Base_Stop_IT(&htim3);

		// Cheated: no reaction time and no chance at the best time
		if (cheated)
			{
			Display_Error();			// " Err"
			printf("Too early! Wait for 8888 before pushing STOP\n\r");
			start_flashing_result();
			return;
			}

      // 2.) Read the value of the timer -- this step provided
		last_reaction_time_in_millisec = __HAL_TIM_GetCounter(&htim3) / 10; // Why is it divide by 10?
		// TIM3 clock is 80 MHz / 8000 (prescaler) = 10 kHz, so each count is 0.1 mS -> 10 counts per mS

	  // 3.) Display the value
		MultiFunctionShield_Display(last_reaction_time_in_millisec);
		start_flashing_result();

      /**************** STUDENT TO FILL IN END HERE ********************/
		// Keep the best time in a global variable
		if (last_reaction_time_in_millisec < best_reaction_time_in_millisec) best_reaction_time_in_millisec = last_reaction_time_in_millisec;
		// Show some stats
		printf("Random Delay was: %d\n\r", rand_millisec );
		printf("Reaction Time from Timer   : %d\n\r", last_reaction_time_in_millisec);
		// Just to keep things random -- reseed with the last reaction time
	    srand((unsigned) last_reaction_time_in_millisec );
}


void got_fastest()
		{
		got_fastest_button = false;
		MultiFunctionShield_Display(best_reaction_time_in_millisec);
		}


static void start_flashing_result()
	{
	got_start_button = false;		// Ignore any Start pushed during the test, so the result gets seen
	got_fastest_button = false;
	result_showing = true;
	last_flash_tick = HAL_GetTick();
	flashing_result = true;
	}

void flash_last_reaction_time()
	{
	// Called over and over from main() while waiting for the next Start push
	if (got_fastest_button)			// S3 is showing the best time, so stop flashing over it
		{
		flashing_result = false;
		got_fastest();
		}
	if (!flashing_result) return;
	if ((HAL_GetTick() - last_flash_tick) < flash_period_millisec) return;

	last_flash_tick = HAL_GetTick();
	result_showing = !result_showing;
	if (!result_showing)
		MultiFunctionShield_Clear();
	else if (cheated)
		Display_Error();
	else
		MultiFunctionShield_Display(last_reaction_time_in_millisec);
	}


