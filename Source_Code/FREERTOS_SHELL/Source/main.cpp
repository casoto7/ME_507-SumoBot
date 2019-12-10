/*! \mainpage SumoBot Mainpage
 *
 *	\section intro_sec Introduction
 *
 *	Within the last decade, there has been a growing movement of hobbyists fascinated with miniature 
 *	robotic  competitions with names such as Battle Bots or SumoBots. To explore this field, we 
 *	partnered with a few other groups to set up a modified sumo bot competition, adapted for the 
 *	desired learning objectives of this class. Our SumoBot competition will utilize fully autonomous
 *	robots designed to replicate the sport of sumo wrestling. The objective is simple; two robots will
 *	attempt to push the other out of a pre-established circular arena to establish victory. 
 *
 *	\section sec_purpose Purpose
 *	
 *	The purpose of this source code is to run the two motors and infrared line sensors
 *	necessary to operate our designed SumoBot.
 *	
 *	\section sec_execution	Execution
 *
 *	To properly execute this source code, a programmer capable of communicating with an
 *	ATxmega128A3U and PDI capability was used. This programmer would allow us to get code
 *	onto our microcontroller on our designed printed circuit board. The infrared line sensors
 *	were connected to the X13 and X11 headers allocated on our printed circuit boards. Lastly,
 *	there was two separate power source screw terminal blocks for the microcontroller and motors.
 *	Screw terminal block, X1, had a 6V power supply connected to power the microcontroller and all 
 *	sensors being used. Screw terminal block, X2, had a 12V power supply connected to power the two
 *	motors being used.
 *
 *	\section sec_testing Testing
 *	
 *	To effectively validate or debug our board, we approached board assembly in a methodical way that
 *	would allow us to isolate any potential bugs in our system. We began by soldering all power supply
 *	subsystems on the board and checked the voltage regulator outputs to ensure proper voltage levels.
 *	We then proceeded to verification of the level shifters. Ensure that our USB to serial UART interface
 *	was working properly. Lastly, we ensured the functionality of the microcontroller being used.  
 *
 *	\section sec_non-implementations Non-Implementations
 *
 *	With our ultrasonic sensors, we were also unable to incorporate them with the FreeRTOS system. We
 *	did not spend extensive time debugging this, but we suspect that there could be a timing conflict.
 *	The ultrasonic sensors were coded to utilize interrupts for measuring time to objects. This may have been
 *	conflicting with other high level interrupts being utilized within FreeRTOS. Additionally, as many other
 *	groups were using the same HC SR04 ultrasonic sensors, we were concerned that our sensor could interpret
 *	their ranging signal as an input and give us inaccurate readings. With this in mind, we decided our efforts
 *	would be more worthwhile to focus on developing code for our other sensors. The QRE1113 infrared line sensors
 *	were also not implemented with the FreeRTOS system. The digital sensor works by setting the input/output pin
 *	as a output to high to charge the capacitor on the board. Then, the input/output line to an input and measure
 *	the amount of time it takes the capacitor to discharge. We were not able to measure the amount of time the
 *	capacitor takes to discharge depending what type of color the sensor was above (white or black).
 *
 *	\section sec_limitations Limitations
 *
 *	The infrared line sensors ultimately used (OSOYOO Infrared Sensor) could pose potential problems if it picks 
 *	up light reflections from other components. The line sensors could trigger a false reading if there is too much
 *	light pollution. The pin generating the PWM signal for Motor 2 was causing trouble when testing our SumoBot. We
 *	were getting an inconsistent signal, which ultimately lead to our second motor not running.
 *
 *	The code is located at our group's bit buck repository:
 *	
 *	\authors Joseph Lin
 *	\authors Carlos Soto
 *	\authors Dexter Yanagisawa
 *	
 *	\date December 9, 2019
 */

//*************************************************************************************
/** \file lab1_main.cpp
 *    This file contains the main() code for a SumoBot for ME 507. The separate code
 *	  files included set up and run the various tasks for the infrared line sensors,
 *	  motor drivers and microcontroller. It includes code for tasks written for infrared 
 *	  opponent detection sensors and ultrasonic opponent detection sensors that where not
 *    implemented effectively to run with the Free RTOS system.
 */
//*************************************************************************************


#include <stdlib.h>                         // Prototype declarations for I/O functions
#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/wdt.h>                        // Watchdog timer header
#include <avr/interrupt.h>
#include <string.h>                         // Functions for C string handling

#include "FreeRTOS.h"                       // Primary header for FreeRTOS
#include "task.h"                           // Header for FreeRTOS task functions
#include "queue.h"                          // FreeRTOS inter-task communication queues
#include "croutine.h"                       // Header for co-routines and such

#include "rs232int.h"                       // ME405/507 library for serial comm.
#include "time_stamp.h"                     // Class to implement a microsecond timer
#include "frt_task.h"                       // Header of wrapper for FreeRTOS tasks
#include "frt_text_queue.h"                 // Wrapper for FreeRTOS character queues
#include "frt_queue.h"                      // Header of wrapper for FreeRTOS queues
#include "frt_shared_data.h"                // Header for thread-safe shared data
#include "shares.h"                         // Global ('extern') queue declarations

#include "xmega_util.h"

#include "task_user.h"						// Header for user interface task.
#include "task_LED.h"						// Header for LED task.
#include "task_MOTION.h"					// Header for Motion task.
#include "task_IRS.h"						// Header for Infrared Opponent Sensors.
#include "task_ULS.h"						// Header for Ultrasonic Opponent Sensors.
#include "task_LINE_SENSE.h"				// Header for Infrared Line Sensors.

#include <util/delay.h>
frt_text_queue print_ser_queue (32, NULL, 10);


//=====================================================================================
/** The main function sets up the RTOS.  Some test tasks are created. Then the 
 *  scheduler is started up; the scheduler runs until power is turned off or there's a 
 *  reset.
 *  @return This is a real-time micro controller program which doesn't return. Ever.
 */

/// An unsigned 16-bit integer that will be used to set the duty cycle of Motor 1.
shared_data <uint16_t>* duty_cycle1;				// Declaring shared variable that sets duty cycle for motor 1.

/// An unsigned 16-bit integer that will be used to set the duty cycle of Motor 2.
shared_data <uint16_t>* duty_cycle2;				// Declaring shared variable that sets duty cycle for motor 2.

/// An unsigned 8-bit integer that will be used to determine what case the motors shall be in.
shared_data <uint8_t>* p_state_num;					// Declaring shared variable that sets state number for motors.

/// A boolean to tell the task for the ultrasonic opponent sensors to start or stop the interrupt timer.
shared_data <bool>* p_receive;						// Declaring shared variable that tells interrupt to start or stop timer.

/// A boolean to tell the task for the ultrasonic opponent sensors to transition to conversion state.
shared_data <bool>* p_convert;						// Declaring shared variable that transitions to conversion state.

/// An unsigned 32-bit integer that will be used to store the travel time of the ultra sonic sensor.
shared_data <int32_t>* duration;					// Declaring shared variable that holds Ultra Sonic travel time.

/// An unsigned 16-bit integer that will be used to store the calculated distance value.
shared_data <uint16_t>* distance;					// Declaring shared variable that holds converted distance value.


int main (void)
{
	cli();
	// Configure the system clock to use internal oscillator at 32 MHz
	config_SYSCLOCK();
	
	// Disable the watchdog timer unless it's needed later. This is important because
	// sometimes the watchdog timer may have been left on...and it tends to stay on	 
	wdt_disable ();

	// Configure a serial port which can be used by a task to print debugging infor-
	// mation, or to allow user interaction, or for whatever use is appropriate.  The
	// serial port will be used by the user interface task after setup is complete and
	// the task scheduler has been started by the function vTaskStartScheduler()
	rs232 ser_dev(115200, &USARTF0); // Create a serial device on USART F0
	ser_dev << clrscr << "FreeRTOS Xmega Testing Program" << endl << endl;
	
	// The user interface is at low priority; it could have been run in the idle task
	// but it is desired to exercise the RTOS more thoroughly in this test program
	new task_user ("UserInt", task_priority (0), 260, &ser_dev);
	
	// The LED blinking task is also low priority and is used to test the timing accuracy
	// of the task transitions.
	new task_LED ("LED_BLINKER", task_priority (1), 260, &ser_dev);
	
	// The Motion task is a medium priority and is used to drive the two motors being used.
	new task_MOTION ("Motor Driver", task_priority (2), 260, &ser_dev); 
	
	// The Line Sense task has the highest priority and is used to the detect when the SumBot
	// is approaching the edge of the ring.
	new task_LINE_SENSE("Line Sensor", task_priority(4), 260, &ser_dev);
	
	// The IRS task is high priority and is used to detect the opponent in the ring using an
	// analog infrared distance sensor. Task is intentionally commented out because it does
	// not execute properly run with the FreeRTOS system.
	//new task_IRS ("IRS", task_priority (3), 260, &ser_dev);
	
	// The ULS task is high priority and is used to detect the opponent in the ring using
	// ultrasonic pulses to determine how far an object is. Task is intentionally commented
	// out because it does not execute properly with the FreeRTOS system. 
	//new task_ULS ("Ultrasonic Opponent Sensing", task_priority(3), 260, &ser_dev);
	
	// Here's where the RTOS scheduler is started up. It should never exit as long as
	// power is on and the micro controller isn't rebooted
	vTaskStartScheduler ();
	
	// Declaring shared variables.
	duty_cycle1 = new shared_data<uint16_t>;
	duty_cycle2 = new shared_data<uint16_t>;
	p_state_num = new shared_data<uint8_t>;
	p_receive = new shared_data <bool>;
	p_convert  = new shared_data <bool>;
	duration = new shared_data <int32_t>;
	distance = new shared_data <uint16_t>;
	
}