#ifndef _LAUNCHPAD_LAUNCHPAD_H_
#define _LAUNCHPAD_LAUNCHPAD_H_ 1
#include <stdint.h>



typedef struct _LAUNCHPAD_LED_COLOR{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t modified;
} launchpad_led_color_t;



typedef struct _LAUNCHPAD{
	launchpad_led_color_t leds[81];
	void* _midi_in;
	void* _midi_out;
} launchpad_t;



typedef struct _LAUNCHPAD_BUTTON_EVENT{
	uint8_t x;
	uint8_t y;
	_Bool is_pressed;
} launchpad_button_event_t;



static inline void launchpad_set_led(launchpad_t* launchpad,uint8_t x,uint8_t y,uint32_t color){
	uint8_t idx=x+y*9;
	(launchpad->leds+idx)->r=color>>16;
	(launchpad->leds+idx)->g=color>>8;
	(launchpad->leds+idx)->b=color;
	(launchpad->leds+idx)->modified=1;
}



_Bool launchpad_open(launchpad_t* out);



void launchpad_close(launchpad_t* launchpad);



void launchpad_update_leds(launchpad_t* launchpad);



_Bool launchpad_process_events(launchpad_t* launchpad,launchpad_button_event_t* out);



#endif
