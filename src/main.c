#define APP 2
#if APP==0
// grid=hue, sidebar=saturation, sidebar_button=(sidebar=brightness)
// grid=hue, sidebar=brightness, sidebar_button=(sidebar=saturation)
// grid=brightness, sidebar=saturation[selected=0%]
#include <launchpad/launchpad.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



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
			usleep(16000);
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
		usleep(16000);
	}
	launchpad_close(&launchpad);
	return 0;
}
#elif APP==1
#include <launchpad/launchpad.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



#define INDEX(x,y) (((y)<<3)|(x))



#define TYPE_MASK 0x1f
#define TYPE_NONE 0
#define TYPE_PAWN 1
#define TYPE_ROOK 2
#define TYPE_KNIGHT 3
#define TYPE_BISHOP 4
#define TYPE_QUEEN 5
#define TYPE_KING 6

#define FLAG_EN_PASSANT 0x20
#define FLAG_MOVED 0x40
#define FLAG_COLOR 0x80

#define FLAG_WHITE 0x00
#define FLAG_BLACK 0x80

#define MOVE_INDEX_MASK 0x3f
#define MOVE_FLAG_CASTLE 0x40
#define MOVE_FLAG_EN_PASSANT 0x80



typedef uint8_t piece_t;



typedef struct _CHESS_MOVES{
	uint8_t data[27];
	uint8_t count;
} chess_moves_t;



static const uint32_t chess_piece_colors[]={
	[TYPE_NONE]=0x000000,
	[TYPE_PAWN]=0xff0000,
	[TYPE_ROOK]=0xffff00,
	[TYPE_KNIGHT]=0x00ff00,
	[TYPE_BISHOP]=0x00ffff,
	[TYPE_QUEEN]=0xff00ff,
	[TYPE_KING]=0x0000ff,
};



static const uint32_t chess_piece_dark_colors[]={
	[TYPE_NONE]=0x000000,
	[TYPE_PAWN]=0x5f0000,
	[TYPE_ROOK]=0x5f5f00,
	[TYPE_KNIGHT]=0x005f00,
	[TYPE_BISHOP]=0x005f5f,
	[TYPE_QUEEN]=0x5f005f,
	[TYPE_KING]=0x00005f,
};



static piece_t chess_board[64];
static uint8_t chess_player_color;
static chess_moves_t chess_moves;



static _Bool _calculate_moves(uint8_t index,chess_moves_t* out,uint8_t state);



static void _draw_entire_board(launchpad_t* launchpad,uint8_t mask,uint8_t value){
	for (uint8_t i=0;i<8;i++){
		for (uint8_t j=0;j<8;j++){
			launchpad_set_led_rgb(launchpad,i,j,((chess_board[INDEX(i,j)]&mask)==value?((chess_board[INDEX(i,j)]&FLAG_COLOR)!=chess_player_color?chess_piece_dark_colors:chess_piece_colors)[chess_board[INDEX(i,j)]&TYPE_MASK]:0x000000));
		}
	}
}



static _Bool _check_move(int8_t x,int8_t y,_Bool allow_empty,chess_moves_t* out){
	if (x<0||x>7||y<0||y>7){
		return 0;
	}
	if ((allow_empty&&!chess_board[INDEX(x,y)])||(chess_board[INDEX(x,y)]&&(chess_board[INDEX(x,y)]&FLAG_COLOR)==(chess_player_color^FLAG_COLOR))){
		out->data[out->count++]=INDEX(x,y);
		return 1;
	}
	return 0;
}



static void _check_moves_directional(int8_t x,int8_t y,int8_t dx,int8_t dy,chess_moves_t* out){
	while (x+dx>=0&&x+dx<8&&y+dy>=0&&y+dy<8){
		x+=dx;
		y+=dy;
		if (!chess_board[INDEX(x,y)]){
			out->data[out->count++]=INDEX(x,y);
		}
		else if (_check_move(x,y,0,out)||chess_board[INDEX(x,y)]){
			break;
		}
	}
}



static _Bool _is_valid_king_move_check(uint8_t min_x,uint8_t min_y,uint8_t max_x,uint8_t max_y,uint8_t state){
	chess_player_color^=FLAG_COLOR;
	for (uint8_t i=0;i<64;i++){
		if (!chess_board[i]||(chess_board[i]&FLAG_COLOR)!=chess_player_color||(chess_board[i]&TYPE_MASK)==TYPE_KING){
			continue;
		}
		chess_moves_t tmp;
		if (!_calculate_moves(i,&tmp,state+1)){
			continue;
		}
		for (uint8_t j=0;j<tmp.count;j++){
			uint8_t mx=(tmp.data[j]&MOVE_INDEX_MASK)&7;
			uint8_t my=(tmp.data[j]&MOVE_INDEX_MASK)>>3;
			if (mx>=min_x&&mx<=max_x&&my>=min_y&&my<=max_y){
				chess_player_color^=FLAG_COLOR;
				return 0;
			}
		}
	}
	chess_player_color^=FLAG_COLOR;
	return 1;
}



static _Bool _is_valid_king_move(int8_t x,int8_t y){
	for (int8_t dx=-1;dx<2;dx++){
		for (int8_t dy=-1;dy<2;dy++){
			if (x+dx>=0&&x+dx<8&&y+dy>=0&&y+dy<8&&(chess_board[INDEX(x+dx,y+dy)]&TYPE_MASK)==TYPE_KING&&(chess_board[INDEX(x+dx,y+dy)]&FLAG_COLOR)==(chess_player_color^FLAG_COLOR)){
				return 0;
			}
		}
	}
	return 1;
}



static void _check_castle(uint8_t x,uint8_t y,chess_moves_t* out){
	if (!_is_valid_king_move((x?6:2),y)){
		return;
	}
	uint8_t min_check_test=(x?4:2);
	uint8_t max_check_test=(x?6:4);
	if (x){
		for (uint8_t i=5;i<7;i++){
			if (chess_board[INDEX(i,y)]){
				return;
			}
		}
	}
	else{
		for (uint8_t i=1;i<4;i++){
			if (chess_board[INDEX(i,y)]){
				return;
			}
		}
	}
	if (!_is_valid_king_move_check(min_check_test,max_check_test,y,y,0)){
		return;
	}
	out->data[out->count++]=INDEX((x?6:2),y)|MOVE_FLAG_CASTLE;
}



static _Bool _calculate_moves(uint8_t index,chess_moves_t* out,uint8_t state){
	out->count=0;
	piece_t piece=chess_board[index];
	if (!piece||(piece&FLAG_COLOR)!=chess_player_color){
		return 0;
	}
	int8_t x=index&7;
	int8_t y=index>>3;
	switch (piece&TYPE_MASK){
		case TYPE_PAWN:
			if (chess_player_color==FLAG_WHITE){
				if (y<7&&!chess_board[INDEX(x,y+1)]){
					out->data[out->count++]=INDEX(x,y+1);
					if (y==1&&!chess_board[INDEX(x,y+2)]){
						out->data[out->count++]=INDEX(x,y+2);
					}
				}
				if (x&&y<7){
					_check_move(x-1,y+1,0,out);
				}
				if (x<7&&y<7){
					_check_move(x+1,y+1,0,out);
				}
				if (x<7&&y==4&&(chess_board[INDEX(x+1,y)]&FLAG_EN_PASSANT)&&(chess_board[INDEX(x+1,y)]&FLAG_COLOR)==(chess_player_color^FLAG_COLOR)){
					out->data[out->count++]=INDEX(x+1,y)|MOVE_FLAG_EN_PASSANT;
				}
				if (x&&y==4&&(chess_board[INDEX(x-1,y)]&FLAG_EN_PASSANT)&&(chess_board[INDEX(x-1,y)]&FLAG_COLOR)==(chess_player_color^FLAG_COLOR)){
					out->data[out->count++]=INDEX(x-1,y)|MOVE_FLAG_EN_PASSANT;
				}
			}
			else{
				if (y&&!chess_board[INDEX(x,y-1)]){
					out->data[out->count++]=INDEX(x,y-1);
					if (y==6&&!chess_board[INDEX(x,y-2)]){
						out->data[out->count++]=INDEX(x,y-2);
					}
				}
				if (x&&y){
					_check_move(x-1,y-1,0,out);
				}
				if (x<7&&y){
					_check_move(x+1,y-1,0,out);
				}
				if (x<7&&y==3&&(chess_board[INDEX(x+1,y)]&FLAG_EN_PASSANT)&&(chess_board[INDEX(x+1,y)]&FLAG_COLOR)==(chess_player_color^FLAG_COLOR)){
					out->data[out->count++]=INDEX(x+1,y)|MOVE_FLAG_EN_PASSANT;
				}
				if (x&&y==3&&(chess_board[INDEX(x-1,y)]&FLAG_EN_PASSANT)&&(chess_board[INDEX(x-1,y)]&FLAG_COLOR)==(chess_player_color^FLAG_COLOR)){
					out->data[out->count++]=INDEX(x-1,y)|MOVE_FLAG_EN_PASSANT;
				}
			}
			break;
		case TYPE_ROOK:
			_check_moves_directional(x,y,1,0,out);
			_check_moves_directional(x,y,-1,0,out);
			_check_moves_directional(x,y,0,1,out);
			_check_moves_directional(x,y,0,-1,out);
			break;
		case TYPE_KNIGHT:
			_check_move(x+2,y-1,1,out);
			_check_move(x+2,y+1,1,out);
			_check_move(x-2,y-1,1,out);
			_check_move(x-2,y+1,1,out);
			_check_move(x-1,y+2,1,out);
			_check_move(x+1,y+2,1,out);
			_check_move(x-1,y-2,1,out);
			_check_move(x+1,y-2,1,out);
			break;
		case TYPE_BISHOP:
			_check_moves_directional(x,y,1,1,out);
			_check_moves_directional(x,y,-1,1,out);
			_check_moves_directional(x,y,1,-1,out);
			_check_moves_directional(x,y,-1,-1,out);
			break;
		case TYPE_QUEEN:
			_check_moves_directional(x,y,1,1,out);
			_check_moves_directional(x,y,1,0,out);
			_check_moves_directional(x,y,1,-1,out);
			_check_moves_directional(x,y,0,1,out);
			_check_moves_directional(x,y,0,-1,out);
			_check_moves_directional(x,y,-1,1,out);
			_check_moves_directional(x,y,-1,0,out);
			_check_moves_directional(x,y,-1,-1,out);
			break;
		case TYPE_KING:
			for (int8_t dx=-1;dx<2;dx++){
				for (int8_t dy=-1;dy<2;dy++){
					if (_is_valid_king_move(x+dx,y+dy)&&_is_valid_king_move_check(x+dx,y+dy,x+dx,y+dy,state)){
						_check_move(x+dx,y+dy,1,out);
					}
				}
			}
			if (!((chess_board[INDEX(0,y)]|piece)&FLAG_MOVED)){
				_check_castle(0,y,out);
			}
			if (!((chess_board[INDEX(7,y)]|piece)&FLAG_MOVED)){
				_check_castle(7,y,out);
			}
			break;
	}
	if ((piece&TYPE_MASK)==TYPE_KING||state>=2){
		return !!out->count;
	}
	uint8_t kx=0;
	uint8_t ky=0;
	for (uint8_t i=0;i<64;i++){
		if ((chess_board[i]&(TYPE_MASK|FLAG_COLOR))==(TYPE_KING|chess_player_color)){
			kx=i&7;
			ky=i>>3;
			break;
		}
	}
	for (int8_t i=0;i<out->count;i++){
		uint8_t j=out->data[i];
		if (j&MOVE_FLAG_CASTLE){
			continue;
		}
		uint8_t k=j&MOVE_INDEX_MASK;
		piece_t other=chess_board[k];
		if (j&MOVE_FLAG_EN_PASSANT){
			chess_board[(chess_player_color==FLAG_WHITE?k+8:k-8)]=piece|FLAG_MOVED;
			chess_board[k]=0;
		}
		else{
			chess_board[k]=piece;
		}
		chess_board[index]=0;
		if (!_is_valid_king_move_check(kx,ky,kx,ky,state)){
			// printf("Invalid move: %u,%u -> %u,%u\n",x,y,k&3,k>>3);
			out->count--;
			if (i<out->count){
				out->data[i]=out->data[out->count];
			}
			i--;
		}
		if (j&MOVE_FLAG_EN_PASSANT){
			chess_board[(chess_player_color==FLAG_WHITE?k+8:k-8)]=0;
		}
		chess_board[index]=piece;
		chess_board[k]=other;
	}
	return !!out->count;
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
		chess_board[index]=(event.y-3+TYPE_ROOK)|chess_player_color|FLAG_MOVED;
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
	chess_moves.count=0;
	_draw_entire_board(&launchpad,0,0);
	uint8_t piece_source_index=0xff;
	uint8_t last_en_passant[2]={0xff,0xff};
	uint8_t timer=0;
	uint8_t quit_timer=0;
	while (1){
		launchpad_button_event_t event;
		if (launchpad_process_events(&launchpad,&event)){
			if (event.x<8&&event.y<8&&event.is_pressed){
				uint8_t new_index=INDEX(event.x,event.y);
				if (last_en_passant[!!chess_player_color]!=0xff){
					if ((chess_board[last_en_passant[!!chess_player_color]]&FLAG_COLOR)==chess_player_color){
						chess_board[last_en_passant[!!chess_player_color]]&=~FLAG_EN_PASSANT;
					}
					last_en_passant[!!chess_player_color]=0xff;
				}
				if (piece_source_index==0xff){
					_draw_entire_board(&launchpad,0,0);
					if (_calculate_moves(new_index,&chess_moves,0)){
						piece_source_index=new_index;
					}
				}
				else if (piece_source_index==new_index){
					piece_source_index=0xff;
					chess_moves.count=0;
					_draw_entire_board(&launchpad,0,0);
				}
				else{
					for (uint8_t i=0;i<chess_moves.count;i++){
						if ((chess_moves.data[i]&MOVE_INDEX_MASK)==new_index){
							chess_moves.count=0;
							if (chess_moves.data[i]&MOVE_FLAG_EN_PASSANT){
								chess_board[(chess_player_color==FLAG_WHITE?new_index+8:new_index-8)]=chess_board[piece_source_index]|FLAG_MOVED;
								chess_board[new_index]=0;
							}
							else if (chess_moves.data[i]&MOVE_FLAG_CASTLE){
								chess_board[new_index]=chess_board[piece_source_index]|FLAG_MOVED;
								chess_board[(new_index+piece_source_index)>>1]=chess_board[INDEX((event.x==2?0:7),event.y)]|FLAG_MOVED;
								chess_board[INDEX((event.x==2?0:7),event.y)]=0;
							}
							else{
								if ((chess_board[new_index]&TYPE_MASK)==TYPE_KING){
									printf("End\n");
								}
								chess_board[new_index]=chess_board[piece_source_index]|FLAG_MOVED;
							}
							chess_board[piece_source_index]=0;
							if ((chess_board[new_index]&TYPE_MASK)==TYPE_PAWN&&event.y==(chess_player_color==FLAG_WHITE?7:0)){
								_select_piece(&launchpad,new_index);
							}
							else if ((chess_board[new_index]&TYPE_MASK)==TYPE_PAWN&&(piece_source_index>>3)==(chess_player_color==FLAG_WHITE?1:6)){
								chess_board[new_index]|=FLAG_EN_PASSANT;
								last_en_passant[!!chess_player_color]=new_index;
							}
							piece_source_index=0xff;
							chess_player_color^=FLAG_COLOR;
							_draw_entire_board(&launchpad,0,0);
							goto _move_found;
						}
					}
					if (chess_board[new_index]&&(chess_board[new_index]&FLAG_COLOR)==chess_player_color){
						piece_source_index=0xff;
						_draw_entire_board(&launchpad,0,0);
						if (_calculate_moves(new_index,&chess_moves,0)){
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
		for (uint8_t i=0;i<chess_moves.count;i++){
			uint32_t color=0xb2b2b2;
			uint8_t j=chess_moves.data[i]&MOVE_INDEX_MASK;
			if (chess_board[j]){
				piece_t type=chess_board[j]&TYPE_MASK;
				color=((timer&(16>>(type==TYPE_KING)))?chess_piece_dark_colors[type]:0x000000);
			}
			launchpad_set_led_rgb(&launchpad,j&7,j>>3,color);
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
#else
#include <launchpad/launchpad.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



#define INDEX(x,y) ((x)|((y)<<3))



static uint8_t fruit_index=0;
static uint8_t snake[64]={INDEX(4,4),INDEX(4,3)};
static uint8_t snake_length=2;



static void _generate_fruit(void){
_retry:
	fruit_index=rand()&63;
	for (uint8_t i=0;i<snake_length;i++){
		if (snake[i]==fruit_index){
			goto _retry;
		}
	}
}



int main(void){
	launchpad_t launchpad;
	if (!launchpad_open(&launchpad)){
		return 1;
	}
	launchpad_set_led_rgb(&launchpad,0,8,0xffffff);
	launchpad_set_led_rgb(&launchpad,1,8,0xffffff);
	launchpad_set_led_rgb(&launchpad,2,8,0xffffff);
	launchpad_set_led_rgb(&launchpad,3,8,0xffffff);
	launchpad_set_led_rgb(&launchpad,8,0,0xffffff);
	int8_t dx=0;
	int8_t dy=1;
	uint8_t quit_timer=0;
	_generate_fruit();
	while (1){
		launchpad_button_event_t event;
		if (launchpad_process_events(&launchpad,&event)){
			if (event.y==8&&event.is_pressed){
				if (!event.x){
					dx=0;
					dy=1;
				}
				else if (event.x==1){
					dx=0;
					dy=-1;
				}
				else if (event.x==2){
					dx=-1;
					dy=0;
				}
				else if (event.x==3){
					dx=1;
					dy=0;
				}
			}
			else if (event.x==8&&!event.y){
				quit_timer=(event.is_pressed?1:0);
			}
		}
		if (dx||dy){
			uint8_t new_head=INDEX(((snake[0]&7)+dx)&7,((snake[0]>>3)+dy)&7);
			if (new_head==fruit_index){
				fruit_index=0xff;
				snake_length++;
			}
			else{
				for (uint8_t i=0;i<snake_length;i++){
					if (snake[i]==new_head){
						dx=0;
						dy=0;
						goto _continue;
					}
				}
				launchpad_set_led_rgb(&launchpad,snake[snake_length-1]&7,snake[snake_length-1]>>3,0x000000);
			}
			for (uint8_t i=snake_length-1;i;i--){
				snake[i]=snake[i-1];
			}
			snake[0]=new_head;
			if (fruit_index==0xff){
				_generate_fruit();
			}
		}
_continue:
		for (uint8_t i=0;i<snake_length;i++){
			launchpad_set_led_hsl(&launchpad,snake[i]&7,snake[i]>>3,i<<4,255,255);
		}
		launchpad_set_led_rgb(&launchpad,fruit_index&7,fruit_index>>3,0xffffff);
		launchpad_update_leds(&launchpad);
		usleep(500000);
		if (quit_timer){
			quit_timer++;
			if (quit_timer==3){
				break;
			}
		}
	}
	launchpad_close(&launchpad);
	return 0;
}
#endif
