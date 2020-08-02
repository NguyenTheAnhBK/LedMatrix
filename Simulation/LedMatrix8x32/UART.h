#include<avr/interrupt.h>

volatile unsigned char _data;

// Initial
void USART_init(unsigned int baud){
	// Set baud rate with USART Baud Rate Register
	// Voi U2X = 0
	UBRRH = (unsigned char) (baud >> 8);
	UBRRL = (unsigned char) baud;
	
	// Enable receiver and transmitter (USART Control and Status Register B)
	UCSRB = (1 << RXEN) | (1 << TXEN);
	// Cho phep ngat sau khi nhan xong
	//UCSRB |= (1 << RXCIE);

	// Set frame format: 8 (data), no parity (USART Parity Mode = 0), 1 stop (USBS = 0)
	// Chon thanh ghi USCRC voi USART Register Select 
	// Chon charecter size voi USART Character Size (UCSZ0 -> UCSZ2)
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);

	//sei(); // set bit I cho phep ngat toan cuc
}


// Transmitter (ATmega co buffer 3 bytes cho TX
void USART_transmit(unsigned char c){
	// UART Data Register Empty = 1 thi bat dau truyen (nhan) du lieu
	while(!(UCSRA & (1 << UDRE))){}; 
	UDR = c; // UART Data Register
}

void USART_transmit_string(char* data){
	while(*data != 0x00){
		USART_transmit(*data);
		data++;
	}		
}


// Receiver
unsigned char USART_receive(){
	while(!(UCSRA & (1 << RXC))){};
	// Get and return received data from buffer
	return UDR;
}

void USART_address_tx(unsigned char c){
	while(!(UCSRA & (1 << UDRE)));
	UCSRB |= (1 << TXB8);
	UDR = c;
}

// Trinh phuc vu ngat USART hoan tat nhan
ISR(SIG_UART_RECV){
	_data = UDR;
	USART_transmit(_data);
}



