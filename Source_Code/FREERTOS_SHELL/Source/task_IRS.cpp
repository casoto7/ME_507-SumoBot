//**************************************************************************************
/** \file task_IRS.cpp
 *    This file contains source code for a user interface task for a ME405/FreeRTOS
 *    test suite. 
 *
 *  Revisions:
 *    \li 09-16-2018 CTR Adapted from JRR task_user.cpp
 *    \li 09-30-2012 JRR Original file was a one-file demonstration with two tasks
 *    \li 10-05-2012 JRR Split into multiple files, one for each task
 *    \li 10-25-2012 JRR Changed to a more fully C++ version with class task_user
 *    \li 11-04-2012 JRR Modified from the data acquisition example to the test suite
 *
 *  License:
 *    This file is copyright 2012 by JR Ridgely and released under the Lesser GNU 
 *    Public License, version 2. It intended for educational use only, but its use
 *    is not limited thereto. */
/*    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 *    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUEN-
 *    TIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 *    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 *    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 *    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */
//**************************************************************************************

#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/wdt.h>                        // Watchdog timer header
#include <util/delay.h>
#include <string.h>

#include "shared_data_sender.h"
#include "shared_data_receiver.h"
#include "task_IRS.h"                      // Header for this file

#define IR_SENSE	PORTA		// Ultra sonic opponent sensors

//-------------------------------------------------------------------------------------
/** This constructor creates a new Infrared Opponent Detection task. Its main job is to
 *  call the parent class's constructor which does most of the work.
 *  @param a_name A character string which will be the name of this task
 *  @param a_priority The priority at which this task will initially run (default: 0)
 *  @param a_stack_size The size of this task's stack in bytes 
 *                      (default: configMINIMAL_STACK_SIZE)
 *  @param p_ser_dev Pointer to a serial device (port, radio, SD card, etc.) which can
 *                   be used by this task to communicate (default: NULL)
 */

task_IRS::task_IRS (const char* a_name, 
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
/* This task blinks LED connected to D2, D3, MATTAIR DEV on board LED (E0)
 */

void task_IRS::run (void)
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
			p_serial->puts("Initializing IR Sensors");
			//ADCA_REFCTRL|= ADC_REFSEL0_bm;
			ADCA_REFCTRL= 0x00;

			ADCA_EVCTRL=0x00;
			ADCA_PRESCALER|=ADC_PRESCALER2_bm;
			
			ADCA_INTFLAGS = ADC_CH0IF_bm;

			ADCA_CH0_CTRL |= ADC_CH_INPUTMODE0_bm; // 0x01
			// Connect PB1 to positive terminal
			ADCA_CH0_MUXCTRL |= ADC_CH_MUXPOS0_bm; // 0x08
			
			ADCA_CTRLA = ADC_ENABLE_bm;
			_delay_us(192);

			transition_to(IRS_ENABLE);
			break;
			
		case IRS_ENABLE:
			p_serial->puts("Enable IR Sensor");

			transition_to(IRS_READ);
			break;
			
		case IRS_READ:
			//p_serial->puts("Reading IR Sensor");
			ADCA_CTRLA |= ADC_CH0START_bm; // 0x04
			//while(!(ADCA_INTFLAGS & ADC_CH0IF_bm));
			

			
			ADCA_INTFLAGS = ADC_CH0IF_bm;
			
			char buffer [10];
			itoa(ADCA_CH0RES, buffer, 10);
			
			
			//p_serial->puts("\n");
			p_serial->puts(buffer);
			p_serial->puts("\n");
			//_delay_ms(10);

			break;
			
		case IRS_OFF:
			p_serial->puts("Hello World");
			//transition_to(LED_SET);
			break;
			
		default:
			break;
		}
		runs++;

		delay_from_to_ms(previousTicks,100);
	}
}