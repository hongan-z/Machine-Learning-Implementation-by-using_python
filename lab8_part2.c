/*	Author: Hongan Zhang
 *	Lab Section: 022  time : 9:44
 *	Assignment: Lab 8  Exercise 2
 *	Exercise Description:
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

// Demo Link: https://youtu.be/b_1_MvGjcs8

#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

// "PWM"
void set_PWM(double frequency) {
	static double current_frequency; 
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } 
		else { TCCR3B |= 0x03; } 		
		
		    if (frequency < 0.954) { OCR3A = 0xFFFF; }
				
		        else if (frequency > 31250) { OCR3A = 0x0000; }
	
				    else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0; // resets counter
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


volatile unsigned char TimerFlag = 0; 
unsigned long time = 1; 
unsigned long cnt = 0; 
void TimerSet(unsigned long T) {
	time= T;
	cnt = time;
}

void TimerOn() {
	TCCR1B 	= 0x0B;	
	OCR1A 	= 125;	
	
	TCNT1 = 0;
	cnt = time;
	SREG |= 0x80;	
}

void TimerOff() {
	TCCR1B 	= 0x00; 
}
 
const unsigned long PERIOD = 50;
const double FREQ[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
unsigned char t_A0, t_A1, t_A2;
unsigned char Fre;
unsigned char S_PWM;

typedef struct task {
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*Tick_1)(int);
} task;

task tasks[4]; 

void TimerISR() {
    unsigned char i;
    for (i = 0; i < 4; ++i)
    {
        if (tasks[i].elapsedTime > tasks[i].period)
        {
            tasks[i].state = tasks[i].Tick_1(tasks[i].state);
            tasks[i].elapsedTime = 0;
        }
        tasks[i].elapsedTime += PERIOD;
    } 
}

enum Freq_States {_Start, _Wait, incr, decr, reset};
int Function(int state) {

    switch (state)      // Transitions
    {
    case _Start:
        state = _Wait;
        Fre = 0;
        break;

    case _Wait:
        if (t_A0 && t_A1)
            state = reset;
        else if (!t_A0 && !t_A1)
            state = _Wait;
        else if (t_A0 && !t_A1)
        {
            if (Fre < 7) 
                Fre++;
            state = incr;
        }
        else 
        {
            if (Fre > 0)
                 Fre--;
            state = decr;
        }
        break;        

    case incr:
        state = (t_A0 && t_A1) ? reset : (t_A0 && !t_A1) ? incr : _Wait;
        break;
        
    case decr:
        state = (t_A0 && t_A1) ? reset : (!t_A0 && t_A1) ? decr : _Wait;
        break;

    case reset:
        state = (!t_A0 && !t_A1) ? _Wait : reset;
        break;
    
    default:
        state = _Start;
        break;
    }

    switch (state)      // State Actions
    {
    case _Start:      break;
    case _Wait:       break;
    case incr:   break;
    case decr:   break;
    case reset:  Fre = 0;    break;
    default:          break;
    }

    return state;
}

int Function1(int state) {
    set_PWM(FREQ[Fre]);
    return 1;
}

enum S_OnOff {_Start2, wait2, press};
int Function2 (int state) {
    switch (state) // Transitions
    {
    case _Start2:
        state = wait2;
        break;

    case wait2:
        if (t_A2)
        {
            if (S_PWM) {
                S_PWM = 0;
                PWM_off();
            }
            else {
                S_PWM = 1;
                PWM_on();
            }
            state = press;
        }
        else
            state = wait2;        
        break;

    case press:
        state = t_A2 ? press : wait2;
        break;        
    
    default:
        break;
    } 

    switch (state) // State Action
    {
    case _Start2:  break;
    case wait2:    break;
    case press: break;
    default:       break;
    } 

    return state;
} 

int Input(int state) {
    t_A0 =  ~PINA & 0x01;
    t_A1 = (~PINA & 0x02) >> 1;
    t_A2 = (~PINA & 0x04) >> 2;
    return 1;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0x40; PORTB = 0x00;

    /* Insert your solution below */
    Fre = 0;
    S_PWM = 0;

    unsigned char i = 0;
    tasks[i].state = 1;
    tasks[i].period = 50;
    tasks[i].elapsedTime = 0;
    tasks[i].Tick_1 = &Input;

    i++;
    tasks[i].state = _Start;
    tasks[i].period = 50;
    tasks[i].elapsedTime = 0;
    tasks[i].Tick_1 = &Function;

    i++;
    tasks[i].state = 1;
    tasks[i].period = 50;
    tasks[i].elapsedTime = 0;
    tasks[i].Tick_1 = &Function1;

    i++;
    tasks[i].state = _Start2;
    tasks[i].period = 50;
    tasks[i].elapsedTime = 0;
    tasks[i].Tick_1 = &Function2;

    TimerSet(PERIOD);
    TimerOn();

    while (1) {
        
    }

    PWM_off();


    return 1;
}
