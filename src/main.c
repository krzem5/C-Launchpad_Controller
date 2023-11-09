#if 0
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
		launchpad_set_led_rgb(launchpad,8,7,0xffffff);
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
			if (mode==1&&event.y==7){
				for (uint8_t i=0;i<64;i++){
					if (launchpad_get_led_rgb(&launchpad,i&7,i>>3)){
						launchpad_set_led_hsl(&launchpad,i&7,i>>3,rand()&255,(rand()&31)+224,(rand()&63)+192);
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
#else
// grid=hue, sidebar=saturation, sidebar_button=(sidebar=brightness)
// grid=hue, sidebar=brightness, sidebar_button=(sidebar=saturation)
// grid=brightness, sidebar=saturation[selected=0%]
#include <launchpad/launchpad.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



#define INDEX(x,y) (((y)<<3)|(x))



#define TYPE_MASK 0x3f
#define TYPE_NONE 0
#define TYPE_PAWN 1
#define TYPE_ROOK 2
#define TYPE_KNIGHT 3
#define TYPE_BISHOP 4
#define TYPE_QUEEN 5
#define TYPE_KING 6

#define FLAG_MOVED 0x40
#define FLAG_COLOR 0x80

#define FLAG_WHITE 0x00
#define FLAG_BLACK 0x80



typedef uint8_t piece_t;



static const uint32_t chess_piece_colors[]={
	[TYPE_NONE]=0x000000,
	[TYPE_PAWN]=0xff0000,
	[TYPE_ROOK]=0xffff00,
	[TYPE_KNIGHT]=0x00ff00,
	[TYPE_BISHOP]=0x00ffff,
	[TYPE_QUEEN]=0xff00ff,
	[TYPE_KING]=0x0000ff,
};



static piece_t chess_board[64];
static uint8_t chess_player_color;
static uint8_t chess_valid_moves[27];
static uint8_t chess_valid_move_count;



static void _draw_entire_board(launchpad_t* launchpad,uint8_t mask,uint8_t value){
	for (uint8_t i=0;i<8;i++){
		for (uint8_t j=0;j<8;j++){
			launchpad_set_led_rgb(launchpad,i,j,((chess_board[INDEX(i,j)]&mask)==value?chess_piece_colors[chess_board[INDEX(i,j)]&TYPE_MASK]:0x000000));
		}
	}
}



static _Bool _check_move(int8_t x,int8_t y,_Bool allow_empty){
	if (x<0||x>7||y<0||y>7){
		return 0;
	}
	if ((allow_empty&&!chess_board[INDEX(x,y)])||(chess_board[INDEX(x,y)]&&(chess_board[INDEX(x,y)]&FLAG_COLOR)==(chess_player_color^FLAG_COLOR))){
		chess_valid_moves[chess_valid_move_count++]=INDEX(x,y);
		return 1;
	}
	return 0;
}



static void _check_moves_directional(int8_t x,int8_t y,int8_t dx,int8_t dy){
	while (x+dx>=0&&x+dx<8&&y+dy>=0&&y+dy<8){
		x+=dx;
		y+=dy;
		if (!chess_board[INDEX(x,y)]){
			chess_valid_moves[chess_valid_move_count++]=INDEX(x,y);
		}
		else if (_check_move(x,y,0)||chess_board[INDEX(x,y)]){
			break;
		}
	}
}



static _Bool _calculate_moves(uint8_t index){
	piece_t piece=chess_board[index];
	if (!piece||(piece&FLAG_COLOR)!=chess_player_color){
		return 0;
	}
	int8_t x=index&7;
	int8_t y=index>>3;
	chess_valid_move_count=0;
	switch (piece&TYPE_MASK){
		case TYPE_PAWN:
			if (chess_player_color==FLAG_WHITE){
				if (y<7&&!chess_board[INDEX(x,y+1)]){
					chess_valid_moves[chess_valid_move_count++]=INDEX(x,y+1);
					if (y==1&&!chess_board[INDEX(x,y+2)]){
						chess_valid_moves[chess_valid_move_count++]=INDEX(x,y+2);
					}
				}
				if (x&&y<7){
					_check_move(x-1,y+1,0);
				}
				if (x<7&&y<7){
					_check_move(x+1,y+1,0);
				}
			}
			else{
				if (y&&!chess_board[INDEX(x,y-1)]){
					chess_valid_moves[chess_valid_move_count++]=INDEX(x,y-1);
					if (y==6&&!chess_board[INDEX(x,y-2)]){
						chess_valid_moves[chess_valid_move_count++]=INDEX(x,y-2);
					}
				}
				if (x&&y){
					_check_move(x-1,y-1,0);
				}
				if (x<7&&y){
					_check_move(x+1,y-1,0);
				}
			}
			break;
		case TYPE_ROOK:
			_check_moves_directional(x,y,1,0);
			_check_moves_directional(x,y,-1,0);
			_check_moves_directional(x,y,0,1);
			_check_moves_directional(x,y,0,-1);
			break;
		case TYPE_KNIGHT:
			_check_move(x+2,y-1,1);
			_check_move(x+2,y+1,1);
			_check_move(x-2,y-1,1);
			_check_move(x-2,y+1,1);
			_check_move(x-1,y+2,1);
			_check_move(x+1,y+2,1);
			_check_move(x-1,y-2,1);
			_check_move(x+1,y-2,1);
			break;
		case TYPE_BISHOP:
			_check_moves_directional(x,y,1,1);
			_check_moves_directional(x,y,-1,1);
			_check_moves_directional(x,y,1,-1);
			_check_moves_directional(x,y,-1,-1);
			break;
		case TYPE_QUEEN:
			_check_moves_directional(x,y,1,1);
			_check_moves_directional(x,y,1,0);
			_check_moves_directional(x,y,1,-1);
			_check_moves_directional(x,y,0,1);
			_check_moves_directional(x,y,0,-1);
			_check_moves_directional(x,y,-1,1);
			_check_moves_directional(x,y,-1,0);
			_check_moves_directional(x,y,-1,-1);
			break;
		case TYPE_KING:
			for (int8_t dx=-1;dx<2;dx++){
				for (int8_t dy=-1;dy<2;dy++){
					_check_move(x+dx,y+dy,1);
				}
			}
			break;
	}
	return !!chess_valid_move_count;
}



static void _select_piece(launchpad_t* launchpad,uint8_t index){
	launchpad_set_led_rgb(launchpad,8,3,chess_piece_colors[TYPE_ROOK]);
	launchpad_set_led_rgb(launchpad,8,4,chess_piece_colors[TYPE_KNIGHT]);
	launchpad_set_led_rgb(launchpad,8,5,chess_piece_colors[TYPE_BISHOP]);
	launchpad_set_led_rgb(launchpad,8,6,chess_piece_colors[TYPE_QUEEN]);
	launchpad_update_leds(launchpad);
	for (;;usleep(16000)){
		launchpad_button_event_t event;
		if (!launchpad_process_events(launchpad,&event)||!event.is_pressed||event.x!=8||event.y<3||event.y>6){
			continue;
		}
		chess_board[index]=(event.y-3+TYPE_ROOK)|chess_player_color;
		break;
	}
	launchpad_set_led_rgb(launchpad,8,3,0x000000);
	launchpad_set_led_rgb(launchpad,8,4,0x000000);
	launchpad_set_led_rgb(launchpad,8,5,0x000000);
	launchpad_set_led_rgb(launchpad,8,6,0x000000);
	launchpad_update_leds(launchpad);
}



int main(void){
	launchpad_t launchpad;
	if (!launchpad_open(&launchpad)){
		return 1;
	}
	launchpad_set_led_rgb(&launchpad,8,7,0xffffff);
	launchpad_set_led_rgb(&launchpad,8,0,0xffffff);
	chess_board[INDEX(0,0)]=TYPE_ROOK|FLAG_WHITE;
	chess_board[INDEX(1,0)]=TYPE_KNIGHT|FLAG_WHITE;
	chess_board[INDEX(2,0)]=TYPE_BISHOP|FLAG_WHITE;
	chess_board[INDEX(3,0)]=TYPE_QUEEN|FLAG_WHITE;
	chess_board[INDEX(4,0)]=TYPE_KING|FLAG_WHITE;
	chess_board[INDEX(5,0)]=TYPE_BISHOP|FLAG_WHITE;
	chess_board[INDEX(6,0)]=TYPE_KNIGHT|FLAG_WHITE;
	chess_board[INDEX(7,0)]=TYPE_ROOK|FLAG_WHITE;
	for (uint8_t i=0;i<8;i++){
		chess_board[INDEX(i,1)]=TYPE_PAWN|FLAG_WHITE;
		chess_board[INDEX(i,2)]=0;
		chess_board[INDEX(i,3)]=0;
		chess_board[INDEX(i,4)]=0;
		chess_board[INDEX(i,5)]=0;
		chess_board[INDEX(i,6)]=TYPE_PAWN|FLAG_BLACK;
	}
	chess_board[INDEX(0,7)]=TYPE_ROOK|FLAG_BLACK;
	chess_board[INDEX(1,7)]=TYPE_KNIGHT|FLAG_BLACK;
	chess_board[INDEX(2,7)]=TYPE_BISHOP|FLAG_BLACK;
	chess_board[INDEX(3,7)]=TYPE_QUEEN|FLAG_BLACK;
	chess_board[INDEX(4,7)]=TYPE_KING|FLAG_BLACK;
	chess_board[INDEX(5,7)]=TYPE_BISHOP|FLAG_BLACK;
	chess_board[INDEX(6,7)]=TYPE_KNIGHT|FLAG_BLACK;
	chess_board[INDEX(7,7)]=TYPE_ROOK|FLAG_BLACK;
	chess_player_color=FLAG_WHITE;
	chess_valid_move_count=0;
	_draw_entire_board(&launchpad,0,0);
	uint8_t piece_source_index=0xff;
	uint8_t timer=0;
	uint8_t quit_timer=0;
	while (1){
		launchpad_button_event_t event;
		if (launchpad_process_events(&launchpad,&event)){
			if (event.x<8&&event.y<8&&event.is_pressed){
				uint8_t new_index=INDEX(event.x,event.y);
				if (piece_source_index==0xff){
					_draw_entire_board(&launchpad,0,0);
					if (_calculate_moves(new_index)){
						piece_source_index=new_index;
					}
				}
				else{
					for (uint8_t i=0;i<chess_valid_move_count;i++){
						if (chess_valid_moves[i]==new_index){
							chess_valid_move_count=0;
							chess_board[new_index]=chess_board[piece_source_index]|FLAG_MOVED;
							chess_board[piece_source_index]=0;
							if ((chess_board[new_index]&TYPE_MASK)==TYPE_PAWN&&event.y==(chess_player_color==FLAG_WHITE?7:0)){
								_select_piece(&launchpad,new_index);
							}
							piece_source_index=0xff;
							_draw_entire_board(&launchpad,0,0);
							chess_player_color^=FLAG_COLOR;
							goto _move_found;
						}
					}
					if (chess_board[new_index]&&(chess_board[new_index]&FLAG_COLOR)==chess_player_color){
						piece_source_index=0xff;
						_draw_entire_board(&launchpad,0,0);
						if (_calculate_moves(new_index)){
							piece_source_index=new_index;
						}
					}
_move_found:
				}
			}
			else if (event.x==8&&!event.y){
				if (event.is_pressed){
					quit_timer=1;
				}
				else{
					quit_timer=0;
				}
			}
			else if (event.x==8&&event.y==7){
				if (event.is_pressed){
					_draw_entire_board(&launchpad,FLAG_COLOR,chess_player_color);
				}
				else{
					_draw_entire_board(&launchpad,0,0);
				}
			}
		}
		for (uint8_t i=0;i<chess_valid_move_count;i++){
			uint32_t color=0xb2b2b2;
			if (chess_board[chess_valid_moves[i]]){
				piece_t type=chess_board[chess_valid_moves[i]]&TYPE_MASK;
				color=((timer&(16>>(type==TYPE_KING)))?chess_piece_colors[type]:0x000000);
			}
			launchpad_set_led_rgb(&launchpad,chess_valid_moves[i]&7,chess_valid_moves[i]>>3,color);
		}
		timer=(timer+1)&31;
		if (quit_timer){
			quit_timer++;
			if (quit_timer==120){
				break;
			}
		}
		launchpad_update_leds(&launchpad);
		usleep(16000);
	}
	launchpad_close(&launchpad);
	return 0;
}
#endif
