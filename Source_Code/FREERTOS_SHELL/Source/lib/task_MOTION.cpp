//**************************************************************************************
/** \file task_MOTION.cpp
 *    This file contains source code for a task responsible for dictating the
 *	  direction of motion for the sumo bot */
 //**************************************************************************************


#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/wdt.h>                        // Watchdog timer header
#include <util/delay.h>

#include "shared_data_sender.h"
#include "shared_data_receiver.h"
#include "task_MOTION.h"                      // Header for this file


//-------------------------------------------------------------------------------------
/* definitions for MOTOR 1 */
#define INA1			PIN3_bm		// define direction of spin (clockwise)
#define INB1			PIN2_bm		// ^^ (ccw)
#define PWM1_out		PIN0_bm		// output pin to motor driver
#define PWM1_tim		TCC0		// TCC0 is timer counter for PORTC pins 0-3
#define PWM1_CC			TCC0.CCA	// TCC0 uses compare channel A for pin 0
#define PWM1_CTR		TCC0.CNT	// 
#define PERIOD			1000		// 1000 clock ticks for 100% Duty cycle

//-------------------------------------------------------------------------------------
/* definitions for MOTOR 2 */
/*
#define INA2			PIN6_bm		// define direction of spin (clockwise)
#define INB2			PIN5_bm		// ^^ (ccw)
#define PWM2_out		PIN0_bm		// output pin to motor driver (PORTD!!)
#define PWM2_tim		TCD0		// TCC0 is timer counter for PORTC pins 0-3
#define PWM2_CC			TCD0.CCA	// TCC0 uses compare channel A for pin 0
#define PWM2_CTR		TCD0.CNT	//
*/

//-------------------------------------------------------------------------------------
/** This constructor creates a new data acquisition task. Its main job is to call the
 *  parent class's constructor which does most of the work.
 *  @param a_name A character string which will be the name of this task
 *  @param INA Set to 1 with INB at 0 to go CW 
 *  @param INB Set to 1 with INA at 0 to go CCW
 *  @param output_pin Pin from where the PWM is sent to motor driver
 *  @param timer_ch Specify which timer channel 
 *  
 */

void task_MOTION::run (void)
{
	// Make a variable which will hold times to use for precise task scheduling
	portTickType previousTicks = xTaskGetTickCount ();

	// Wait a little while for user interface task to finish up
	delay_ms(10);


	while(1)
	{
		switch (state)
		{
		case INIT:
			PWM1_CTR = 0;							// Clearing the value of timer counter.
			PORTC.OUTCLR = PWM1_out | INA1 | INB1;		// Clearing pins prior to configuring it as output.
			PORTC.DIRSET = PWM1_out;					// Setting PWM pin as output.
			TCC0_PER = PERIOD;						// Selects timer period
			PWM1_tim.CTRLB = TC_WGMODE_SINGLESLOPE_gc | TC0_CCAEN_bm;	// Single slope method compared to channel CCA.
			PWM1_tim.CTRLA =TC_CLKSEL_DIV1_gc;		// starts clock. want highest resolution, use the fastest clock speed (32 MHz)
			PWM1_CC = 0;								// Changing PWM channel to began with a value of 0%.
			PORTC.OUTSET = PWM1_out;					// Initializing PWM pin as an output.
			PORTC.DIRSET = INA1;						// spin motor cw
			PORTC.OUTSET = INA1;
			int array[] = {100, 150, 200, 250, 300, 350, 400, 450, 500, 550};
			transition_to(FORWARD);
			break;
			
		case FORWARD:
			p_serial-> puts("Motor Running");
			for(uint8_t i=0; i<10; i++ )
			{
				PWM1_CC = array[i];							// Setting PWM channel to 15%;
				_delay_ms(3000);
			}
			transition_to(HOLD);
			break;
			
		case HOLD: 
			p_serial-> puts("Motor Off");
			PWM1_CC = 0;
			break;
		//case BACKWARD:
		//case RIGHT:
		//case LEFT:
		}
	}
}
