#define F_CPU 8000000UL // 8MHz
#include<avr/io.h>
#include<avr/interrupt.h>

#include<util/delay.h>

#include "UART.h"

#define ST_CP_PORT PORTA
#define ST_CP_PIN 0
#define DS_PORT PORTA
#define DS_PIN 1
#define SH_CP_PORT PORTA
#define SH_CP_PIN 2

#define DS_low() DS_PORT &= ~(1 << DS_PIN);
#define DS_high() DS_PORT |= 1 << DS_PIN;
#define ST_CP_low() ST_CP_PORT &= ~(1 << ST_CP_PIN);
#define ST_CP_high() ST_CP_PORT |= 1 << ST_CP_PIN;
#define SH_CP_low() SH_CP_PORT &= ~(1 << SH_CP_PIN);
#define SH_CP_high() SH_CP_PORT |= 1 << SH_CP_PIN;


void HC595_out(unsigned char byte){
	//SH_CP_low();
	//ST_CP_low();
	for(int i = 0; i < 8; i++){
		if(bit_is_set(byte, i)){
			DS_high();
		}
		else
			DS_low();
		SH_CP_low();
		SH_CP_high();
	}
}

//unsigned char buffer[8] = {0xF0, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char buffer[] = {0x38,0x44,0x44,0x44,0x7C,0x44,0x44,0x00};

int main(){
	DDRA |= 0b00000111;
	PORTA &= 0x00;
	DDRC |= 0xFF;
	PORTC &= 0xFF;

	// Tan so quet 120Hz -> dinh thoi 8.3ms tren T/C1
	TCCR1B = (1 << CS11); //CS12 = 0, CS11 = 1, CS10 = 0: chon Prescale = 8 (1Mhz)
	TCNT1 = 57202;
	TIMSK = (1 << TOIE1); //cho phep ngat khi co tran T/C1
	sei(); //set bit I cho phep ngat toan cuc
	

	// Baud rate = 57600 bps (f = 8MHz) -> UBRRL = 8
	USART_init(8);

	USART_transmit_string("Led Matrix 8x8");


	unsigned char r = 0;
	while(1){
		unsigned char data = USART_receive();
		USART_transmit(data);

		buffer[r++] = data;	
		if(r == 8){
			//USART_transmit(buffer[0]);
			r = 0;
		}	

		//_delay_ms(10);
	}
			
	return 0;
}

ISR(TIMER1_OVF_vect){
	TCNT1 = 57202;

	//quet tung hang
	static unsigned char i;

	PORTC &= 0xFF; //tat het led

	HC595_out(buffer[i]);
	ST_CP_low();
	ST_CP_high();

	PORTC = ~(1 << i);

	i++;
	if(i == 8)
		i = 0;
}


