#ifndef __PROJECT_CONFIG_H
#define __PROJECT_CONFIG_H

// A7 on arduino nano, can be any analog pin
#define ADC_PIN_NUM                     7

// 0 - 1.1V, input signal should be 0.5-0.6V DC component
// use this settings for get signal from headphones/speakers line
// 1 - AREF (3.3V), input signal should be 1-2V DC component
// use this settings for microphone, like MAX9814 or MAX4466 without external componets
#define ADC_REFERENCE_VOLTAGE           1

#define LED_PIN							2				// PIN of atmega ws2812 signal
#define LED_PORT						D				// PORT of atmega ws2812 signal
#define LED_COUNT						(122)			// Count led in sequential connection
#define LED_CENTERED					(true)			// Start animation from center of led strip

#define LED_VOLUME_BRIGHTNES_K			(0.6f)			// 0.5...10.0 More volume - more 
#define LED_RAINBOW_PIXEL_DIF			(2)				// Set 0 for disable rainbow
#define LED_RAINBOW_PERIOD				(8192)			// (ms), maybe should be power of 2, but not must
// Color, that will display inactive pixel
#define LED_IDLE_COLOR_R				(30)			// Red   channel default
#define LED_IDLE_COLOR_G				(0)				// Green channel default
#define LED_IDLE_COLOR_B				(60)			// Blue  channel default
#define LED_MAX_BRIGHTNESS              (255)           // 10...255, drop this value down, if shitty power supply

#define ALLOW_OVERFLOW                  (false)         // If music change volume, sometimes strip full brigth or dark on some soconds, this option prevent this

// Sleep and wakeup
#define SLEEP_ENABLE 					(true)			// Automatic turn light off after music shut
#if SLEEP_ENABLE
#define SLEEP_TIME						(300)			// (s) After this time led strip will turn off if no music
#define ADC_THRESHOLD_SLEEP				(5)				// (5...50) Volume below this value will be interpreted as silent (up if shit audiocable or amplifier)
#define ADC_THRESHOLD_WAKE				(10)			// If volume greater, wakeup from sleep (up if random wakeup)
#define WAKEUP_TIME						(5)				// (s) Time for wakeup from sleep (for dont wakeup from system sound)
#endif // SLEEP_ENABLE

#define ADC_LONG_AMP_FILTER_ORDER		(10)			// 6...14, bigger value - faster reaction on volume change
#define ADC_SHORT_AMP_FILTER_ORDER		(6)				// 4...10, bigger value - softer led rise/fall

#define LED_MIN_VALUE_K					(0.5f)			// 0...1.0, offset zero from relative from volume
#define LED_MAX_VALUE_K					(1.3f)			// 1.0...2.0, max led value, relative from volume

// Cutoff high freq from input signal (noise reduction)
// 6: -20dB 1kHz; 0dB 100Hz
// 5: -20dB 2kHz; 0dB 200Hz
// 4: -20dB 4kHz; 0dB 400Hz
// 3: -20dB 9kHz; 0dB 800Hz
// 2: -20dB 20kHz; 0dB 1600Hz
#define LOWFREQ_FILTER_ORDER            (4)

// For lib

#define ws2812_port LED_PORT     // Data port 
#define ws2812_pin  LED_PIN     // Data out pin

#define ws2812_resettime  20 

#endif //  __PROJECT_CONFIG_H
