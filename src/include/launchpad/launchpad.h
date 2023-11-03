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



static inline uint32_t launchpad_get_led_rgb(launchpad_t* launchpad,uint8_t x,uint8_t y){
	uint8_t idx=x+y*9;
	return ((launchpad->leds+idx)->r<<16)|((launchpad->leds+idx)->g<<8)|(launchpad->leds+idx)->b;
}



static inline void launchpad_set_led(launchpad_t* launchpad,uint8_t x,uint8_t y,uint8_t r,uint8_t g,uint8_t b){
	uint8_t idx=x+y*9;
	(launchpad->leds+idx)->r=r;
	(launchpad->leds+idx)->g=g;
	(launchpad->leds+idx)->b=b;
	(launchpad->leds+idx)->modified=1;
}



static inline void launchpad_set_led_rgb(launchpad_t* launchpad,uint8_t x,uint8_t y,uint32_t color){
	launchpad_set_led(launchpad,x,y,color>>16,color>>8,color);
}



_Bool launchpad_open(launchpad_t* out);



void launchpad_close(launchpad_t* launchpad);



void launchpad_update_leds(launchpad_t* launchpad);



_Bool launchpad_process_events(launchpad_t* launchpad,launchpad_button_event_t* out);



void launchpad_set_led_hsl(launchpad_t* launchpad,uint8_t x,uint8_t y,uint8_t h,uint8_t s,uint8_t l);



#endif
