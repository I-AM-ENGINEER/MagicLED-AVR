#ifndef __USART_BAUDRATES_CFG
#define __USART_BAUDRATES_CFG

#if F_CPU == 1000000UL
typedef enum {
	USART_baudrate_1200 	= 103,	// error: 0.2
	USART_baudrate_2400 	= 51,	// error: 0.2
	USART_baudrate_4800 	= 25,	// error: 0.2
	USART_baudrate_9600 	= 12,	// error: 0.2
} USART_baudrate_t;

#elif F_CPU == 1843200UL

typedef enum {
	USART_baudrate_1200 	= 191,	// error: 0.0
	USART_baudrate_2400 	= 95,	// error: 0.0
	USART_baudrate_4800 	= 47,	// error: 0.0
	USART_baudrate_9600 	= 23,	// error: 0.0
	USART_baudrate_19200 	= 11,	// error: 0.0
	USART_baudrate_38400 	= 5,	// error: 0.0
	USART_baudrate_57600 	= 3,	// error: 0.0
	USART_baudrate_115200 	= 1,	// error: 0.0
	USART_baudrate_230400 	= 0,	// error: 0.0
} USART_baudrate_t;

#elif F_CPU == 2000000UL

typedef enum {
	USART_baudrate_1200 	= 207,	// error: 0.2
	USART_baudrate_2400 	= 103,	// error: 0.2
	USART_baudrate_4800 	= 51,	// error: 0.2
	USART_baudrate_9600 	= 25,	// error: 0.2
	USART_baudrate_19200 	= 12,	// error: 0.2
	USART_baudrate_250000 	= 0,	// error: 0.0
} USART_baudrate_t;

#elif F_CPU == 3686400UL

typedef enum {
	USART_baudrate_1200 	= 383,	// error: 0.0
	USART_baudrate_2400 	= 191,	// error: 0.0
	USART_baudrate_4800 	= 95,	// error: 0.0
	USART_baudrate_9600 	= 47,	// error: 0.0
	USART_baudrate_19200 	= 23,	// error: 0.0
	USART_baudrate_38400 	= 11,	// error: 0.0
	USART_baudrate_57600 	= 7,	// error: 0.0
	USART_baudrate_115200 	= 3,	// error: 0.0
	USART_baudrate_230400 	= 1,	// error: 0.0
	USART_baudrate_460800 	= 0,	// error: 0.0
} USART_baudrate_t;

#elif F_CPU == 4000000UL

typedef enum {
	USART_baudrate_1200 	= 416,	// error: -0.1
	USART_baudrate_2400 	= 207,	// error: 0.2
	USART_baudrate_4800 	= 103,	// error: 0.2
	USART_baudrate_9600 	= 51,	// error: 0.2
	USART_baudrate_19200 	= 25,	// error: 0.2
	USART_baudrate_38400 	= 12,	// error: 0.2
	USART_baudrate_57600 	= 8,	// error: -3.5
	USART_baudrate_250000 	= 1,	// error: 0.0
	USART_baudrate_500000 	= 0,	// error: 0.0
} USART_baudrate_t;

#elif F_CPU == 7372800UL

typedef enum {
	USART_baudrate_1200 	= 767,	// error: 0.0
	USART_baudrate_2400 	= 383,	// error: 0.0
	USART_baudrate_4800 	= 191,	// error: 0.0
	USART_baudrate_9600 	= 95,	// error: 0.0
	USART_baudrate_19200 	= 47,	// error: 0.0
	USART_baudrate_38400 	= 23,	// error: 0.0
	USART_baudrate_57600 	= 15,	// error: 0.0
	USART_baudrate_115200 	= 7,	// error: 0.0
	USART_baudrate_230400 	= 3,	// error: 0.0
	USART_baudrate_460800 	= 1,	// error: 0.0
	USART_baudrate_921600 	= 0,	// error: 0.0
} USART_baudrate_t;

#elif F_CPU == 8000000UL

typedef enum {
	USART_baudrate_1200 	= 832,	// error: 0.0
	USART_baudrate_2400 	= 416,	// error: -0.1
	USART_baudrate_4800 	= 207,	// error: 0.2
	USART_baudrate_9600 	= 103,	// error: 0.2
	USART_baudrate_19200 	= 51,	// error: 0.2
	USART_baudrate_38400 	= 25,	// error: 0.2
	USART_baudrate_57600 	= 16,	// error: 2.1
	USART_baudrate_115200 	= 8,	// error: -3.5
	USART_baudrate_250000 	= 3,	// error: 0.0
	USART_baudrate_500000 	= 1,	// error: 0.0
	USART_baudrate_1000000 	= 0,	// error: 0.0
} USART_baudrate_t;

#elif F_CPU == 11059200UL

typedef enum {
	USART_baudrate_1200 	= 1151,	// error: 0.0
	USART_baudrate_2400 	= 575,	// error: 0.0
	USART_baudrate_4800 	= 287,	// error: 0.0
	USART_baudrate_9600 	= 143,	// error: 0.0
	USART_baudrate_19200 	= 71,	// error: 0.0
	USART_baudrate_38400 	= 35,	// error: 0.0
	USART_baudrate_57600 	= 23,	// error: 0.0
	USART_baudrate_115200 	= 11,	// error: 0.0
	USART_baudrate_230400 	= 5,	// error: 0.0
	USART_baudrate_460800 	= 2,	// error: 0.0
} USART_baudrate_t;

#elif F_CPU == 14745600UL

typedef enum {
	USART_baudrate_1200 	= 1535,	// error: 0.0
	USART_baudrate_2400 	= 767,	// error: 0.0
	USART_baudrate_4800 	= 383,	// error: 0.0
	USART_baudrate_9600 	= 191,	// error: 0.0
	USART_baudrate_19200 	= 95,	// error: 0.0
	USART_baudrate_38400 	= 47,	// error: 0.0
	USART_baudrate_57600 	= 31,	// error: 0.0
	USART_baudrate_115200 	= 15,	// error: 0.0
	USART_baudrate_230400 	= 7,	// error: 0.0
	USART_baudrate_460800 	= 3,	// error: 0.0
	USART_baudrate_921600 	= 1,	// error: 0.0
	USART_baudrate_1843200	= 0,	// error: 0.0
} USART_baudrate_t;

#elif F_CPU == 16000000UL

typedef enum {
	USART_baudrate_1200 	= 1666,	// error: -0.0
	USART_baudrate_2400 	= 832,	// error: 0.0
	USART_baudrate_4800 	= 416,	// error: -0.1
	USART_baudrate_9600 	= 207,	// error: 0.2
	USART_baudrate_19200 	= 103,	// error: 0.2
	USART_baudrate_38400 	= 51,	// error: 0.2
	USART_baudrate_57600 	= 34,	// error: -0.8
	USART_baudrate_115200 	= 16,	// error: 2.1
	USART_baudrate_230400 	= 8,	// error: -3.5
	USART_baudrate_250000 	= 7,	// error: 0.0
	USART_baudrate_500000 	= 3,	// error: 0.0
	USART_baudrate_1000000 	= 1,	// error: 0.0
	USART_baudrate_2000000 	= 0,	// error: 0.0
} USART_baudrate_t;

#elif F_CPU == 20000000UL

typedef enum {
	USART_baudrate_1200 	= 2082,	// error: 0.0
	USART_baudrate_2400 	= 1041,	// error: -0.0
	USART_baudrate_4800 	= 520,	// error: -0.0
	USART_baudrate_9600 	= 259,	// error: 0.2
	USART_baudrate_19200 	= 129,	// error: 0.2
	USART_baudrate_38400 	= 64,	// error: 0.2
	USART_baudrate_57600 	= 42,	// error: 0.9
	USART_baudrate_115200 	= 21,	// error: -1.4
	USART_baudrate_230400 	= 10,	// error: -1.4
	USART_baudrate_250000 	= 9,	// error: 0.0
	USART_baudrate_500000 	= 4,	// error: 0.0
	USART_baudrate_2500000 	= 0,	// error: 0.0
} USART_baudrate_t;

#else
#error "UART SPEED ISNT SUPPORT OR F_CPU UNDEFINED"
#endif

#endif /* __USART_BAUDRATES_CFG */
