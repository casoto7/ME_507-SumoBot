//**************************************************************************************
/** \file task_MOTION.h
 *    This file contains header stuff for creating a task responsible for dictating the 
 *	  direction of motion for the sumo bot */
 //**************************************************************************************
 
 // This define prevents this .h file from being included multiple times in a .cpp file
#ifndef _TASK_MOTION_H_
#define _TASK_MOTION_H_
 
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

/// Class that creates a task that dictates the different modes of motion for the SumoBot.
class task_MOTION : public frt_task
{
private:
	// No private variables or methods for this class

protected:
	
	enum MOTION_states 
	{
		INIT,
		HOLD,
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
	};								// !< Task state
	
	uint8_t ctr;					//!< Loop Counter
	

public:
	// This constructor creates a user interface task object
	task_MOTION (const char*, 
				 unsigned portBASE_TYPE, 
				 size_t, 
				 emstream*);

	/** This method is called by the RTOS once to run the task loop for ever and ever.
	 */
	void run (void);
};

#endif // _TASK_MOTION_H_
