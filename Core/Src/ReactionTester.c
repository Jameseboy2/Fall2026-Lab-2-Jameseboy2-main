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
		// Step 1
		Display_Waiting();				// "----"
		// Step 2
		HAL_Delay(rand_millisec);
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

      // 2.) Read the value of the timer -- this step provided
		last_reaction_time_in_millisec = __HAL_TIM_GetCounter(&htim3) / 10; // Why is it divide by 10?
		// TIM3 clock is 80 MHz / 8000 (prescaler) = 10 kHz, so each count is 0.1 mS -> 10 counts per mS

	  // 3.) Display the value
		MultiFunctionShield_Display(last_reaction_time_in_millisec);

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


