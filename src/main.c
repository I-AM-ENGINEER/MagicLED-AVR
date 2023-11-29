#define F_CPU	16000000UL

#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

#include "config.h"
#include "light_ws2812.h"

int16_t ADC_get_DC( void );
int16_t ADC_get_short_amp( void );
int16_t ADC_get_long_amp( void );
uint32_t get_tick( void );
struct cRGB hsv2rgb(uint8_t h, uint8_t s, uint8_t v);
struct cRGB get_pixel_color(uint16_t pixelN);
void color_update( void );

volatile uint32_t tick;
volatile int32_t adc_dc_value;			// DC part of signal
volatile int32_t adc_long_volume;		// For music volume adjust
volatile int32_t adc_short_volume;		// For music change detection
volatile int16_t adc_last_value;
uint8_t hsv_color;
uint8_t hsv_brightness;

const struct cRGB idle_color = {
	.r = LED_IDLE_COLOR_R,
	.g = LED_IDLE_COLOR_G,
	.b = LED_IDLE_COLOR_B
};

struct cRGB leds_buffer[LED_COUNT];

ISR(TIMER0_COMPA_vect){	
	tick++;
}

ISR(TIMER1_COMPB_vect, ISR_NOBLOCK){}	// Hardware start ADC, no more

ISR(ADC_vect){
	adc_last_value = ADC;
	int16_t current_volume = abs(ADC_get_DC() - adc_last_value);

	adc_dc_value += adc_last_value - ADC_get_DC();
	adc_short_volume += current_volume - ADC_get_short_amp();
	adc_long_volume += current_volume - ADC_get_long_amp();
}

int16_t ADC_get_DC( void ){
	return (int16_t)(adc_dc_value >> ADC_DC_FILTER_ORDER);
}

int16_t ADC_get_short_amp( void ){
	return (int16_t)(adc_short_volume >> ADC_SHORT_AMP_FILTER_ORDER);
}

int16_t ADC_get_long_amp( void ){
	return (int16_t)(adc_long_volume >> ADC_LONG_AMP_FILTER_ORDER);
}

uint32_t get_tick( void ){
	return tick;
}

struct cRGB hsv2rgb(uint8_t h, uint8_t s, uint8_t v) {    
	struct cRGB rgb;

	uint8_t tmp_val = ((uint16_t)v * (uint16_t)s) / 256; // Should be /255, but /256 faster
    uint8_t vmin = (uint16_t)v - tmp_val;
    uint8_t a = tmp_val * ((uint16_t)h * 24 / 17 % 60) / 60;
    uint8_t vinc = vmin + a;
    uint8_t vdec = v - a;
	uint8_t zone = ((24 * (uint16_t)h / 17) / 60) % 6;

    switch (zone) {
		case 0: rgb.r = v; rgb.g = vinc; rgb.b = vmin; break;
		case 1: rgb.r = vdec; rgb.g = v; rgb.b = vmin; break;
		case 2: rgb.r = vmin; rgb.g = v; rgb.b = vinc; break;
		case 3: rgb.r = vmin; rgb.g = vdec; rgb.b = v; break;
		case 4: rgb.r = vinc; rgb.g = vmin; rgb.b = v; break;
		case 5: rgb.r = v; rgb.g = vmin; rgb.b = vdec; break;
    }
    return rgb;
}

struct cRGB get_pixel_color(uint16_t pixelN){
	uint8_t pixel_color = hsv_color + pixelN*LED_RAINBOW_PIXEL_DIF;
	
	struct cRGB RGB = hsv2rgb(pixel_color, 255, hsv_brightness);

	RGB.g >>=1;	// I hate green, fuck green

	if(RGB.r < LED_IDLE_COLOR_R){
		RGB.r = LED_IDLE_COLOR_R;
	}
	if(RGB.g < LED_IDLE_COLOR_G){
		RGB.g = LED_IDLE_COLOR_G;
	}
	if(RGB.b < LED_IDLE_COLOR_B){
		RGB.b = LED_IDLE_COLOR_B;
	}
	return RGB;
}

void color_update( void ){
	hsv_color = (uint8_t)((get_tick() % (uint32_t)LED_RAINBOW_PERIOD) * 255UL / (uint32_t)LED_RAINBOW_PERIOD);
	uint16_t brightness = (uint16_t)((float)ADC_get_long_amp() * LED_VOLUME_BRIGHTNES_K);
	if(brightness > 255){
		brightness = 255;
	}
	hsv_brightness = brightness;
}

void strip_write( void ){
	ws2812_setleds(leds_buffer, LED_COUNT);
	tick += ((LED_COUNT*24+ws2812_resettime)/1000); // Compensation send time
}

void strip_fill( void ){
	#if LED_CENTERED
	const uint16_t led_count = LED_COUNT/2;
	#else
	const uint16_t led_count = LED_COUNT;
	#endif

	uint16_t volume_level_min = ADC_get_long_amp() * LED_MIN_VALUE_K;
	uint16_t volume_level_max = ADC_get_long_amp() * LED_MAX_VALUE_K;
	uint16_t volume_current = ADC_get_short_amp();
	uint16_t led_count_active;

	if(volume_current < volume_level_min){
		led_count_active = 0;
	}else if(volume_current > volume_level_max){
		led_count_active = led_count;
	}else{
		led_count_active = (uint32_t)led_count * (uint32_t)(volume_current - volume_level_min) / (uint32_t)(volume_level_max - volume_level_min);
	}

	#if LED_CENTERED
	for(uint16_t i = 0; i < (led_count - led_count_active); i++){
		leds_buffer[i] = idle_color;
		leds_buffer[LED_COUNT - i - 1] = idle_color;
	}
	for(uint16_t i = (led_count - led_count_active); i < led_count; i++){
		struct cRGB pixel_color = get_pixel_color(i);
		leds_buffer[i] = pixel_color;
		leds_buffer[LED_COUNT - i - 1] = pixel_color;
	}
	#else
	for(uint16_t i = 0; i < led_count_active; i++){
		leds_buffer[i] = idle_color;
	}
	for(uint16_t i = led_count_active; i < led_count; i++){
		leds_buffer[i] = get_pixel_color(i);
	}
	#endif
}

int main(){
	// TIM0 - millis
	TCCR0A = (1 << WGM01); // CTC mode
	TCCR0B = (1 << CS01) | (1 << CS00); // prescaller 64
	OCR0A  = F_CPU/64/500 - 1; // One tick in milsecond
	TIMSK0 = (1 << OCIE0A);

	// TIM1 - ADC reading interval
	TCCR1B = (1 << WGM12) | (1 << CS12);	// Prescaller 256, CTC
	uint16_t prescaller = F_CPU/256/1000; 	// 1kHz ADC reading 
	OCR1AH = (uint8_t)((prescaller-1) >> 8);
	OCR1AL = (uint8_t)(prescaller-1);
	TIMSK1 = (1 << OCIE1B);

	// ADC init
	ADMUX = (0b0111 << MUX0) | (0b11 << REFS0); //A7 pin
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (0b111 << ADPS0) | (1 << ADATE); // prescaller 128, yes, fuck f/2
	ADCSRB = (0b101 << ADTS0);

	DDRC = (1 << PC0) | (1 << PC1);

	sei();

	while(1) {
		static bool sleep_mode = false;
		static uint32_t timestamp;
		static uint32_t sleep_timestamp;

		#if !SLEEP_ENABLE
		sleep_mode = false;
		sleep_timestamp == 0;
		#endif
		
		if(sleep_mode){
			if(ADC_get_long_amp() > ADC_THRESHOLD_WAKE){
				if(sleep_timestamp == 0){
					sleep_timestamp = get_tick();
				}else if((get_tick() - sleep_timestamp) > (WAKEUP_TIME * 1000UL)){
					sleep_mode = false;
					sleep_timestamp = 0;
				}
			}else{
				sleep_timestamp = 0;
			}
		}else{
			color_update();
			strip_fill();

			if(ADC_get_long_amp() < ADC_THRESHOLD_SLEEP){
				if(sleep_timestamp == 0){
					sleep_timestamp = get_tick();
				}else if((get_tick() - sleep_timestamp) > (SLEEP_TIME * 1000UL)){
					sleep_mode = true;
					sleep_timestamp = 0;
				}
			}else{
				sleep_timestamp = 0;
			}
		}
		
		
		strip_write();
		PORTC &= ~(1 << PC1);
		while((get_tick() - timestamp) < LED_UPDATE_PERIOD);
		timestamp = get_tick();
		PORTC |= (1 << PC1);
	}
}
