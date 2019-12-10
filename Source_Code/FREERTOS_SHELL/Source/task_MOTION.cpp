//**************************************************************************************
/** \file task_MOTION.cpp
 *    This file contains source code for a task responsible for dictating the
 *	  direction of motion for the sumo bot. 
 *		
 *	    REVISION HISTORY: 
 *		- Changed Motor 2 PWM Driver pin to PE0.
 */
 //**************************************************************************************


#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/wdt.h>                        // Watchdog timer header
#include <util/delay.h>

#include "shared_data_sender.h"
#include "shared_data_receiver.h"
#include "task_MOTION.h"                      // Header for this file


//-------------------------------------------------------------------------------------
// Definitions for MOTOR 1
#define INA1			PIN3_bm		// Define direction of spin (clockwise), Port C Pin 3.
#define INB1			PIN2_bm		// Define direction of spin (counter clockwise), Port C Pin 2.
#define PWM1_out		PIN0_bm		// Output pin to motor driver, Port C Pin 0.
#define PWM1_tim		TCC0		// TCC0 is timer counter for PORTC Pins (0-3).
#define PWM1_CC			TCC0.CCA	// TCC0 uses compare channel A for Pin 0.
#define PWM1_CTR		TCC0.CNT	// 
#define PERIOD			1000		// 1000 clock ticks for 100% Duty cycle

//-------------------------------------------------------------------------------------
// Definitions for MOTOR 2

#define INA2			PIN6_bm		// Define direction of spin (clockwise), Port C Pin 6.
#define INB2			PIN5_bm		// Define direction of spin (counter clockwise), Port C Pin 5.
#define PWM2_out		PIN0_bm		// Output pin to motor driver, Port E Pin 0.
#define PWM2_tim		TCE0		// TCE0 is timer coutner for PORTE pins (0-3).
#define PWM2_CC			TCE0.CCA	// TCE0 uses compare channel for Pin 0.
#define PWM2_CTR		TCE0.CNT	

task_MOTION::task_MOTION (const char* a_name, 
					  unsigned portBASE_TYPE a_priority, 
					  size_t a_stack_size,
					  emstream* p_ser_dev)
	: frt_task (a_name, a_priority, a_stack_size, p_ser_dev)
{
	// Nothing is done in the body of this constructor. All the work is done in the
	// call to the frt_task constructor on the line just above this one
}


//-------------------------------------------------------------------------------------
/* This task turns on motor 1 and motor 2.
 */

//-------------------------------------------------------------------------------------
/** This constructor creates a new data acquisition task. Its main job is to call the
 *  parent class's constructor which does most of the work.
 *  @param a_name A character string which will be the name of this task
 *  @param a_priority The priority at which this task will initially run (default: 0)
 *  @param a_stack_size The size of this task's stack in bytes
 *                      (default: configMINIMAL_STACK_SIZE)
 *  @param p_ser_dev Pointer to a serial device (port, radio, SD card, etc.) which can
 *                   be used by this task to communicate (default: NULL) 
 *  
 */

void task_MOTION::run (void)
{
	// Make a variable which will hold times to use for precise task scheduling
	portTickType previousTicks = xTaskGetTickCount ();

	// Wait a little while for user interface task to finish up
	delay_ms(10);
	
	uint8_t state_num;													// Creating a variable for shared data variable.
	
	while(1)
	{
		switch (state)
		{
		case INIT:
			
			// -----------------------------------------------------------------------------------------------------------//
			// Initialization for motor 1
			
			PWM1_CTR = 0;												// Clearing the value of timer counter used for Motor 1 PWM.
			PORTC.OUTCLR = PWM1_out;									// Clearing pins prior to configuring it as output.
			PORTC.OUTCLR = INA1 | INB1;									// Clearing pins prior to configuring it as output.
			PORTC.DIRSET = PWM1_out;									// Setting PWM pin used for Motor 1 as output.
			PORTC.DIRSET = INA1 | INB1;									// Setting direction pins used for Motor 1 as output.
			TCC0_PER = PERIOD;											// Initializing timer period to 1000.
			PWM1_tim.CTRLB = TC_WGMODE_SINGLESLOPE_gc | TC0_CCAEN_bm;	// Single slope method compared to channel CCA.
			PWM1_tim.CTRLA =TC_CLKSEL_DIV1_gc;							// Starts clock and set to highest resolution. (Uses the fastest clock speed (32 MHz))
			PWM1_CC = 0;												// Setting PWM channel used by Motor 1 to began with a value of 0%.
			PORTC.OUTSET = PWM1_out;									// Initializing Motor 1 PWM pin as an output.
			
			// -----------------------------------------------------------------------------------------------------------//
			// Initialization for motor 2
			
			PWM2_CTR = 0;												// Clearing the value of timer counter used for Motor 2 PWM.
			PORTE.OUTCLR = PWM2_out;									// Clearing pins prior to configuring it as output.
			PORTC.OUTCLR = INA2 | INB2;									// Clearing pins prior to configuring it as output.
			PORTE.DIRSET = PWM2_out;									// Setting PWM pin used for Motor 2 as output.
			PORTC.DIRSET = INA2 | INB2;									// Setting direction pins used for Motor 2 as output.
			TCE0_PER = PERIOD;											// Initializing timer period to 1000.
			PWM2_tim.CTRLB = TC_WGMODE_SINGLESLOPE_gc | TC0_CCAEN_bm;	// Single slope method compared to channel CCA.
			PWM2_tim.CTRLA = TC_CLKSEL_DIV1_gc;							// Starts clock and set to highest resolution. (Uses the fastest clock speed (32MHz))
			PWM2_CC = 0;												// Setting PWM channel used by Motor 2 to begin with a value of 0%.
			PORTE.OUTSET = PWM2_out;									// Initializing Motor 2 PWM pin as an output.
			
			transition_to(HOLD);
			break;
			
		case HOLD:
		
			state_num = p_state_num->get();								// Gets shared variable to indicate what case it should be in.
			p_serial->puts("Motor is Stopped");							// Prints out a description to indicate the case status.
			
			PWM1_CC = 0;
			PWM2_CC = 0;
		
			if (state_num == 1)											// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(FORWARD);
			}
			
			else if (state_num == 2)									// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(BACKWARD);
			}
			
			else if (state_num == 3)									// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(RIGHT);
			}
			
			else if (state_num == 4)									// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(LEFT);
			}
			
			break;
		
		case FORWARD:
			
			state_num = p_state_num->get();								// Gets shared variable to indicate what case it should be in.
			PWM1_CC = duty_cycle1->get();								// Set motor 1 PWM to value pointed to by p_duty_cycle1.
			PWM2_CC = duty_cycle2->get();								// Set motor 2 PWM to value pointed to by p_duty_cycle2.
			
			p_serial->puts("Motor Forward");							// Prints out a description to indicate the case status.
			PORTC_OUTSET = INA2;										// Telling Motor 1 to turn in the clockwise direction.
			PORTC_OUTSET = INB1;										// Telling Motor 2 to turn in the clockwise direction.
			PORTC_OUTCLR = INA1;										// Clearing values of directional pins for Motor 1.
			PORTC_OUTCLR = INB2;										// Clearing values of directional pins for Motor 2.
			
			
			if (state_num == 0)											// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(HOLD);
			}
			if (state_num == 2)											// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(BACKWARD);
			}
			
			else if (state_num == 3)									// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(RIGHT);
			}
			
			else if (state_num == 4)									// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(LEFT);
			}
			
			break;
		
		case BACKWARD:
			
			state_num = p_state_num->get();								// Gets shared variable to indicate what case it should be in.
			PWM1_CC = duty_cycle1->get();								// Set Motor 1 PWM to value pointed to by p_duty_cycle1.
			PWM2_CC = duty_cycle2->get();								// Set Motor 2 PWM to value pointed to by p_duty_cycle2.
			
			p_serial->puts("Motor Backwards");							// Prints out a description to indicate the case status.
			PORTC_OUTSET = INB2;										// Telling Motor 1 to turn in the counter-clockwise direction.
			PORTC_OUTSET = INA1;										// Telling Motor 2 to turn in the counter-clockwise direction.
			PORTC_OUTCLR = INB1;										// Clearing values of directional pins for Motor 1.
			PORTC_OUTCLR = INA2;										// Clearing values of directional pins for Motor 2.
			
			
			if (state_num == 0)											// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(HOLD);
			}
			if (state_num == 1)											// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(FORWARD);
			}
			
			else if (state_num == 3)									// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(RIGHT);
			}
			
			else if (state_num == 4)									// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(LEFT);
			}
			
			break;
		
		case RIGHT:
			
			state_num = p_state_num->get();								// Gets shared variable to indicate what case it should be in.
			PWM1_CC = duty_cycle1->get();								// Set Motor 1 PWM to value pointed to by p_duty_cycle1.
			PWM2_CC = duty_cycle2->get();								// Set Motor 2 PWM to value pointed to by p_duty_cycle2.
			
			p_serial->puts("Motor Turning Right");						// Prints out a description to indicate the case status.
			PORTC_OUTSET = INA1;										// Telling Motor 1 to turn in the clockwise direction.
			PORTC_OUTSET = INA2;										// Telling Motor 2 to turn in the counter-clockwise direction.
			PORTC_OUTCLR = INB1;										// Clearing values of directional pins for Motor 1.
			PORTC_OUTCLR = INB2;										// Clearing values of directional pins for Motor 2.
			
			
			if (state_num == 0)											// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(HOLD);
			}
			if (state_num == 1)											// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(FORWARD);
			}
			
			else if (state_num == 2)									// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(BACKWARD);
			}
			
			else if (state_num == 4)									// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(LEFT);
			}
			
			break;
			
		case LEFT:
			
			state_num = p_state_num->get();								// Gets shared variable to indicate what case it should be in.
			PWM1_CC = duty_cycle1->get();								// Set Motor 1 PWM to value pointed to by p_duty_cycle1.
			PWM2_CC = duty_cycle2->get();								// Set Motor 2 PWM to value pointed to by p_duty_cycle2.
			
			p_serial->puts("Motor Turning Left");						// Prints out a description to indicate the case status.
			PORTC_OUTSET = INB2;										// Telling Motor 2 to turn in the clockwise direction.
			PORTC_OUTSET = INB1;										// Telling Motor 1 to turn in the counter-clockwise direction. 
			PORTC_OUTCLR = INA1;										// Clearing values of directional pins for Motor 1.
			PORTC_OUTCLR = INA2;										// Clearing values of directional pins for Motor 2.
			
			
			if (state_num == 0)											// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(HOLD);
			}
			if (state_num == 1)											// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(FORWARD);
			}
			
			else if (state_num == 2)									// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(BACKWARD);
			}
			
			else if (state_num == 3)									// Logic statement checks what case the motor task should stay or transition to.
			{
				transition_to(RIGHT);
			}
			
			break;
		
		default:
		break;
		
		}
		runs++;
		  
		delay_from_to_ms(previousTicks,100);
	}
}
