/*
Functions:
button n.1:	cycling modes
			strip flashes according to selected mode
MODE 1:	buttons 2..8 changes colors using array variable "preset_colors"

MODE 2: buttons 2 and 3	changes red channel
		buttons 4 and 5	changes green channel
		buttons 6 and 7	changes blue channel
		button 8 sets all channels to zero

MODE 3: fading between 7 basic colors

*/

/*DEFINE ESSENTIALS*/
#define F_CPU 11059200
#define BUTTONS PORTD			//IMPORTATNT: Buttons needs to be on PORT D
#define LED PORTC
#define DDR(p) (*(&p - 1))
#define PIN(p) (*(&p - 2)) 

/*DEFINE CONSTANTS*/
#define red 0
#define gre 1
#define blu 2
#define reset 0x00

/*LIBRARIES*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*VARIABLES*/
uint8_t rgb_val[3] = {0,0,0};						//RGB value (0-255)
const uint8_t preset_colors[8][3] = {{0,0,0},
									{255,0,0},		//RED
									{0,255,0},		//GREEN
									{0,0,255},		//BLUE
									{255,255,0},	//YELLOW
									{0,255,255},	//CYAN
									{255,0,255},	//PURPLE
									{255,255,255}};	//WHITE	
bool buttons[8];									//array for button states
uint8_t MODE = 0;									//mode counter
volatile int counter = 256;							//PWM simulation counter
bool btn_beg = 0;									//temp variable for btn_get function

/*FUNCTIONS*/
void btn_get();
void mode_0();
void mode_1();
void mode_2();
void led_reset();
void led_white();
void fade(uint8_t color, bool sign);

ISR(TIMER1_COMPA_vect) {
	counter--;					//advance counter
	if (counter == 0) {
		counter = 256;			//reset counter
		LED = reset;			//turn off LEDs
	} 
	else {
		if (counter == rgb_val[red]) {LED |= (1<<red);}		//set corresponding colors
		if (counter == rgb_val[gre]) {LED |= (1<<gre);}
		if (counter == rgb_val[blu]) {LED |= (1<<blu);}
	}
}

ISR(INT0_vect) {
	if(MODE == 2) {buttons[0] = 1;}				//force button 0 to be active in fading mode 
}

int main(void) {
	//LED PORT
	DDR(LED) = 0xff;			//set PORT to output
	LED = reset;				//set all pins low (reset)
	
	//BUTTONS PORT
	DDR(BUTTONS) = 0x00;		//set PORT to input
	BUTTONS = 0xff;				//add pull-up resistors
	/*
	255 devisions @ 100Hz
	39,2157 us (25.5kHz)
	25.6kHz => OCR1A = 215; N = 1
	*/
	TCCR1B = (1<<WGM12) | (1<<CS10);	//Select CTC mode + set No prescaler
	OCR1A = 215;						//Set top of counter
	TIMSK = (1<<OCIE1A);				//enable interrupt vector on compare match
	
	MCUCR = (1<<ISC01);					//falling edge of INT0
	GICR = (1<<INT0);					//enable external interrupt INT0
	
	sei();								//enable interrupt routines
	
	
	while (1) {
		_delay_ms(50);									//button spam prevention
		btn_get();										//update buttons
		if (buttons[0]) {								//mode changing button
			MODE++;										//cycle modes
			if (MODE == 3) {MODE = 0;}					//mode selector rollover
			led_reset();								//clear LED strip
			for (int i = 0; i <= MODE; i++) {			//blink white color according to selected mode
				_delay_ms(100);
				led_white();
				_delay_ms(100);
				led_reset();
			}
		buttons[0] = 0;
		}
		switch(MODE) {									//call mode functions
			case 0:
				mode_0();
				break;
			case 1:
				mode_1();
				break;
			case 2:
				mode_2();
				break;
			default:
				break;
		}
	}
}

void btn_get() {
	btn_beg = buttons[0] ? 1 : 0;
	for (uint8_t btn = btn_beg; btn <= 7; btn++) {
		if (!(PIN(BUTTONS) & (1<<btn))) {			//test for every button
			buttons[btn] = 1;						//set variable to true/false for every button
			} else {
			buttons[btn] = 0;
		}
	}
}

void mode_0() {
	for (uint8_t btn = 0; btn <= 7; btn++) {		//cycle through all buttons
		if (buttons[btn]) {
			rgb_val[0] = preset_colors[btn][0];		//set values to preset colors 
			rgb_val[1] = preset_colors[btn][1];
			rgb_val[2] = preset_colors[btn][2];
		}
	}
}

void mode_1() {										//custom color mode
	if (buttons[1]) {	rgb_val[0] += 10;	}		//red channel
	if (buttons[2]) {	rgb_val[0] -= 10;	}
	if (buttons[3]) {	rgb_val[1] += 10;	}		//green channel
	if (buttons[4]) {	rgb_val[1] -= 10;	}
	if (buttons[5]) {	rgb_val[2] += 10;	}		//blue channel
	if (buttons[6]) {	rgb_val[2] -= 10;	}
	if (buttons[7]) {	led_reset();		}		//reset color
}

void mode_2() {			//7 color fading
	rgb_val[red] = 255;
	_delay_ms(50);
	fade(gre, 1);		
	_delay_ms(50);
	fade(red, 0);
	_delay_ms(50);
	fade(blu, 1);
	_delay_ms(50);
	fade(gre, 0);
	_delay_ms(50);
	fade(red, 1);
	_delay_ms(50);
	fade(blu, 0);
}

void led_reset() {		//resetting strip
	rgb_val[0] = 0;
	rgb_val[1] = 0;
	rgb_val[2] = 0;
}

void led_white() {		//setting strip to white (used for mode selection)
	rgb_val[0] = 255;
	rgb_val[1] = 255;
	rgb_val[2] = 255;
}

void fade(uint8_t color, bool sign) {		//generating a slope for fading colors
	for (int s = 0; s < 17; s++) {			//17 steps
		if (sign) {							//fading on
			rgb_val[color] += 15;
		} else {							//fading off
			rgb_val[color] -= 15;
		}
		_delay_ms(12);						//fading takes 4 times a static color with 17 steps (50*4/17 = 11.76)
	}
}