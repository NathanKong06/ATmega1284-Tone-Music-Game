#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#include <util/delay.h>
#include "nokia5110.h"
#endif

void set_PWM(double frequency){
	static double current_frequency;
	if (frequency != current_frequency){
		if (!frequency) {TCCR3B &= 0x08;}
		else {TCCR3B |= 0x03;}
		if (frequency < 0.954) {OCR3A = 0xFFFF;}
		else if (frequency > 31250) {OCR3A = 0x0;}
		else {OCR3A = (short)(8000000/(128*frequency))-1;}
		TCNT3 = 0;
		current_frequency = frequency;
	}
}


void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}


void ADC_Init() {
	ADCSRA |= (1<<ADEN) | (1<<ADSC) | (1<<ADATE);
}

enum States{start,init, music,play,game,wait,input}state;

const double musics[5][8] = 
{{261.63,293.66,329.63,349.23,392,440,493.88,523.25},{523.25,493.88,440,392,349.23,329.63,293.66,261.63},{261.63,293.66,349.23,329.63,392,440,493.88,523.25},{523.25,493.88,440,392,349.23,329.63,293.66,261.63},{261.63,293.66,329.63,349.23,392,493.88,440,523.25}};

const double tones[5][8] = 
{{261.63,293.66,523.25,329.63,349.23,392,440,493.88l},{261.63,293.66,329.63,349.23,392,440,523.25,493.88},{261.63,293.66,329.63,349.23,392,440,493.88,523.25},{261.63,293.66,329.63,349.23,392,493.88,523.25,440},{261.63,293.66,329.63,349.23,440,392,493.88,392}};

const unsigned char j_size = 8;
const unsigned char i_size = 5;
const unsigned char k_size = 8;
const unsigned char l_size = 5;

unsigned char i = 0;
unsigned char j = 0;
unsigned char k = 0;
unsigned char l = 0;

char score = 0;

unsigned short temp = 0;

unsigned short x;

void Tick() {
unsigned char tmpPB = ~PINB;
switch(state){
	case start:
		i = 0;
		j = 0;
		k = 0;
		l = 0;
		score = 0;
		temp = 0;
		x = 0;
		state = init;
		break;
	case init:
		nokia_lcd_write_string("Up for Game                 Down for Song",1); //display welcome message
		nokia_lcd_render();
		nokia_lcd_clear();
		ADMUX = 0;
		x = ADC;
		_delay_ms(1000);
		if (x > 700)
			state = game;
		else if (x < 300)
			state = music;
		else
			state = init;
		break;
	case music:
		nokia_lcd_clear();
		nokia_lcd_write_string("You've chosen song. Use mic for song.",1);
		nokia_lcd_render();
		x = ADC;
		ADMUX = 1;
		set_PWM(0);
		if (tmpPB & 0x01) { //If reset button is pressed
			//nokia_lcd_write_char(score,1);	//Display score
			//nokia_lcd_render();
			_delay_ms(1000);
			nokia_lcd_clear();
			state = start;
			break;
		}
		else if (x > 85) //If mic press
			state = play;
		else// Stay in state until mic press or reset button
			state = music;
		break;
	case play:
		nokia_lcd_clear();
		if (j < j_size){ // If playing through array
			set_PWM(musics[i][j]); // Play note
			j++; // Iterate
			state = play;
		}
		else if (j >= j_size) { // Finish playing through array
			set_PWM(0);
			if (i < i_size) // If still more songs in array
				i++;
			else
				i = 0;
			j = 0;
			state = music;
		}
		break;
	case game:
		nokia_lcd_clear();
		nokia_lcd_write_string("You've chosen game. Pick higher tone.", 1);
		
		if (tmpPB & 0x01) { //If reset button is pressed
			//nokia_lcd_write_char(score,1);	//Display score
			//nokia_lcd_render();
			_delay_ms(1000);
			nokia_lcd_clear();
			score = 0;
			state = start;
			break;
		}
		if ((k < (k_size - 1)) && (l < (l_size - 1))) { // k and l are in range
			set_PWM(tones[k][l]);
			_delay_ms(300);
			set_PWM(tones[k][l+1]);
			_delay_ms(300);			
			l++;
			state = wait;
		}
		if ((k < (k_size - 1)) && (l >= (l_size - 1))) { // k is in range but l is not
			l = 0;
			k++;
			state = wait;
		}
		if ((k >= (k_size - 1))) { // k is not in range 
			k = 0;
			l = 0;
			state = wait;
		}

		break;
		
	case wait:
		nokia_lcd_clear();
		nokia_lcd_write_string("Waiting for   input",1);
		set_PWM(0);
		x = ADC;
		ADMUX = 0;
		if (tmpPB & 0x01) { //If reset button is pressed
			//nokia_lcd_write_char(score,1);	//Display score
			//nokia_lcd_render();
			_delay_ms(1000);
			nokia_lcd_clear();
			score = 0;
			state = start;
			break;
		}
		if (x > 700 || x < 300){
			temp = x;
			state = input;
		}
		else 
			state = wait;	
		break;
	case input:
		if (l != 0) {
			if ((tones[k][l-1] > tones[k][l]) && temp < 300) {
				score++;
				if (score == 0)
					nokia_lcd_write_string("0",3);
				else if (score == 1)
					nokia_lcd_write_string("1",3);
				else if (score == 2)
					nokia_lcd_write_string("2",3);
				else if (score == 3)
					nokia_lcd_write_string("3",3);
				else if (score == 4)
					nokia_lcd_write_string("4",3);
				else if (score == 5)
					nokia_lcd_write_string("5",3);
				else if (score == 6)
					nokia_lcd_write_string("6",3);
				else if (score == 7)
					nokia_lcd_write_string("7",3);
				else if (score == 8)
					nokia_lcd_write_string("8",3);
				else if (score == 9)
					nokia_lcd_write_string("9",3);
				else if (score == 10)
					nokia_lcd_write_string("10",3);
				else if (score == 11)
					nokia_lcd_write_string("11",3);
				else if (score == 12)
					nokia_lcd_write_string("12",3);
				else if (score == 13)
					nokia_lcd_write_string("13",3);
				else if (score == 14)
					nokia_lcd_write_string("14",3);
				else if (score == 15)
					nokia_lcd_write_string("15",3);
				else if (score == 16)
					nokia_lcd_write_string("16",3);
				else if (score == 17)
					nokia_lcd_write_string("17",3);
				else if (score == 18)
					nokia_lcd_write_string("18",3);
				else if (score == 19)
					nokia_lcd_write_string("19",3);
				else if (score == 20)
					nokia_lcd_write_string("20",3);
				else{
					nokia_lcd_write_string("You have won.",1);
					nokia_lcd_render();
					_delay_ms(3000);
					state = start;
					score = 0;
					break;
				}
		//		nokia_lcd_write_char(score + '0',7); 	//display score		
				nokia_lcd_render();
				_delay_ms(3000);
				state = game;
			}
			else if ((tones[k][l-1] < tones[k][l]) && temp > 700) {
				score++;
				if (score == 0)
					nokia_lcd_write_string("0",3);
				else if (score == 1)
					nokia_lcd_write_string("1",3);
				else if (score == 2)
					nokia_lcd_write_string("2",3);
				else if (score == 3)
					nokia_lcd_write_string("3",3);
				else if (score == 4)
					nokia_lcd_write_string("4",3);
				else if (score == 5)
					nokia_lcd_write_string("5",3);
				else if (score == 6)
					nokia_lcd_write_string("6",3);
				else if (score == 7)
					nokia_lcd_write_string("7",3);
				else if (score == 8)
					nokia_lcd_write_string("8",3);
				else if (score == 9)
					nokia_lcd_write_string("9",3);
				else if (score == 10)
					nokia_lcd_write_string("10",3);
				else if (score == 11)
					nokia_lcd_write_string("11",3);
				else if (score == 12)
					nokia_lcd_write_string("12",3);
				else if (score == 13)
					nokia_lcd_write_string("13",3);
				else if (score == 14)
					nokia_lcd_write_string("14",3);
				else if (score == 15)
					nokia_lcd_write_string("15",3);
				else if (score == 16)
					nokia_lcd_write_string("16",3);
				else if (score == 17)
					nokia_lcd_write_string("17",3);
				else if (score == 18)
					nokia_lcd_write_string("18",3);
				else if (score == 19)
					nokia_lcd_write_string("19",3);
				else if (score == 20)
					nokia_lcd_write_string("20",3);
				else{
					nokia_lcd_write_string("You have won.",1);
					nokia_lcd_render();
					_delay_ms(3000);
					state = start;
					score = 0;
					break;
				}
		//		nokia_lcd_write_char(score + '0',7); //display score					
				nokia_lcd_render();
				_delay_ms(3000);
				state = game;
			}
			else if (tones[k][l-1] == tones[k][l]) {
				score++;
				if (score == 0)
					nokia_lcd_write_string("0",3);
				else if (score == 1)
					nokia_lcd_write_string("1",3);
				else if (score == 2)
					nokia_lcd_write_string("2",3);
				else if (score == 3)
					nokia_lcd_write_string("3",3);
				else if (score == 4)
					nokia_lcd_write_string("4",3);
				else if (score == 5)
					nokia_lcd_write_string("5",3);
				else if (score == 6)
					nokia_lcd_write_string("6",3);
				else if (score == 7)
					nokia_lcd_write_string("7",3);
				else if (score == 8)
					nokia_lcd_write_string("8",3);
				else if (score == 9)
					nokia_lcd_write_string("9",3);
				else if (score == 10)
					nokia_lcd_write_string("10",3);
				else if (score == 11)
					nokia_lcd_write_string("11",3);
				else if (score == 12)
					nokia_lcd_write_string("12",3);
				else if (score == 13)
					nokia_lcd_write_string("13",3);
				else if (score == 14)
					nokia_lcd_write_string("14",3);
				else if (score == 15)
					nokia_lcd_write_string("15",3);
				else if (score == 16)
					nokia_lcd_write_string("16",3);
				else if (score == 17)
					nokia_lcd_write_string("17",3);
				else if (score == 18)
					nokia_lcd_write_string("18",3);
				else if (score == 19)
					nokia_lcd_write_string("19",3);
				else if (score == 20)
					nokia_lcd_write_string("20",3);
				else{
					nokia_lcd_write_string("You have won.",1);
					nokia_lcd_render();
					_delay_ms(3000);
					state = start;
					score = 0;
					break;
				}
		//		nokia_lcd_write_char(score + '0',7); //display score	
				nokia_lcd_render();
				_delay_ms(3000);
				state = game;
			}
			else {
		//		nokia_lcd_write_char(score + '0',7 ); //display score and fail message 			
				if (score == 0)
					nokia_lcd_write_string("0",3);
				else if (score == 1)
					nokia_lcd_write_string("1",3);
				else if (score == 2)
					nokia_lcd_write_string("2",3);
				else if (score == 3)
					nokia_lcd_write_string("3",3);
				else if (score == 4)
					nokia_lcd_write_string("4",3);
				else if (score == 5)
					nokia_lcd_write_string("5",3);
				else if (score == 6)
					nokia_lcd_write_string("6",3);
				else if (score == 7)
					nokia_lcd_write_string("7",3);
				else if (score == 8)
					nokia_lcd_write_string("8",3);
				else if (score == 9)
					nokia_lcd_write_string("9",3);
				else if (score == 10)
					nokia_lcd_write_string("10",3);
				else if (score == 11)
					nokia_lcd_write_string("11",3);
				else if (score == 12)
					nokia_lcd_write_string("12",3);
				else if (score == 13)
					nokia_lcd_write_string("13",3);
				else if (score == 14)
					nokia_lcd_write_string("14",3);
				else if (score == 15)
					nokia_lcd_write_string("15",3);
				else if (score == 16)
					nokia_lcd_write_string("16",3);
				else if (score == 17)
					nokia_lcd_write_string("17",3);
				else if (score == 18)
					nokia_lcd_write_string("18",3);
				else if (score == 19)
					nokia_lcd_write_string("19",3);
				else if (score == 20)
					nokia_lcd_write_string("20",3);
				else{
					nokia_lcd_write_string("You have won.",1);
					nokia_lcd_render();
					_delay_ms(3000);
					state = start;
					score = 0;
					break;
				}		
				nokia_lcd_render();
				_delay_ms(3000);
				nokia_lcd_clear();
				nokia_lcd_write_string("Nice try",1);
				nokia_lcd_render();
				_delay_ms(3000);
				score = 0;
				state = init;
			}
		}
		else {
			score++;
			if (score == 0)
				nokia_lcd_write_string("0",3);
			else if (score == 1)
				nokia_lcd_write_string("1",3);
			else if (score == 2)
				nokia_lcd_write_string("2",3);
			else if (score == 3)
				nokia_lcd_write_string("3",3);
			else if (score == 4)
				nokia_lcd_write_string("4",3);
			else if (score == 5)
				nokia_lcd_write_string("5",3);
			else if (score == 6)
				nokia_lcd_write_string("6",3);
			else if (score == 7)
				nokia_lcd_write_string("7",3);
			else if (score == 8)
				nokia_lcd_write_string("8",3);
			else if (score == 9)
				nokia_lcd_write_string("9",3);
			else if (score == 10)
				nokia_lcd_write_string("10",3);
			else if (score == 11)
				nokia_lcd_write_string("11",3);
			else if (score == 12)
				nokia_lcd_write_string("12",3);
			else if (score == 13)
				nokia_lcd_write_string("13",3);
			else if (score == 14)
				nokia_lcd_write_string("14",3);
			else if (score == 15)
				nokia_lcd_write_string("15",3);
			else if (score == 16)
				nokia_lcd_write_string("16",3);
			else if (score == 17)
				nokia_lcd_write_string("17",3);
			else if (score == 18)
				nokia_lcd_write_string("18",3);
			else if (score == 19)
				nokia_lcd_write_string("19",3);
			else if (score == 20)
				nokia_lcd_write_string("20",3);
			else{
				nokia_lcd_write_string("You have won.",1);
				nokia_lcd_render();
				_delay_ms(3000);
				state = start;
				score = 0;
				break;
			}
		//	nokia_lcd_write_char(score,7); 	//display score
			nokia_lcd_render();
			_delay_ms(3000);
			state = game;
		}
		break;
	}
	nokia_lcd_render();
}

int main(void) {
   	ADC_Init();
	DDRA = 0x00; PORTA = 0xFF; 
  	DDRB = 0xFE; PORTB = 0x01;
	DDRC = 0xFF; PORTC = 0x00;
	state = start;

	TimerSet(250);
	TimerOn();

	nokia_lcd_init();
	nokia_lcd_clear();

	PWM_on();

    while (1) {
	Tick();
	while(!TimerFlag);
	TimerFlag = 0;
    }
	PWM_off();
    return 1;

}
