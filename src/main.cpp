#define LED_PIN							(2)				// Hardware PIN of arduino
#define LED_COUNT						(122)			// Count led in sequential connection
#define LED_CENTERED					(true)			// Start animation from center of led strip
#define LED_ORDER						(ORDER_GRB)	

#define LED_VOLUME_BRIGHTNES_K			(2.5f)			// 0.5...10.0 More volume - more 
#define LED_RAINBOW_PIXEL_DIF			(2)
#define LED_RAINBOW_PERIOD				(32768)			// (ms), maybe should be power of 2, but not must
#define LED_IDLE_COLOR					mRGB(30, 0, 60)	// Color, that will display inactive pixel

// Sleep and wakeup
#define SLEEP_ENABLE 					(true)			// Automatic turn light off after music shut
#if SLEEP_ENABLE
#define SLEEP_TIME						(300)			// (s) After this time led strip will turn off if no music
#define ADC_THRESHOLD_SLEEP				(5)				// (5...50) Volume below this value will be interpreted as silent (up if shit audiocable or amplifier)
#define ADC_THRESHOLD_WAKE				(10)			// If volume greater, wakeup from sleep (up if random wakeup)
#define WAKEUP_TIME						(5)				// (s) Time for wakeup from sleep (for dont wakeup from system sound)
#endif // SLEEP_ENABLE

#define ADC_DC_FILTER_ORDER				(10)			// 8...16 DC signal value, bigger value - more stable, but longer startup
#define ADC_LONG_AMP_FILTER_ORDER		(9)				// 6...12, bigger value - faster reaction on volume change
#define ADC_SHORT_AMP_FILTER_ORDER		(6)				// 4...10, bigger value - softer led rise/fall

#define LED_MIN_VALUE_K					(0.5f)			// 0...1.0, offset zero from relative from volume
#define LED_MAX_VALUE_K					(1.3f)			// 1.0...2.0, max led value, relative from volume

#define UPDATE_PERIOD					(50)			// (ms), led update period

//#define DEBUG

#define COLOR_DEBTH 3 // Need for microLED lib

#include <Arduino.h>
#include <microLED.h>
#include <stdarg.h>

#ifdef DEBUG
#define DBG_LOG(TAG, ...) {Serial.print(TAG); Serial.print(": "); Serial.println(__VA_ARGS__);}
#else
#define DBG_LOG(...)
#endif // DEBUG

microLED< LED_COUNT, LED_PIN, -1, LED_WS2812, LED_ORDER, CLI_HIGH, SAVE_MILLIS > strip;

volatile int32_t adc_dc_value;			// DC part of signal
volatile int32_t adc_long_volume;		// For music volume adjust
volatile int32_t adc_short_volume;		// For music change detection
volatile int16_t adc_last_value;
uint8_t current_color;
uint8_t current_brightness;

inline int16_t ADC_get_DC( void ){
	return (int16_t)(adc_dc_value >> ADC_DC_FILTER_ORDER);
}

inline int16_t ADC_get_short_amp( void ){
	return (int16_t)(adc_short_volume >> ADC_SHORT_AMP_FILTER_ORDER);
}

inline int16_t ADC_get_long_amp( void ){
	return (int16_t)(adc_long_volume >> ADC_LONG_AMP_FILTER_ORDER);
}

ISR(TIMER1_COMPB_vect, ISR_NOBLOCK){}	// Hardware start ADC, no more

ISR(ADC_vect){
	adc_last_value = ADC;
	int16_t current_volume = abs(ADC_get_DC() - adc_last_value);

	adc_dc_value += adc_last_value - ADC_get_DC();
	adc_short_volume += current_volume - ADC_get_short_amp();
	adc_long_volume += current_volume - ADC_get_long_amp();
}

void current_color_update( void ){
	current_color = (uint8_t)((millis() % (uint32_t)LED_RAINBOW_PERIOD) * 255UL / (uint32_t)LED_RAINBOW_PERIOD);
	uint16_t brightness = (uint16_t)((float)ADC_get_long_amp() * LED_VOLUME_BRIGHTNES_K);
	if(brightness > 255){
		brightness = 255;
	}
	current_brightness = brightness;
}

mData get_pixel_color(uint16_t pixelN){
	uint8_t color = current_color + pixelN*LED_RAINBOW_PIXEL_DIF;
	
	mData RGB = mHSVfast(color, 255, current_brightness);

	RGB.g >>=1;	// I hate green, fuck green

	if(RGB.r < LED_IDLE_COLOR.r){
		RGB.r = LED_IDLE_COLOR.r;
	}
	if(RGB.g < LED_IDLE_COLOR.g){
		RGB.g = LED_IDLE_COLOR.g;
	}
	if(RGB.b < LED_IDLE_COLOR.b){
		RGB.b = LED_IDLE_COLOR.b;
	}

	return RGB;
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
	uint16_t led_on;	

	if(volume_current < volume_level_min){
		led_on = 0;
	}else if(volume_current > volume_level_max){
		led_on = led_count;
	}else{
		led_on = (uint32_t)led_count * (uint32_t)(volume_current - volume_level_min) / (uint32_t)(volume_level_max - volume_level_min);
	}

	#if LED_CENTERED
	for(uint16_t i = 0; i < (led_count - led_on); i++){
		strip.set(i, LED_IDLE_COLOR);
		strip.set(LED_COUNT - i - 1, LED_IDLE_COLOR);
	}
	for(uint16_t i = (led_count - led_on); i < led_count; i++){
		strip.set(i, get_pixel_color(i));
		strip.set(LED_COUNT - i - 1, get_pixel_color(i));
	}
	#else
	for(uint16_t i = 0; i < led_on; i++){
		strip.set(i, get_pixel_color(i));
	}
	for(uint16_t i = led_on; i < led_count; i++){
		strip.set(i, LED_IDLE_COLOR);
	}
	#endif
}

void setup() {
	#ifdef DEBUG
	Serial.begin(1000000);
	#endif // DEBUG
	
	pinMode(PIN_A0, OUTPUT);
	digitalWrite(PIN_A0, LOW);

	strip.begin();
	strip.setBrightness(255);

	TCCR1B = (1 << WGM12) | (1 << CS12);	// Prescaller 256, CTC
	uint16_t prescaller = F_CPU/256/400; 	// 400 Hz 
	OCR1AH = (uint8_t)((prescaller-1) << 8);
	OCR1AL = (uint8_t)(prescaller-1);
	TIMSK1 = (1 << OCIE1B);

	// ADC init
	ADMUX = (0b0111 << MUX0) | (0b11 << REFS0); //A7 pin
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (0b111 << ADPS0) | (1 << ADATE); // prescaller 128, yes, fuck f/2
	ADCSRB = (0b101 << ADTS0);
	sei();
}

void loop() {
	static uint32_t last_timestamp;
	static bool sleep_mode = false;
	static uint32_t sleep_timestamp;

	#if !SLEEP_ENABLE
	sleep_mode = false;
	#endif

	if(sleep_mode){
		strip.clear();

		if(ADC_get_long_amp() > ADC_THRESHOLD_WAKE){
			if(sleep_timestamp == 0){
				sleep_timestamp = millis();
			}else if((millis() - sleep_timestamp) > (WAKEUP_TIME * 1000UL)){
				sleep_mode = false;
				sleep_timestamp = 0;
			}
		}else{
			sleep_timestamp = 0;
		}
	}else{
		current_color_update();
		strip_fill();

		if(ADC_get_long_amp() < ADC_THRESHOLD_SLEEP){
			if(sleep_timestamp == 0){
				sleep_timestamp = millis();
			}else if((millis() - sleep_timestamp) > (SLEEP_TIME * 1000UL)){
				sleep_mode = true;
				sleep_timestamp = 0;
			}
		}else{
			sleep_timestamp = 0;
		}
	}
	strip.show();
	while ((millis() - last_timestamp) < UPDATE_PERIOD);
	last_timestamp = millis();
}
