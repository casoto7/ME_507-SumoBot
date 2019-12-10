/*
 * Sumo Bots.cpp
 *
 * Created: 10/24/2019 10:22:55 AM
 * Author : melab15
 */ 
#define     F_CPU    2000000UL

#include <avr/io.h>					// header to enable data flow control over pins
#include <util/delay.h>



int main(void)
{

	// PORT definition 
	#define MO_DRIVE	PORTC		// Motor drivers
	#define ENCODER     PORTD		// Motor encoders (PD0 provides PWM for motor 2)
	#define UL_SENSE	PORTE		// Ultra sonic opponent sensors
	
	// F_CPU is 32MHz, timer prescaled by 64 to work on 500KHz
	// It is th smallest possible prescaler that SERVO_FRAME can be held in a 16-bit value.
	#define TIMER_PRESCALER 64
	// microseconds to counter "ticks". Every tick is 2 microseconds.
	#define TICKS(n)        ((n)/2)

	// Useful constants
	#define SERVO_MIN       TICKS(1000)
	#define SERVO_CENTER    TICKS(1500)
	#define SERVO_MAX       TICKS(2000)

	// Pulse every 18 microseconds
	#define SERVO_FRAME     TICKS(1000)

	// USE timer TCD0 which is on port D
	#define TIMER   TCD0
	

	// %% -------------- INITIALIZATION ---------------- %% 
		
	// ========== motor stuff ==========
	ENCODER.DIR = 0xff;			// All 8 bits of PORT are outputs
	// Prescaler
	TIMER.CTRLA = TC_CLKSEL_DIV64_gc;
	// Enable 4 compare units, Single Slope PWM mode
	TIMER.CTRLB = (TC0_CCDEN_bm|TC0_CCCEN_bm|TC0_CCBEN_bm|TC0_CCAEN_bm|TC_WGMODE_SS_gc);
	// Period = 18 microseconds
	TIMER.PER = SERVO_FRAME;
	// 4 servos at center!
	TIMER.CCA = SERVO_CENTER;
	MO_DRIVE.DIR = 0b00011110;
	//PORTC.DIR = PIN2_bm;
	//PORTC.DIR = PIN3_bm;
	//PORTC.DIR = PIN4_bm;
	
	MO_DRIVE.OUTSET = 0b00010110;
	MO_DRIVE.OUTCLR = 0b00001000;
	
	PORTD.DIR = 0b00001100;
	
	int timer = 1;
    
    while (timer < 200) // 100 sec loop
    {
		_delay_ms(500);
		UL_SENSE.OUTTGL = PIN1_bm;		//blink LED
		PORTD.OUTTGL = 0b00001100;
		
		
		timer = timer + 1;
		
			

    }
	
	PORTC.OUTCLR = 0b00011110;


}

