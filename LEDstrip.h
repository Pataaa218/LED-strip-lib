/*
*	LEDstrip.h - A library to control a RGB LED strip module.
*	Created by Patrik Procházka, March 27, 2025.
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
*/

#ifndef LEDstrip_h
#define LEDstrip_h

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#ifndef LED_port
#define LED_port PORTD
#endif

#define DDR(p) (*(&p - 1))
#define red_pin 0
#define gre_pin 1
#define blu_pin 2

void LED_init();
void LED_setColor(uint8_t rgb_values[3]);
void LED_setColor(uint8_t red, uint8_t green, uint8_t blue);
void LED_clear();
void LED_white();
void LED_red();
void LED_green();
void LED_blue();
void LED_callInt();
void LED_fade();
void slope(uint8_t color, bool sign);

uint8_t _rgb[3] = {0,0,0};
volatile int _counter;

void LED_init() {
	DDR(LED_port) = 0xff;			//set PORT to output
	LED_port = 0x00;				//set all pins low
	_counter = 256;					//initiate counter
	/*
	255 devisions @ 100Hz
	39,2157 us (25.5kHz)
	25.6kHz => OCR1A = 215; N = 1
	*/
	TCCR1B = (1<<WGM12) | (1<<CS10);	//Select CTC mode + set No prescaler
	OCR1A = 215;						//Set top of counter
	TIMSK = (1<<OCIE1A);				//enable interrupt vector on compare match
	sei();								//enable interrupt routines
}

void LED_callInt() {
	_counter--;						//advance counter
	if (_counter == 0) {
		_counter = 256;				//reset counter
		LED_port = 0x00;			//turn off LEDs
	}
	else {
		if (_counter == _rgb[0]) {LED_port |= (1<<red_pin);}		//set corresponding colors
		if (_counter == _rgb[1]) {LED_port |= (1<<gre_pin);}
		if (_counter == _rgb[2]) {LED_port |= (1<<blu_pin);}
	}
}

void LED_setColor(uint8_t rgb_values[3]) {
	for (int i = 0; i <= 2; i++){_rgb[i] = rgb_values[i];}
}

void LED_setColor(uint8_t red, uint8_t green, uint8_t blue) {
	_rgb[0] = red;
	_rgb[1] = green;
	_rgb[2] = blue;
}

void LED_clear() {
	_rgb[0] = 0;
	_rgb[1] = 0;
	_rgb[2] = 0;
}

void LED_white() {
	_rgb[0] = 255;
	_rgb[1] = 255;
	_rgb[2] = 255;
}

void LED_red() {
	_rgb[0] = 255;
	_rgb[1] = 0;
	_rgb[2] = 0;
}

void LED_green() {
	_rgb[0] = 0;
	_rgb[1] = 255;
	_rgb[2] = 0;
}

void LED_blue() {
	_rgb[0] = 0;
	_rgb[1] = 0;
	_rgb[2] = 255;
}

void LED_fade() {
	LED_red();
	_delay_ms(50);
	slope(gre_pin, 1);
	_delay_ms(50);
	slope(red_pin, 0);
	_delay_ms(50);
	slope(blu_pin, 1);
	_delay_ms(50);
	slope(gre_pin, 0);
	_delay_ms(50);
	slope(red_pin, 1);
	_delay_ms(50);
	slope(blu_pin, 0);
}

void slope(uint8_t color, bool sign) {		//generating a slope for fading colors
	for (int s = 0; s < 17; s++) {
		if (sign) {
			_rgb[color] += 15;
			} else {
			_rgb[color] -= 15;
		}
		_delay_ms(12);						//fading takes 4 times a static color with 17 steps (50*4/17 = 11.76)
	}
}


#endif