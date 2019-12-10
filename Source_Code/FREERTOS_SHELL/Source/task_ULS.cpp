/*
* UL_Sensing.cpp
*
* Created: 10/31/19
* Author : Joseph Lin
* Purpose: function file that outputs pulse and interprets the return signal for distance to object
*/



#include <avr/io.h>							// header to enable data flow control over pins
#include <util/delay.h>
#include <avr/wdt.h>                        // Watchdog timer header

#include "shared_data_sender.h"
#include "shared_data_receiver.h"
#include "task_ULS.h"                      // Header for this file

using namespace std;

#define UL_SENSE	PORTE		// Ultra sonic opponent sensors

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


task_ULS::task_ULS (const char* a_name, 
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
/* This task runs Ultrasonic opponent sensors connected to ..... 
 */


void task_ULS::run (void)
{
	delay_ms(10);	// Wait a little for user interface task to finish up
	char dist_array[20];	// character array for distance int-> char conversion storage
	
	while(1)
	{
		switch (state)							// multi way branch statement. Executes different parts of code based on changing value
		{
		case INIT:
			PORTE.DIRCLR = PIN5_bm;						// sets up PE5 as input
			PORTE.PIN5CTRL = PORT_ISC_BOTHEDGES_gc;		// configure PE3 to trigger interrupt for any change in logic level.
			PORTE.INT0MASK = PIN5_bm;					// selects PE3 as source for interrupt0
			PORTE.INTCTRL = PORT_INT0LVL_MED_gc;		// Enable interrupt 0 at medium level (what if you want to use different pins of portE @different levels?
			p_receive->put(true);						// initialize interrupt to start timer
			p_convert-> put(false);						// nothing to convert at beginning
			transition_to(ULS_TRIGGER);
			break;
			
		case ULS_TRIGGER:
			PORTD.DIRSET = PIN6_bm;				// sets up PD6 as output (DIR bit on level shifters)
  			PORTE.DIRSET = PIN4_bm;				// sets up PE4 as output (USOS3 trigger)
			PORTD.OUTSET = PIN6_bm;				// set PD6 HIGH to set shifters for A->B communication
			PORTE.OUTSET = PIN4_bm;				// set PE4 HIGH to supply pulse to trigger input to start ranging
			p_serial->puts("triggering");
			_delay_ms(12);						// needs at least 10 us to send supply pulse
			PORTE.OUTCLR = PIN4_bm;				// turn off pulse
			PORTE.DIRCLR = PIN3_bm;		
			PORTD.OUTCLR = PIN6_bm;				// set PD6 LOW to shift 5V signal from sensor to 3.3 for uC
			transition_to(ULS_CONVERT);
			break;
			
		case ULS_CONVERT:
			if (p_convert->get() == true)				// if there is a value to be converted...
			{
				distance->put(4*(duration->get())/58);	//
				p_serial ->puts("lets try to print distance");
				itoa(distance->get(),dist_array, 10);
				transition_to(ULS_TRIGGER);
			}
			p_serial ->puts("STATE: ULS_CONVERT");
			_delay_ms(5000);
			break;
		}
	}
}


