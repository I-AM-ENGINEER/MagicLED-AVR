#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "config.h"
#include "light_ws2812.h"
#include "usart.h"


uint16_t ADC_get_short_amp( void );
uint16_t ADC_get_long_amp( void );
uint32_t get_tick( void );
struct cRGB hsv2rgb(uint8_t h, uint8_t s, uint8_t v);
struct cRGB get_pixel_color(uint16_t pixelN);
void color_update( void );

volatile uint32_t tick;
volatile uint16_t adc_dc_value;			// DC part of signal
volatile uint32_t adc_long_volume;		// For music volume adjust
volatile uint32_t adc_short_volume;		// For music change detection

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

EMPTY_INTERRUPT(TIMER1_COMPB_vect);	// Hardware start ADC, no more

ISR(ADC_vect){
	static uint8_t i;
	static int16_t filter_lowfreq;
	static uint8_t dc_filtred;
	static uint16_t current_volume;

	uint8_t adc_value = ADCH;

	uint8_t filter_lowfreq_out = (uint8_t)(filter_lowfreq >> LOWFREQ_FILTER_ORDER);
	filter_lowfreq += (int16_t)adc_value - (int16_t)filter_lowfreq_out;

	// filtering
	current_volume += abs((int16_t)filter_lowfreq_out - (int16_t)dc_filtred);

	if(!--i){
		i = 32;

		dc_filtred = (uint8_t)(adc_dc_value >> 8);
		adc_dc_value += (int16_t)filter_lowfreq_out - (int16_t)dc_filtred;

		// for prevent pixel jitter
		static uint16_t prefilter;
		uint16_t prefilter_filtered = (uint16_t)(prefilter >> 3);
		prefilter += (int16_t)current_volume - (int16_t)prefilter_filtered;

		uint16_t adc_short_filtered = (uint16_t)(adc_short_volume >> ADC_SHORT_AMP_FILTER_ORDER);
		adc_short_volume += (int16_t)prefilter_filtered - (int16_t)adc_short_filtered;

		uint16_t adc_long_filter = (uint16_t)(adc_long_volume >> ADC_LONG_AMP_FILTER_ORDER);
		adc_long_volume += (int16_t)current_volume - (int16_t)adc_long_filter;
		current_volume = 0;
	}
}

inline uint16_t ADC_get_short_amp( void ){
	return (uint16_t)(adc_short_volume >> ADC_SHORT_AMP_FILTER_ORDER);
}

inline uint16_t ADC_get_long_amp( void ){
	return (uint16_t)(adc_long_volume >> ADC_LONG_AMP_FILTER_ORDER);
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
	if(brightness > LED_MAX_BRIGHTNESS){
		brightness = LED_MAX_BRIGHTNESS;
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

	uint16_t volume_level_min = (uint16_t)((float)ADC_get_long_amp() * LED_MIN_VALUE_K);
	uint16_t volume_level_max = (uint16_t)((float)ADC_get_long_amp() * LED_MAX_VALUE_K);
	uint16_t volume_current = ADC_get_short_amp();

	#if !ALLOW_OVERFLOW
	if((volume_current < volume_level_min)){
		adc_long_volume = ((uint32_t)((float)volume_current/LED_MIN_VALUE_K) << ADC_LONG_AMP_FILTER_ORDER);
	}
	if (volume_current > volume_level_max){
		adc_long_volume = ((uint32_t)((float)volume_current/LED_MAX_VALUE_K) << ADC_LONG_AMP_FILTER_ORDER);
	}
	#endif
	
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
	wdt_enable(WDTO_1S);
	//usart_init(USART_baudrate_1000000);

	// TIM0 - millis
	TCCR0A = (1 << WGM01); // CTC mode
	TCCR0B = (1 << CS01) | (1 << CS00); // prescaller 64
	OCR0A  = F_CPU/64/1000 - 1; // One tick in milsecond
	TIMSK0 = (1 << OCIE0A);

	// TIM1 - ADC reading interval
	TCCR1B = (1 << WGM12) | (1 << CS10);	// Prescaller 1, CTC
	uint16_t prescaller = F_CPU/40000; 	// 40KSPs ADC reading
	OCR1AH = (uint8_t)((prescaller-1) >> 8);
	OCR1AL = (uint8_t)(prescaller-1);
	TIMSK1 = (1 << OCIE1B);

	// ADC init
	ADMUX = (ADC_PIN_NUM << MUX0) | (1 << ADLAR); // 8 bit ADC mode (right aligment)
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (0b100 << ADPS0) | (1 << ADATE); // prescaller 16, 1MHz convertion, max speed of ADC, 8.5 bit accuracy
	ADCSRB = (0b101 << ADTS0);

	// AVCC as reference
	#if ADC_REFERENCE_VOLTAGE == 0
	ADMUX |= (1 << REFS1) | (1 << REFS0);
	#endif

	sei();

	while(1) {
		wdt_reset();
		static bool sleep_mode = false;
		static uint32_t sleep_timestamp;

		#if !SLEEP_ENABLE
		sleep_mode = false;
		sleep_timestamp == 0;
		#endif
		
		if(sleep_mode){
			for(uint16_t i = 0; i < LED_COUNT; i++){
				if(leds_buffer[i].r != 0) leds_buffer[i].r--;
				if(leds_buffer[i].g != 0) leds_buffer[i].g--;
				if(leds_buffer[i].b != 0) leds_buffer[i].b--;
			}
			_delay_ms(100);

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
			bool ready = true;
			if(leds_buffer[0].r < LED_IDLE_COLOR_R){
				for(uint16_t i = 0; i < LED_COUNT; i++){
					leds_buffer[i].r++;
				}
				ready = false;
			}
			if(leds_buffer[0].g < LED_IDLE_COLOR_G){
				for(uint16_t i = 0; i < LED_COUNT; i++){
					leds_buffer[i].g++;
				}
				ready = false;
			} 
			if(leds_buffer[0].b < LED_IDLE_COLOR_B){
				for(uint16_t i = 0; i < LED_COUNT; i++){
					leds_buffer[i].b++;
				}
				ready = false;
			} 
			if(ready){
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
			}else{
				_delay_ms(50);
				adc_short_volume = 0;
			}
		}
		strip_write();
	}
}
