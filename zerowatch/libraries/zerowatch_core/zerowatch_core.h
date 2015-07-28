#ifndef __zerowatch_core_H__
#define __zerowatch_core_H__

#include <Arduino.h>
#include <SPI.h>
#include "pt2.h"
#include "Ucglib.h"
#include <avr/pgmspace.h>
#include "app_main.h"

class Zerowatch {
private:
  #define sel_btn 10
  #define up_btn 3
  #define down_btn 5
  #define back_btn 2
  bool system_init = 0;
  void __clock(); //clock interrupt
  void screen_init();
  void show_watchface(uint8_t type);
  void interrupt();
  void init();
  void init_msg_array();
  void setTimeValue();
  bool receiveBluetoothData();
  void parseStartSignal(byte c);
  void parseCommand(byte c);
  void parseMessage(byte c);
  void parseTime(byte c);
  void parseId(byte c);
  bool parseEndSignal(byte c);
  void processTransaction();
  void drawMessage(int message, int y);
  int FreeRam();
  //threads
   int button_thread1(struct pt *pt, int interval);
   int button_thread2(struct pt *pt, int interval);
   int button_thread3(struct pt *pt, int interval);
   int button_thread4(struct pt *pt, int interval);
   int protothread5(struct pt *pt, int interval);
   int protothread6(struct pt *pt, int interval);
   int protothread7(struct pt *pt, int interval);
   int protothread8(struct pt *pt, int interval);
  
  void update_clock();
  void update_screen();
  
  //watchfaces
  void setup_watchface_1();
  void setup_watchface_2();
  void setup_watchface_3();
  
  void watchface_1();
  void watchface_2();
  void watchface_3();
  
  //variables below here
	#define BT Serial1
	#define font1 ucg_font_04b_03_hf
	#define font2 ucg_font_ncenR14_tr
	#define font3 ucg_font_7x13B_tr

   	//**************************************************
	//bluetooth handlers
	//**************************************************
	#define TR_MODE_IDLE 1
	#define TR_MODE_WAIT_CMD 11
	#define TR_MODE_WAIT_MESSAGE 101
	#define TR_MODE_WAIT_TIME 111
	#define TR_MODE_WAIT_ID 121
	#define TR_MODE_WAIT_COMPLETE 201

	#define TRANSACTION_START_BYTE 0xfc
	#define TRANSACTION_END_BYTE 0xfd

	#define CMD_TYPE_NONE 0x00
	#define CMD_TYPE_RESET_NORMAL_OBJ 0x02

	#define CMD_TYPE_ADD_NORMAL_OBJ 0x12

	#define CMD_TYPE_DELETE_NORMAL_OBJ 0x22

	#define CMD_TYPE_SET_TIME 0x31
	#define CMD_TYPE_REQUEST_MOVEMENT_HISTORY 0x32
	#define CMD_TYPE_SET_CLOCK_STYLE 0x33
	#define CMD_TYPE_SET_INDICATOR 0x34

	#define CMD_TYPE_PING 0x51
	#define CMD_TYPE_AWAKE 0x52
	#define CMD_TYPE_SLEEP 0x53
	#define CMD_TYPE_REBOOT 0x54

	byte TRANSACTION_POINTER;
	byte TR_COMMAND;
	bool bt_can_update;
	//**************************************************
	//clock styles
	//**************************************************
	#define CLOCK_STYLE_SIMPLE_ANALOG  0x01
	#define CLOCK_STYLE_SIMPLE_DIGIT  0x02
	#define CLOCK_STYLE_SIMPLE_MIX  0x03
	byte clockStyle;

	//**************************************************
	//bluetooth messages
	//**************************************************
	#define MSG_COUNT_MAX 5 //max messages
	#define MSG_BUFFER_MAX 30 //max characters per message

	//----- Emergency item buffer
	#define EMG_COUNT_MAX 5 //max messages
	#define EMG_BUFFER_MAX 30 //max characters per emergency message

	//**************************************************
	//message item buffers
	//**************************************************
	unsigned char msgBuffer[MSG_COUNT_MAX][MSG_BUFFER_MAX];
	char msgParsingLine;
	char msgParsingChar;
	char msgCurDisp;

	//**************************************************
	//time
	//**************************************************
	volatile byte iWeek;    // 1: SUN, MON, TUE, WED, THU, FRI,SAT
	volatile byte iAmPm;    // 0:AM, 1:PM
	volatile byte iHour;
	volatile byte iMinutes;
	volatile uint8_t seconds;

	#define INDICATOR_ENABLE 0x01
	bool updateIndicator;

	#define TIME_BUFFER_MAX 6
	char timeParsingIndex;
	char timeBuffer[6];

	int xxx;
	byte iRadius;
	// Calculate clock pin position
	double RAD;
	double LR;

	//**************************************************
	//screen
	//**************************************************
	uint8_t default_timeout = 30;
	uint8_t screen_timeout = default_timeout;
	bool screen_changed;
	bool clock_needs_redraw;
	bool menu_needs_redraw;
	bool needredraw;
	bool screenneedstodim;
	bool screen_dimmed;
	
	//***************************************************
	//misc
	//***************************************************
	uint8_t level; //menu level

	bool btn_sel_pressed;
	bool btn_up_pressed;
	bool btn_down_pressed;

	bool init_apps;
	bool btn_back_pressed;
    bool buttons_can_be_pressed;
	
	struct pt pt1,pt2,pt3,pt4,pt5,pt6,pt7,pt8;
    Ucglib_SSD1351_18x128x128_HWSPI ucg;
public:
  _Shell _shell;
  Zerowatch();
  void __init();
  void __Zerowatch_loop();
  //api calls
  void Shell_LaunchApp(const char* params);
  int ram_usage(bool type); //type 0 = percent, type 1 = kb
  //void set_screen_timeout(uint8_t timeout);
  //void update_time(uint8_t day, uint8_t hour, bool ampm, uint8_t minute);
  //graphics
  void clear_screen();
  void setFont(uint8_t type);
  void setColor(uint8_t r,uint8_t g,uint8_t b);
  void draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
  void fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
  //void draw_roundrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r);
  //void fill_roundrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r);
  void setPrintPos(uint8_t x, uint8_t y);
  void print(char* text);
  void printint(int text);
}; 
#endif