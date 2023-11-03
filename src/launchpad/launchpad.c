#include <alsa/asoundlib.h>
#include <launchpad/launchpad.h>
#include <stdint.h>
#include <string.h>



#define LAUNCHPAD_MIDI_SUBDEVICE_NAME "Launchpad Mini MK3 LPMiniMK3 MI"

#define SYSEX_PREFIX 0xf0
#define SYSEX_SUFFIX 0xf7

#define SYSEX_MANUFACTURER_ID_NOVATION 0x00,0x20,0x29

#define SYSEX_MESSAGE_PREFIX 0x02,0x0d

#define SYSEX_MESSAGE_SET_COLOR 0x03
#define SYSEX_MESSAGE_SET_BRIGHTNESS 0x08
#define SYSEX_MESSAGE_SET_MODE 0x0e
#define SYSEX_MESSAGE_SET_INTERFACE 0x10

#define SYSEX_COLOR_DATA(x,y,r,g,b) 0x03,(x)+(y)*10+11,(r)>>1,(g)>>1,(b)>>1
#define SYSEX_BRIGHTNESS_MAX 0x7f
#define SYSEX_MODE_PROGRAMER 0x01
#define SYSEX_INTERFACE_MIDI 0x01



static const unsigned char _launchpad_setup_command[]={
	SYSEX_PREFIX,
	SYSEX_MANUFACTURER_ID_NOVATION,
	SYSEX_MESSAGE_PREFIX,
	SYSEX_MESSAGE_SET_INTERFACE,
	SYSEX_INTERFACE_MIDI,
	SYSEX_SUFFIX,

	SYSEX_PREFIX,
	SYSEX_MANUFACTURER_ID_NOVATION,
	SYSEX_MESSAGE_PREFIX,
	SYSEX_MESSAGE_SET_MODE,
	SYSEX_MODE_PROGRAMER,
	SYSEX_SUFFIX,

	SYSEX_PREFIX,
	SYSEX_MANUFACTURER_ID_NOVATION,
	SYSEX_MESSAGE_PREFIX,
	SYSEX_MESSAGE_SET_BRIGHTNESS,
	SYSEX_BRIGHTNESS_MAX,
	SYSEX_SUFFIX,

	SYSEX_PREFIX,
	SYSEX_MANUFACTURER_ID_NOVATION,
	SYSEX_MESSAGE_PREFIX,
	SYSEX_MESSAGE_SET_COLOR,
	SYSEX_COLOR_DATA(0,0,0,0,0),SYSEX_COLOR_DATA(1,0,0,0,0),SYSEX_COLOR_DATA(2,0,0,0,0),SYSEX_COLOR_DATA(3,0,0,0,0),SYSEX_COLOR_DATA(4,0,0,0,0),SYSEX_COLOR_DATA(5,0,0,0,0),SYSEX_COLOR_DATA(6,0,0,0,0),SYSEX_COLOR_DATA(7,0,0,0,0),SYSEX_COLOR_DATA(8,0,0,0,0),
	SYSEX_COLOR_DATA(0,1,0,0,0),SYSEX_COLOR_DATA(1,1,0,0,0),SYSEX_COLOR_DATA(2,1,0,0,0),SYSEX_COLOR_DATA(3,1,0,0,0),SYSEX_COLOR_DATA(4,1,0,0,0),SYSEX_COLOR_DATA(5,1,0,0,0),SYSEX_COLOR_DATA(6,1,0,0,0),SYSEX_COLOR_DATA(7,1,0,0,0),SYSEX_COLOR_DATA(8,1,0,0,0),
	SYSEX_COLOR_DATA(0,2,0,0,0),SYSEX_COLOR_DATA(1,2,0,0,0),SYSEX_COLOR_DATA(2,2,0,0,0),SYSEX_COLOR_DATA(3,2,0,0,0),SYSEX_COLOR_DATA(4,2,0,0,0),SYSEX_COLOR_DATA(5,2,0,0,0),SYSEX_COLOR_DATA(6,2,0,0,0),SYSEX_COLOR_DATA(7,2,0,0,0),SYSEX_COLOR_DATA(8,2,0,0,0),
	SYSEX_COLOR_DATA(0,3,0,0,0),SYSEX_COLOR_DATA(1,3,0,0,0),SYSEX_COLOR_DATA(2,3,0,0,0),SYSEX_COLOR_DATA(3,3,0,0,0),SYSEX_COLOR_DATA(4,3,0,0,0),SYSEX_COLOR_DATA(5,3,0,0,0),SYSEX_COLOR_DATA(6,3,0,0,0),SYSEX_COLOR_DATA(7,3,0,0,0),SYSEX_COLOR_DATA(8,3,0,0,0),
	SYSEX_COLOR_DATA(0,4,0,0,0),SYSEX_COLOR_DATA(1,4,0,0,0),SYSEX_COLOR_DATA(2,4,0,0,0),SYSEX_COLOR_DATA(3,4,0,0,0),SYSEX_COLOR_DATA(4,4,0,0,0),SYSEX_COLOR_DATA(5,4,0,0,0),SYSEX_COLOR_DATA(6,4,0,0,0),SYSEX_COLOR_DATA(7,4,0,0,0),SYSEX_COLOR_DATA(8,4,0,0,0),
	SYSEX_COLOR_DATA(0,5,0,0,0),SYSEX_COLOR_DATA(1,5,0,0,0),SYSEX_COLOR_DATA(2,5,0,0,0),SYSEX_COLOR_DATA(3,5,0,0,0),SYSEX_COLOR_DATA(4,5,0,0,0),SYSEX_COLOR_DATA(5,5,0,0,0),SYSEX_COLOR_DATA(6,5,0,0,0),SYSEX_COLOR_DATA(7,5,0,0,0),SYSEX_COLOR_DATA(8,5,0,0,0),
	SYSEX_COLOR_DATA(0,6,0,0,0),SYSEX_COLOR_DATA(1,6,0,0,0),SYSEX_COLOR_DATA(2,6,0,0,0),SYSEX_COLOR_DATA(3,6,0,0,0),SYSEX_COLOR_DATA(4,6,0,0,0),SYSEX_COLOR_DATA(5,6,0,0,0),SYSEX_COLOR_DATA(6,6,0,0,0),SYSEX_COLOR_DATA(7,6,0,0,0),SYSEX_COLOR_DATA(8,6,0,0,0),
	SYSEX_COLOR_DATA(0,7,0,0,0),SYSEX_COLOR_DATA(1,7,0,0,0),SYSEX_COLOR_DATA(2,7,0,0,0),SYSEX_COLOR_DATA(3,7,0,0,0),SYSEX_COLOR_DATA(4,7,0,0,0),SYSEX_COLOR_DATA(5,7,0,0,0),SYSEX_COLOR_DATA(6,7,0,0,0),SYSEX_COLOR_DATA(7,7,0,0,0),SYSEX_COLOR_DATA(8,7,0,0,0),
	SYSEX_COLOR_DATA(0,8,0,0,0),SYSEX_COLOR_DATA(1,8,0,0,0),SYSEX_COLOR_DATA(2,8,0,0,0),SYSEX_COLOR_DATA(3,8,0,0,0),SYSEX_COLOR_DATA(4,8,0,0,0),SYSEX_COLOR_DATA(5,8,0,0,0),SYSEX_COLOR_DATA(6,8,0,0,0),SYSEX_COLOR_DATA(7,8,0,0,0),SYSEX_COLOR_DATA(8,8,0,0,0),
	SYSEX_SUFFIX
};



_Bool launchpad_open(launchpad_t* out){
	char name_buffer[64];
	for (int card=-1;snd_card_next(&card)>=0&&card>=0;){
		snprintf(name_buffer,64,"hw:%u",card);
		snd_ctl_t* ctl;
		if (snd_ctl_open(&ctl,name_buffer,0)<0){
			continue;
		}
		for (int device=-1;snd_ctl_rawmidi_next_device(ctl,&device)>=0&&device>=0;){
			snd_rawmidi_info_t *info;
			snd_rawmidi_info_alloca(&info);
			snd_rawmidi_info_set_device(info,device);
			snd_rawmidi_info_set_stream(info,SND_RAWMIDI_STREAM_INPUT);
			if (snd_ctl_rawmidi_info(ctl,info)<0){
				continue;
			}
			int subdevice_count=snd_rawmidi_info_get_subdevices_count(info);
			snd_rawmidi_info_set_stream(info,SND_RAWMIDI_STREAM_OUTPUT);
			if (snd_ctl_rawmidi_info(ctl,info)<0||!subdevice_count||subdevice_count!=snd_rawmidi_info_get_subdevices_count(info)){
				continue;
			}
			for (int subdevice=0;subdevice<subdevice_count;subdevice++){
				snd_rawmidi_info_set_subdevice(info,subdevice);
				snd_ctl_rawmidi_info(ctl,info);
				if (!strcmp(snd_rawmidi_info_get_subdevice_name(info),LAUNCHPAD_MIDI_SUBDEVICE_NAME)){
					snprintf(name_buffer,64,"hw:%u,%u,%u",card,device,subdevice);
					snd_ctl_close(ctl);
					goto _launchpad_found;
				}
			}
		}
		snd_ctl_close(ctl);
	}
	return 0;
_launchpad_found:
	memset(out,0,sizeof(launchpad_t));
	if (snd_rawmidi_open((snd_rawmidi_t**)(&(out->_midi_in)),(snd_rawmidi_t**)(&(out->_midi_out)),name_buffer,SND_RAWMIDI_NONBLOCK)<0){
		return 0;
	}
	snd_rawmidi_nonblock(out->_midi_in,1);
	snd_rawmidi_read(out->_midi_in,NULL,0);
	snd_rawmidi_write(out->_midi_out,_launchpad_setup_command,sizeof(_launchpad_setup_command));
	return 1;
}



void launchpad_close(launchpad_t* launchpad){
	snd_rawmidi_write(launchpad->_midi_out,_launchpad_setup_command,sizeof(_launchpad_setup_command));
	snd_rawmidi_close(launchpad->_midi_in);
	snd_rawmidi_close(launchpad->_midi_out);
}



void launchpad_update_leds(launchpad_t* launchpad){
	uint8_t buffer[413]={
		SYSEX_PREFIX,
		SYSEX_MANUFACTURER_ID_NOVATION,
		SYSEX_MESSAGE_PREFIX,
		SYSEX_MESSAGE_SET_COLOR
	};
	uint16_t i=7;
	for (uint8_t j=0;j<81;j++){
		if (!(launchpad->leds+j)->modified){
			continue;
		}
		(launchpad->leds+j)->modified=0;
		buffer[i]=0x03;
		buffer[i+1]=j+j/9+11;
		buffer[i+2]=(launchpad->leds+j)->r>>1;
		buffer[i+3]=(launchpad->leds+j)->g>>1;
		buffer[i+4]=(launchpad->leds+j)->b>>1;
		i+=5;
	}
	if (i==7){
		return;
	}
	buffer[i]=SYSEX_SUFFIX;
	snd_rawmidi_write(launchpad->_midi_out,buffer,i+1);
}



_Bool launchpad_process_events(launchpad_t* launchpad,launchpad_button_event_t* out){
	while (1){
		uint8_t buffer[3];
		int length=snd_rawmidi_read(launchpad->_midi_in,buffer,3);
		if (length<=0){
			return 0;
		}
		if (buffer[0]!=0x90&&buffer[0]!=0xb0){
			continue;
		}
		out->x=(buffer[1]%10)-1;
		out->y=buffer[1]/10-1;
		out->is_pressed=!!buffer[2];
		return 1;
	}
}
