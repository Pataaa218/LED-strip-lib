/*
*
*	MIT License
*
*	Copyright (c) 2025 Pataaa218
*
*	Permission is hereby granted, free of charge, to any person obtaining a copy
*	of this software and associated documentation files (the "Software"), to deal
*	in the Software without restriction, including without limitation the rights
*	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*	copies of the Software, and to permit persons to whom the Software is
*	furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included in all
*	copies or substantial portions of the Software.
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*	SOFTWARE.
*
*	Software originally created for educational purposes, for SPŠ Tøebíè.
*	Examples and latest version can be found on Github.
*	https://github.com/Pataaa218/LED-strip-lib
*
*	This example shows how to use the library for MCU ATmega16A/644P
*	For more details see library header file version 1.1.0 or later
*/


#define F_CPU 11059200
#define LED_port PORTD				//default is PORTD

#include <avr/io.h>					//LEDstrip library also includes interrupt and delay libraries
#include "../../lib/LEDstrip.h"		//including the LEDstrip library     (change to your library directory)

ISR(TIMER1_COMPA_vect){
	LED_callInt();					//simulate 3 PWM with one timer interrupt
}

int main(void) {
	LED_init();						//initialize port, set up interrupt and enable them	
	
    while (1) {
		_delay_ms(500);
		
		LED_clear();				//clear LED strip
		slope(red_pin, 1);			//fade red color
		LED_red();					//set full red color 
		
		_delay_ms(1000);
		
		LED_clear();				//clear LED strip
		slope(gre_pin, 1);			//fade green color
		LED_green();				//set full green color
		
		_delay_ms(1000);
		
		LED_clear();				//clear LED strip
		slope(blu_pin, 1);			//fade blue color
		LED_blue();					//set full blue color
		
		_delay_ms(1000);
		
		for (int i = 0; i<5; i++) {	//call 5 times
			LED_fade();				//fade full spectrum
		}
		
		LED_clear();				//clear LED strip
		_delay_ms(500);
		
		LED_setColor(255, 155, 50);	//set custom color
		_delay_ms(1000);
		
	}
}

