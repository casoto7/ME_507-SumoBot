/*
* UL_Sensing.cpp
*
* Created: 10/31/19
* Author : Joseph Lin
* Purpose: Task file that detects whether the SumoBot is inside the ring.
*/



#include <avr/io.h>							// header to enable data flow control over pins
#include <util/delay.h>
#include <avr/wdt.h>                        // Watchdog timer header

#include "shared_data_sender.h"
#include "shared_data_receiver.h"
#include "task_LINE_SENSE.h"                      // Header for this file

using namespace std;


//-------------------------------------------------------------------------------------
/** This constructor creates a new data acquisition task. Its main job is to call the
 *  parent class's constructor which does most of the work.
 *  @param a_name A character string which will be the name of this task
 *  @param a_priority The priority at which this task will initially run (default: 0)
 *  @param a_stack_size The size of this task's stack in bytes 
 *                      (default: configMINIMAL_STACK_SIZE)
 *  @param p_ser_dev Pointer to a serial device (port, radio, SD card, etc.) which can
 *                   be used by this task to communicate (default: NULL)
 */


task_LINE_SENSE::task_LINE_SENSE (const char* a_name, 
					  unsigned portBASE_TYPE a_priority, 
					  size_t a_stack_size,
					  emstream* p_ser_dev
					 )
	: frt_task (a_name, a_priority, a_stack_size, p_ser_dev)
	
{
	// Nothing is done in the body of this constructor. All the work is done in the
	// call to the frt_task constructor on the line just above this one
}

//-------------------------------------------------------------------------------------

void task_LINE_SENSE::run (void)
{
	// Make a variable which will hold times to use for precise task scheduling
	portTickType previousTicks = xTaskGetTickCount ();
	
	delay_ms(10);	// Wait a little for user interface task to finish up
	
	while(1)
	{
		switch (state)							
		{
		case INIT:
			p_serial->puts("Init");

			PORTD_DIRSET = PIN7_bm;				// Sets Port D Pin 7 as an output.
			PORTD_OUTCLR = PIN7_bm;				// Sets DIR pin on level shifter, U5, to low (Data transmitted B->A).
			PORTF_DIRCLR = PIN0_bm | PIN4_bm;	// Sets Port F Pin 0 and Port F Pin 4 as inputs (PF0-Left Side Mounted, PF4-Right Side Mounted).
			transition_to(READING);
			break;
			
		case READING:
			p_serial->puts("Reading");			// Prints out a description to indicate the case status.

			if (!(PORTF.IN & PIN0_bm))			// Logic state to detect whether the left mounted line sensor is triggered.
			{
				p_serial->puts("Left");			// Prints out a description of which line sensor has been triggered.
				p_state_num->put(3);			// Sends shared variable p_state_num to Motion task to drive motors to turn right.
				duty_cycle1->put(100);			// Sends shared variable duty_cycle1 to Motion task to set Motor 1 duty cycle to 10%.
				duty_cycle2->put(100);			// Sends shared variable duty_cycle2 to Motion task to set Motor 2 duty cycle to 10%.
			}
			
			else if (!(PORTF.IN & PIN4_bm))
			{
				p_serial->puts("Right");		// Prints out a description of which line sensor has been triggered.
				p_state_num->put(4);			// Sends shared variable p_state_num to Motion task to drive motors to turn left.
				duty_cycle1->put(100);			// Sends shared variable duty_cycle1 to Motion task to set Motor 1 duty cycle to 10%.
				duty_cycle2->put(100);			// Sends shared variable duty_cycle2 to Motion task to set Motor 2 duty cycle to 10%.
			}
			else
			{
				p_serial->puts("Nothing");		// Prints out a description to indicate neither line sensor has been triggered.
				p_state_num->put(1);			// Sends shared variable p_state_num to Motion task to drive motors to go forward.
				duty_cycle1->put(100);			// Sends shared variable duty_cycle1 to Motion task to set Motor 1 duty cycle to 10%.
				duty_cycle2->put(500);			// Sends shared variable duty_cycle2 to Motion task to set Motor 2 duty cycle to 10%.
			}
			break;
			
		default:
			break;
		
		}
		runs++;
		
		delay_from_to_ms(previousTicks,100);
	}
}


