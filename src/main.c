// grid=hue, sidebar=saturation, sidebar_button=(sidebar=brightness)
// grid=hue, sidebar=brightness, sidebar_button=(sidebar=saturation)
// grid=brightness, sidebar=saturation[selected=0%]
#include <launchpad/launchpad.h>
#include <stdio.h>
#include <stdlib.h>



static const uint32_t sidebar_colors[7]={
	0xff00ff,
	0x0000ff,
	0x00ffff,
	0x00ff00,
	0xffff00,
	0xff0000,
	0xffffff,
};

static uint8_t mode;
static uint8_t bar_values[8];
static uint8_t selected_bar_index;
static uint32_t selected_sidebar_color;



static void _redraw_bar(launchpad_t* launchpad,uint8_t index){
	uint8_t value=bar_values[index];
	for (uint8_t i=0;i<8;i++){
		launchpad_set_led_hsl(launchpad,index,i,index<<5,(index==selected_bar_index?150:255),(value==(i<<1)+1?50:(value>=(i<<1)+2?255:0)));
	}
}



static void _select_button(launchpad_t* launchpad,uint8_t selection_index){
	mode=selection_index;
	launchpad_set_led_rgb(launchpad,8,0,0xffffff);
	for (uint8_t i=1;i<8;i++){
		launchpad_set_led_rgb(launchpad,8,i,0x000000);
	}
	for (uint8_t i=0;i<4;i++){
		launchpad_set_led_rgb(launchpad,i,8,(mode==2?0xffffff:0x000000));
	}
	for (uint8_t i=0;i<4;i++){
		launchpad_set_led_rgb(launchpad,4+i,8,(i==selection_index?0x00ff00:0xffffff));
	}
	if (!mode){
		launchpad_set_led_rgb(launchpad,8,7,0xffffff);
		for (uint8_t i=0;i<8;i++){
			for (uint8_t j=0;j<8;j++){
				launchpad_set_led_hsl(launchpad,i,j,(i+j*8)<<2,255,255);
			}
		}
		return;
	}
	if (mode==1){
		for (uint8_t i=0;i<64;i++){
			launchpad_set_led_rgb(launchpad,i&7,i>>3,0x000000);
		}
		return;
	}
	if (mode==2){
		selected_bar_index=0xff;
		for (uint8_t i=0;i<8;i++){
			bar_values[i]=(rand()&15)+1;
			_redraw_bar(launchpad,i);
		}
		return;
	}
	for (uint8_t i=0;i<7;i++){
		launchpad_set_led_rgb(launchpad,8,i+1,sidebar_colors[i]);
	}
	selected_sidebar_color=sidebar_colors[5];
	launchpad_set_led_rgb(launchpad,0,8,selected_sidebar_color);
	for (uint8_t i=0;i<64;i++){
		launchpad_set_led_rgb(launchpad,i&7,i>>3,0x000000);
	}
}



int main(void){
	launchpad_t launchpad;
	if (!launchpad_open(&launchpad)){
		return 1;
	}
	_select_button(&launchpad,3);
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
			if (!mode&&event.y==7){
				for (uint8_t i=0;i<64;i++){
					if (launchpad_get_led_rgb(&launchpad,i&7,i>>3)){
						launchpad_set_led_rgb(&launchpad,i&7,i>>3,0x000000);
					}
					else{
						launchpad_set_led_hsl(&launchpad,i&7,i>>3,i<<2,255,255);
					}
				}
				goto _redraw_display;
			}
			if (mode==3){
				selected_sidebar_color=sidebar_colors[event.y-1];
				launchpad_set_led_rgb(&launchpad,0,8,selected_sidebar_color);
				goto _redraw_display;
			}
			continue;
		}
		if (event.y==8){
			if (event.x>3&&event.x<8){
				_select_button(&launchpad,event.x-4);
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
				launchpad_set_led_hsl(&launchpad,event.x,event.y,(event.x+(event.y<<3))<<2,255,255);
			}
		}
		else if (mode==1){
			if (launchpad_get_led_rgb(&launchpad,event.x,event.y)){
				launchpad_set_led_rgb(&launchpad,event.x,event.y,0x000000);
			}
			else{
				launchpad_set_led_hsl(&launchpad,event.x,event.y,rand()&255,(rand()&31)+224,(rand()&63)+192);
			}
		}
		else if (mode==2){
			uint8_t old_selected_bar_index=selected_bar_index;
			selected_bar_index=(selected_bar_index==event.x?0xff:event.x);
			if (old_selected_bar_index!=0xff){
				_redraw_bar(&launchpad,old_selected_bar_index);
			}
			if (old_selected_bar_index!=event.x){
				_redraw_bar(&launchpad,event.x);
			}
		}
		else{
			launchpad_set_led_rgb(&launchpad,event.x,event.y,(launchpad_get_led_rgb(&launchpad,event.x,event.y)==selected_sidebar_color?0x000000:selected_sidebar_color));
		}
_redraw_display:
		launchpad_update_leds(&launchpad);
	}
	launchpad_close(&launchpad);
	return 0;
}
