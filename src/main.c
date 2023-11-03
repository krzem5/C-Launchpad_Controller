#include <launchpad/launchpad.h>
#include <stdio.h>
#include <stdlib.h>



int main(void){
	launchpad_t launchpad;
	if (!launchpad_open(&launchpad)){
		return 1;
	}
	launchpad_set_led(&launchpad,0,0,0xff0000);
	launchpad_set_led(&launchpad,1,0,0x00ff00);
	launchpad_set_led(&launchpad,2,0,0x0000ff);
	launchpad_set_led(&launchpad,8,0,0xffffff);
	launchpad_update_leds(&launchpad);
	while (1){
		launchpad_button_event_t event;
		if (!launchpad_process_events(&launchpad,&event)){
			continue;
		}
		printf("%u,%u: %u\n",event.x,event.y,event.is_pressed);
		if (event.x==8&&!event.y){
			if (event.is_pressed){
				break;
			}
			else{
				continue;
			}
		}
		launchpad_set_led(&launchpad,event.x,event.y,(event.is_pressed?((rand()&255)<<16)|((rand()&255)<<8)|(rand()&255):0x000000));
		launchpad_update_leds(&launchpad);
	}
	launchpad_close(&launchpad);
	return 0;
}
