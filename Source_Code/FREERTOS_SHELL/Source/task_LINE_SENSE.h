//**************************************************************************************
/** \file task_LINE_SENSE.h
 *  This file contains header stuff for the Line Sense task for ME 507 SumoBot
 *	Competition (Fall 2019 ME 507).
 *  Revisions:
 *    11/14/19 JLL
 *	*/
//**************************************************************************************

// This define prevents this .h file from being included multiple times in a .cpp file

#ifndef _TASK_LINE_SENSE_H_
#define _TASK_LINE_SENSE_H_

#include <stdlib.h>                         // Prototype declarations for I/O functions

#include "FreeRTOS.h"                       // Primary header for FreeRTOS
#include "task.h"                           // Header for FreeRTOS task functions
#include "queue.h"                          // FreeRTOS inter-task communication queues

#include "rs232int.h"                       // ME405/507 library for serial comm.
#include "time_stamp.h"                     // Class to implement a microsecond timer
#include "frt_task.h"                       // Header for ME405/507 base task class
#include "frt_queue.h"                      // Header of wrapper for FreeRTOS queues
#include "frt_text_queue.h"                 // Header for a "<<" queue class
#include "frt_shared_data.h"                // Header for thread-safe shared data

#include "shares.h"                         // Global ('extern') queue declarations

#include "math.h"

//-------------------------------------------------------------------------------------


/// Class that creates a task for infrared line sensor to detect the edge of ring.
class task_LINE_SENSE : public frt_task
{
private:
	
protected:
	
	enum LINE_SENSE_states{
		INIT,
		READING
		};	
public:
	// This constructor creates a user interface task object
	task_LINE_SENSE (const char*, unsigned portBASE_TYPE, size_t, emstream*);
	void run(void);		// method is called by RTOS once to run the task loop for ever and ever
};

#endif //used to terminate multiple line if command