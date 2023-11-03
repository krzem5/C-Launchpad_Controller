#include <launchpad/launchpad.h>
#include <stdio.h>
#include <stdlib.h>



static uint8_t mode;
static uint8_t bar_values[8];
static uint8_t selected_bar_index;



static void _redraw_bar(launchpad_t* launchpad,uint8_t index){
	uint8_t value=bar_values[index];
	for (uint8_t i=0;i<8;i++){
		launchpad_set_led_hsl(launchpad,index,i,index<<5,(index==selected_bar_index?175:255),(value==(i<<1)+1?50:(value>=(i<<1)+2?255:0)));
	}
}



static void _select_button(launchpad_t* launchpad,uint8_t selection_index){
	mode=selection_index;
	for (uint8_t i=0;i<4;i++){
		launchpad_set_led_rgb(launchpad,i,8,(mode==2?0xffffff:0x000000));
	}
	for (uint8_t i=0;i<3;i++){
		launchpad_set_led_rgb(launchpad,5+i,8,(i==selection_index?0x00ff00:0xffffff));
	}
	if (!mode){
		for (uint8_t i=0;i<8;i++){
			for (uint8_t j=0;j<8;j++){
				launchpad_set_led_hsl(launchpad,i,j,(i+j*8)<<2,255,255);
			}
		}
		return;
	}
	if (mode==1){
		for (uint8_t i=0;i<8;i++){
			for (uint8_t j=0;j<8;j++){
				launchpad_set_led_rgb(launchpad,i,j,0x000000);
			}
		}
		return;
	}
	selected_bar_index=0xff;
	bar_values[0]=1;
	bar_values[1]=3;
	bar_values[2]=6;
	bar_values[3]=2;
	bar_values[4]=10;
	bar_values[5]=12;
	bar_values[6]=7;
	bar_values[7]=16;
	for (uint8_t i=0;i<8;i++){
		_redraw_bar(launchpad,i);
	}
}



int main(void){
	launchpad_t launchpad;
	if (!launchpad_open(&launchpad)){
		return 1;
	}
	launchpad_set_led_rgb(&launchpad,0,0,0xef0000);
	launchpad_set_led_rgb(&launchpad,0,1,0x2f0000);
	launchpad_set_led_rgb(&launchpad,1,0,0x00ef00);
	launchpad_set_led_rgb(&launchpad,1,1,0x00ef00);
	launchpad_set_led_rgb(&launchpad,1,2,0x002f00);
	launchpad_set_led_rgb(&launchpad,2,0,0x0000ef);
	launchpad_set_led_rgb(&launchpad,2,1,0x0000ef);
	launchpad_set_led_rgb(&launchpad,2,2,0x0000ef);
	launchpad_set_led_rgb(&launchpad,2,3,0x00002f);
	launchpad_set_led_rgb(&launchpad,8,0,0xffffff);
	_select_button(&launchpad,2);
	launchpad_update_leds(&launchpad);
	while (1){
		launchpad_button_event_t event;
		if (!launchpad_process_events(&launchpad,&event)||!event.is_pressed){
			continue;
		}
		if (event.x==8){
			if (!event.y){
				break;
			}
			continue;
		}
		if (event.y==8){
			if (event.x>4&&event.x<8){
				_select_button(&launchpad,event.x-5);
				goto _redraw_display;
			}
			if (mode==2){
				if (!event.x){
					if (selected_bar_index!=0xff&&bar_values[selected_bar_index]<16){
						bar_values[selected_bar_index]++;
						_redraw_bar(&launchpad,selected_bar_index);
						goto _redraw_display;
					}
				}
				else if (event.x==1){
					if (selected_bar_index!=0xff&&bar_values[selected_bar_index]){
						bar_values[selected_bar_index]--;
						_redraw_bar(&launchpad,selected_bar_index);
						goto _redraw_display;
					}
				}
				else if (event.x==2){
					if (selected_bar_index!=0xff){
						uint8_t old_selected_bar_index=selected_bar_index;
						selected_bar_index=(selected_bar_index-1)&7;
						_redraw_bar(&launchpad,old_selected_bar_index);
						_redraw_bar(&launchpad,selected_bar_index);
						goto _redraw_display;
					}
				}
				else if (event.x==3){
					if (selected_bar_index!=0xff){
						uint8_t old_selected_bar_index=selected_bar_index;
						selected_bar_index=(selected_bar_index+1)&7;
						_redraw_bar(&launchpad,old_selected_bar_index);
						_redraw_bar(&launchpad,selected_bar_index);
						goto _redraw_display;
					}
				}
			}
			continue;
		}
		if (!mode){
			if (launchpad_get_led_rgb(&launchpad,event.x,event.y)){
				launchpad_set_led_rgb(&launchpad,event.x,event.y,0x000000);
			}
			else{
				launchpad_set_led_hsl(&launchpad,event.x,event.y,(event.x+event.y*8)<<2,255,255);
			}
		}
		else if (mode==1){
			if (launchpad_get_led_rgb(&launchpad,event.x,event.y)){
				launchpad_set_led_rgb(&launchpad,event.x,event.y,0x000000);
			}
			else{
				launchpad_set_led(&launchpad,event.x,event.y,rand()&255,rand()&255,rand()&255);
			}
		}
		else{
			uint8_t old_selected_bar_index=selected_bar_index;
			selected_bar_index=(selected_bar_index==event.x?0xff:event.x);
			if (old_selected_bar_index!=0xff){
				_redraw_bar(&launchpad,old_selected_bar_index);
			}
			if (old_selected_bar_index!=event.x){
				_redraw_bar(&launchpad,event.x);
			}
		}
_redraw_display:
		launchpad_update_leds(&launchpad);
	}
	launchpad_close(&launchpad);
	return 0;
}
