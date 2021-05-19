/*	Author: hongan Zhang 5/19 4:53
 *  Partner(s) Name: 
 *
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void)
{
    /* Time
    /* Insert DDR and PORT initializations */
    // Inputs
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0x00; PORTB = 0xFF;
    // Outputs
    DDRC = 0xFF; PORTC = 0x00;

    /* Insert your solution below */
    unsigned char tmpA, tmpB;
    unsigned char count;
    unsigned char i;

    while (1) 
    {
        tmpA = PINA;
        tmpB = PINB;

        count = 0;

        for (i = 0; i < 8; i++) 
        {
            if (tmpA & 0x01) count ++;
            if (tmpB & 0x01) count ++;

            tmpA = tmpA >> 1;
            tmpB = tmpB >> 1;            
        }

        PORTC = count;
    }
    return 1;
}
