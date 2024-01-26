#ifndef USART_H_
#define USART_H_

#include <stdint.h>
#include <stdlib.h>
#include "usart_bands.h"

// usart settings
void usart_init( uint16_t baudrate);
void usart_send_byte( uint8_t data );
void usart_send_str_len( const char* s, size_t length );
void usart_send_str_len_P( const char* s_P, size_t length );
void usart_send_str( const char* s );
void usart_send_str_P( const char* s_P );

#endif /* USART_H_ */