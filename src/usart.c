#include "usart.h"
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

void usart_init( uint16_t baudrate){
	UBRR0H = (uint8_t)(baudrate >> 8);
	UBRR0L = (uint8_t)(baudrate);
	
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0A = (1 << U2X0);
	UCSR0C = (1 << UCSZ01)| (1 << UCSZ00);
}

void usart_send_byte( uint8_t data ){
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0  = data;
}

void usart_send_str_len( const char* s, size_t length ){
	for (uint16_t i = 0; i < length; i++){
		usart_send_byte(*s++);
	}
}

void usart_send_str_len_P( const char* s_P, size_t length ){
	for (uint16_t i = 0; i < length; i++){
		usart_send_byte(pgm_read_byte(s_P++));
	}
}

void usart_send_str( const char* s ){
	while (*s) usart_send_byte(*s++);
}

void usart_send_str_P( const char* s_P ){
	while (pgm_read_byte(s_P)) usart_send_byte(pgm_read_byte(s_P++));
}
