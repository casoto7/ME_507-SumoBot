//**************************************************************************************
/** \file task_ULS.h
 *    This file contains header stuff for an Ultranic Distance Sensor task for ME 507
 *	Sumo Bot competition (Fall 2019)
 *  Revisions:
 *    11/14/19 JLL
 *	*/
//**************************************************************************************

// This define prevents this .h file from being included multiple times in a .cpp file

#ifndef _TASK_ULS_H_
#define _TASK_ULS_H_

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

/// Class that creates a task for ultrasonic opponent sensors to the distance of an opponent.
class task_ULS : public frt_task
{
private:
	
protected:
	
	enum ULS_states{
		INIT,
		ULS_TRIGGER,
		ULS_ECHO,
		ULS_CONVERT
		};	
public:
	// This constructor creates a user interface task object
	task_ULS (const char*, unsigned portBASE_TYPE, size_t, emstream*);
	void run(void);		// method is called by RTOS once to run the task loop for ever and ever
};

#endif //used to terminate multiple line if command