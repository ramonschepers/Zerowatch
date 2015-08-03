#include <SPI.h>
#include "pt2.h"
#include "Ucglib.h"
#include <avr/pgmspace.h>
#include "zerowatch_core.h"
#include "app_main.h"

Zerowatch::Zerowatch() {
	if (!system_init) {
		system_init = 1;
		init();
	}
}

void  Zerowatch::__clock() {
	seconds++;
	if (seconds >= 60) {
		iMinutes++;
		seconds = 0;
		if(iMinutes >= 60) {
			iMinutes = 0;
			iHour++;
			if(iHour > 12) {
				iHour = 1;
				(iAmPm == 0) ? iAmPm=1 : iAmPm=0;
				if(iAmPm == 0) {
					iWeek++;
					if(iWeek >= 7){
						iWeek = 1;
					}
				}
			}
		}
	}
	if (!screen_dimmed){
		if(screen_timeout > 0) {
			screen_timeout--;
		} else {
			screen_dimmed = 1;
			screenneedstodim = 1;
		}
	}
}

void Zerowatch::__init() {
	
		PT_INIT(&pt1);
		PT_INIT(&pt2);
		PT_INIT(&pt3);
		PT_INIT(&pt4);
		PT_INIT(&pt5);
		PT_INIT(&pt6);
		PT_INIT(&pt7);
		PT_INIT(&pt8);
		
		BT.begin(9600);
		
		pinMode(8, OUTPUT);
		digitalWrite(8, HIGH);
		pinMode(back_btn,INPUT);
		pinMode(up_btn,INPUT);
		pinMode(down_btn,INPUT);
		pinMode(sel_btn,INPUT);

		delay(200);
		screen_init();
	  
		delay(500);
		pinMode(4, OUTPUT);
        analogWriteResolution(16);
		analogWrite(4,1024);
	
}

extern "C" char* sbrk(int incr);
int Zerowatch::FreeRam() {
  char top;
  return &top - reinterpret_cast<char*>(sbrk(0));
}

int Zerowatch::ram_usage(bool type) {
	if (type == 0) {
		return map(FreeRam(),0,32768,100,0); //in percent
	} else {
		return map(FreeRam(),0,32768,32,0); //in kb
	}
}


void Zerowatch::init() {
	pinMode(SCREEN,OUTPUT);
	digitalWrite(SCREEN,HIGH);
	pinMode(A5,INPUT);
	system_init = 1;
	TRANSACTION_POINTER = TR_MODE_IDLE;
	TR_COMMAND = CMD_TYPE_NONE;
	bt_can_update = 1;
	clockStyle = CLOCK_STYLE_SIMPLE_MIX;
	msgParsingLine = 0;
	msgParsingChar = 0;
	msgCurDisp = 0;
	iWeek = 1;    // 1: SUN, MON, TUE, WED, THU, FRI,SAT
	iAmPm = 0;    // 0:AM, 1:PM
	iHour = 0;
	iMinutes = 0;
	seconds = 0;
	updateIndicator = 1;
	timeParsingIndex = 0;
	timeBuffer[0] = {-1};
	timeBuffer[1] = {-1};
	timeBuffer[2] = {-1};
	timeBuffer[3] = {-1};
	timeBuffer[4] = {-1};
	timeBuffer[5] = {-1};
	xxx = 0;
	iRadius = 60;
	// Calculate clock pin position
	RAD=3.141592/180;
	LR = 89.99;
	default_timeout = 30;
	screen_timeout = default_timeout;
	clock_needs_redraw = 1;
	menu_needs_redraw = 0;
	screen_changed = 0;
	needredraw = 1;
	screenneedstodim = 0;
	screen_dimmed = 0;
	level = 0;
	
	btn_sel_pressed = 0;
	btn_up_pressed = 0;
	btn_down_pressed = 0;

	init_apps = 1;
	btn_back_pressed = 0;
    buttons_can_be_pressed = 0;
	init_msg_array();
}
void Zerowatch::init_msg_array() {
  for(int i=0; i<MSG_COUNT_MAX; i++) {
    for(int j=0; j<MSG_BUFFER_MAX; j++) {
      msgBuffer[i][j] = 0x00;
    }
  }
  msgParsingLine = 0;
  msgParsingChar = 0;    // First 2 byte is management byte
  msgCurDisp = 0;
}

void Zerowatch::screen_init() {
	ucg.begin(UCG_FONT_MODE_TRANSPARENT);
	ucg.setRotate180();
	ucg.setColor(0, 0,0,0);
	ucg.clearScreen();
	ucg.setFont(font1);
	ucg.setColor(255,255,255);
	ucg.setPrintPos(44,80);
	ucg.print("Zerowatch");
	ucg.setPrintPos(20,95);
	ucg.print("By Ramon Schepers");
	ucg.setPrintPos(48,110);
	ucg.print("(Pix-OS)");
	ucg.setColor(20,20,255);
	ucg.drawBox(14, 10, 66, 34);
	ucg.drawBox(14, 46, 32, 16);
	ucg.drawBox(48, 46, 32, 16);
	ucg.drawBox(82, 46, 32, 16);
	ucg.drawBox(82, 10, 32, 16);
	ucg.drawBox(82, 28, 32, 16);
}

void Zerowatch::setup_watchface_1() {
  ucg.clearScreen();
  ucg.setColor(20,20,255);
  uint8_t y = 5;
  ucg.drawRBox(5,y,118,20,8);
  ucg.setColor(20,20,20);
  ucg.drawRFrame(5,y,118,20,8);
  y+=100;
  ucg.setColor(20,20,255);
  ucg.drawRBox(5,y,118,20,8);
  ucg.setColor(20,20,20);
  ucg.drawRFrame(5,y,118,20,8);
  
  y = 40;
  ucg.setColor(255,255,255);
  ucg.setFont(font1);
  for (int i = 0; i < MSG_COUNT_MAX; i++){
    drawMessage(i, y);
    y+=10;
  }
  needredraw = 0;
}

void Zerowatch::drawMessage(int message, int y) {
  ucg.setPrintPos(10,y);
  for(int i=0; i<MSG_BUFFER_MAX; i++) {
    char curChar = msgBuffer[message][i];
    if(curChar == 0x00) break;
    if(curChar >= 0xf0) continue;
    ucg.write(curChar);
  }
}

void Zerowatch::watchface_1() {
  ucg.setFont(font2);
  ucg.setColor(20,20,255);
  ucg.drawBox(15,8,100,16);
  ucg.setColor(255,255,255);
  ucg.setPrintPos(30,22);
  if (iHour < 10) {
    ucg.print("0");
  }
  ucg.print(iHour);
  ucg.print(":");
  if (iMinutes < 10) {
    ucg.print("0");
  }
  ucg.print(iMinutes);
  ucg.print(":");
  if (seconds < 10) {
    ucg.print("0");
  }
  ucg.print(seconds);
  ucg.setColor(20,20,255);
  ucg.drawBox(15,108,100,16);
  ucg.setColor(255,255,255);
  ucg.setPrintPos(12,122);
  if (iAmPm == 0) {
    ucg.print("AM");
  } else {
    ucg.print("PM");
  }
  ucg.setPrintPos(80,122);
  if (iWeek == 1) {
    ucg.print("Sun");
  } else if (iWeek == 2) {
    ucg.print("Mon");
  } else if (iWeek == 3) {
    ucg.print("Tue");
  } else if (iWeek == 4) {
    ucg.print("Wed");
  } else if (iWeek == 5) {
    ucg.print("Thu");
  } else if (iWeek == 6) {
    ucg.print("Fri");
  } else if (iWeek == 7) {
    ucg.print("Sat");
  }
  /*
    ucg.setColor(0,0,0);
    ucg.drawBox(10,60,40,10);
    ucg.setPrintPos(10,60);
    ucg.setColor(255,255,255);
    ucg.setFont(font1);
    ucg.print("Battery (%): ");
    ucg.setPrintPos(10,70);
    ucg.print(GetBatteryPercent());
*/  
  clock_needs_redraw = 0;
}

//button handling
int Zerowatch::button_thread1(struct pt *pt, int interval) {
  static unsigned long timeout = 0;
  PT_BEGIN(pt); 
  while(1){
    // ensure that the pin is high when you start
    PT_WAIT_UNTIL(pt, digitalRead(back_btn) == HIGH); 
    // wait until pin goes low
    PT_WAIT_UNTIL(pt, digitalRead(back_btn) == LOW); 
    // insert delay here for minimum time the pin must be high
    timeout = millis() + 500; //in ms
    // wait until the delay has expired
    PT_WAIT_UNTIL(pt, timeout - millis() > 0); 
    //PT_WAIT_UNTIL(pt, digitalRead(back_btn) == LOW); 
    // wait until the pin goes high again
    PT_WAIT_UNTIL(pt, digitalRead(back_btn) == HIGH); 
    // call the button callback
    if (!screen_dimmed) {
      if (level == 1) {
        level=0;
        needredraw = 1;
        screen_changed = 1;
        menu_needs_redraw = 1;
      }
    } else {
		ucg.begin(UCG_FONT_MODE_TRANSPARENT);
		ucg.setRotate180();
		level = 0;
        screen_changed = 1;
		needredraw = 1;
	}
    screen_timeout = default_timeout;
    screen_dimmed = 0;
    screenneedstodim = 0;
  }
  PT_END(pt);
}
int Zerowatch::button_thread2(struct pt *pt, int interval) {
  static unsigned long timeout = 0;
  PT_BEGIN(pt); 
  while(1){
    // ensure that the pin is high when you start
    PT_WAIT_UNTIL(pt, digitalRead(sel_btn) == HIGH); 
    // wait until pin goes low
    PT_WAIT_UNTIL(pt, digitalRead(sel_btn) == LOW); 
    // insert delay here for minimum time the pin must be high
    timeout = millis() + 500; //in ms
    // wait until the delay has expired
    PT_WAIT_UNTIL(pt, timeout - millis() > 0); 
    //PT_WAIT_UNTIL(pt, digitalRead(sel_btn) == LOW); 
    // wait until the pin goes high again
    PT_WAIT_UNTIL(pt, digitalRead(sel_btn) == HIGH); 
    // call the button callback
    if (!screen_dimmed) {
      if (level == 0) {
        level = 1;
        screen_changed = 1;
      }
      if (level == 1){
		  if (buttons_can_be_pressed) {
			btn_sel_pressed = 1;
			screen_changed = 1;
		  }
	  }
    } else {
		ucg.begin(UCG_FONT_MODE_TRANSPARENT);
		ucg.setRotate180();
		level = 0;
		needredraw = 1;
	}
    screen_timeout = default_timeout;
    screen_dimmed = 0;
    screenneedstodim = 0;
  }
  PT_END(pt);
}
int Zerowatch::button_thread3(struct pt *pt, int interval) {
  static unsigned long timeout = 0;
  PT_BEGIN(pt); 
  while(1){
    // ensure that the pin is high when you start
    PT_WAIT_UNTIL(pt, digitalRead(up_btn) == HIGH); 
    // wait until pin goes low
    PT_WAIT_UNTIL(pt, digitalRead(up_btn) == LOW); 
    // insert delay here for minimum time the pin must be high
    timeout = millis() + 500; //in ms
    // wait until the delay has expired
    PT_WAIT_UNTIL(pt, timeout - millis() > 0); 
    //PT_WAIT_UNTIL(pt, digitalRead(sel_btn) == LOW); 
    // wait until the pin goes high again
    PT_WAIT_UNTIL(pt, digitalRead(up_btn) == HIGH); 
    // call the button callback
    if (!screen_dimmed) {
    if (level == 1){
      if (buttons_can_be_pressed) {
        btn_up_pressed = 1;
        screen_changed = 1;
      }
    }
    } else {
		ucg.begin(UCG_FONT_MODE_TRANSPARENT);
		ucg.setRotate180();
		level = 0;
		needredraw = 1;
	}
    screen_timeout = default_timeout;
    screen_dimmed = 0;
    screenneedstodim = 0;
  }
  PT_END(pt);
}
int Zerowatch::button_thread4(struct pt *pt, int interval) {
  static unsigned long timeout = 0;
  PT_BEGIN(pt); 
  while(1){
    // ensure that the pin is high when you start
    PT_WAIT_UNTIL(pt, digitalRead(down_btn) == HIGH); 
    // wait until pin goes low
    PT_WAIT_UNTIL(pt, digitalRead(down_btn) == LOW); 
    // insert delay here for minimum time the pin must be high
    timeout = millis() + 500; //in ms
    // wait until the delay has expired
    PT_WAIT_UNTIL(pt, timeout - millis() > 0); 
    //PT_WAIT_UNTIL(pt, digitalRead(sel_btn) == LOW); 
    // wait until the pin goes high again
    PT_WAIT_UNTIL(pt, digitalRead(down_btn) == HIGH); 
    // call the button callback
    if (!screen_dimmed) {
    if (level == 1){
      if (buttons_can_be_pressed) {
        btn_down_pressed = 1;
        screen_changed = 1;
      }
    }
    } else {
		ucg.begin(UCG_FONT_MODE_TRANSPARENT);
		ucg.setRotate180();
		level = 0;
		needredraw = 1;
	}
    screen_timeout = default_timeout;
    screen_dimmed = 0;
    screenneedstodim = 0;
  }
  PT_END(pt);
}


//bluetooth updating
int Zerowatch::protothread5(struct pt *pt, int interval) {
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  while(1) {
    PT_WAIT_UNTIL(pt, millis() - timestamp > interval );
    timestamp = millis(); // take a new timestamp
    //run thread at given ms in loop
    if(!bt_can_update) {
      bt_can_update = 1;
    }
  }
  PT_END(pt);
}
//bluetooth updating
int Zerowatch::protothread8(struct pt *pt, int interval) {
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  while(1) {
    PT_WAIT_UNTIL(pt, millis() - timestamp > interval );
    timestamp = millis(); // take a new timestamp
    //run thread at given ms in loop
	__clock();
	}
  PT_END(pt);
}
int Zerowatch::protothread6(struct pt *pt, int interval) {
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  while(1) { // never stop 
    /* each time the function is called the second boolean
    *  argument "millis() - timestamp > interval" is re-evaluated
    *  and if false the function exits after that. */
    PT_WAIT_UNTIL(pt, millis() - timestamp > interval );
    timestamp = millis(); // take a new timestamp
    //run thread at given ms in loop
    update_clock();
  }
  PT_END(pt);
}
int Zerowatch::protothread7(struct pt *pt, int interval) {
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  while(1) { // never stop 
    /* each time the function is called the second boolean
    *  argument "millis() - timestamp > interval" is re-evaluated
    *  and if false the function exits after that. */
    PT_WAIT_UNTIL(pt, millis() - timestamp > interval );
    timestamp = millis(); // take a new timestamp
    //run thread at given ms in loop
    update_screen();
  }
  PT_END(pt);
}
void Zerowatch::__Zerowatch_loop() {
	button_thread1(&pt1, 5);
	button_thread2(&pt2, 5);
	button_thread3(&pt3, 5);
	button_thread4(&pt4, 5);
	protothread6(&pt6, 1000);
	protothread7(&pt7, 10);
	protothread8(&pt8, 1000);
	bool isReceived = 0;
	// Receive data from remote and parse
	if (bt_can_update) {
		isReceived = receiveBluetoothData();
	}
	if (!isReceived) {
		protothread5(&pt5, 1000);
		//if no data, wait till next check, saves battery ;)
	}
	if (!screen_dimmed) {
		if (level == 0) {
			init_apps = 1;
			buttons_can_be_pressed = 0;
			if (clock_needs_redraw) {
				if(clockStyle == CLOCK_STYLE_SIMPLE_MIX) {
					if (!screenneedstodim) {
						if (needredraw) {
							setup_watchface_1();
						}
						watchface_1();
					}
				} else {
					if (!screenneedstodim) {
						if (needredraw) {
							setup_watchface_1();
						}
						watchface_1();
					}
				}
			}
		} else if (level == 1) {
			//menu level 1
			if (init_apps) {
			  ucg.clearScreen();
			  buttons_can_be_pressed = 1;
			  menu_needs_redraw = 0;
			  _shell.init();
			  init_apps = 0;
			}
			if(btn_up_pressed) {
				if (!_shell.btn_up) {
					_shell.btn_up = 1;
				}
				btn_up_pressed = 0;
			}
			if( btn_down_pressed) {
				if (!_shell.btn_down) {
					_shell.btn_down = 1;
				}
				btn_down_pressed = 0;
			}
			if(btn_sel_pressed) {
				if (!_shell.btn_sel) {
					_shell.btn_sel = 1;
				}
				btn_sel_pressed = 0;
			}
			if (btn_back_pressed) {
				btn_back_pressed = 0;
				clock_needs_redraw = 0;
				needredraw = 1;
				screen_changed = 1;
				menu_needs_redraw = 1;
				screenneedstodim = 0;
				_shell.exit();
			}
			_shell.Loop();
		}
	} else {
		if (screenneedstodim) {
			//ucg.clearScreen();
			screen_powerdown();
			screenneedstodim = 0;
			if (!needredraw) {
				needredraw = 1;
			}
		}
	}
}

void Zerowatch::screen_powerdown() {
        digitalWrite(6, LOW);
        //digitalWrite(6, HIGH);
		
        digitalWrite(7, LOW);
        digitalWrite(8, LOW);
		
        delay(50);
		SPI.end();
}

bool Zerowatch::GetAppName(int index, char* name, int len) {
	return Shell_GetAppName(index, name, len);
}

int Zerowatch::GetBatteryPercent() {
	long d = analogRead(A5);
	d *= 1652;
	uint8_t val = map(d >> 8,3700,4200,0,100);
	if (val > 150) {
		val = 0;
	}
	if (val < 0) {
		val = 0;
	}
		
	if (val > 100) {
		val = 100;
	}
	return val;	//Battery divider scaled to millivolts from reference
}

int Zerowatch::AppCount() {
 return Shell_AppCount();
}

void Zerowatch::update_screen() {
 if (level == 0) {
   if (screen_changed) {
     if (clock_needs_redraw == 0) {
        clock_needs_redraw = 1;
      }
      screen_changed = 0;
    }
  }
}

void Zerowatch::update_clock() {
	if (level == 0) {
		if (clock_needs_redraw == 0) {
			clock_needs_redraw = 1;
		}
	}
}

//***************************************************
//bluetooth data parsing
//***************************************************
// Parsing packet according to current mode
bool Zerowatch::receiveBluetoothData() {
  int isTransactionEnded = false;
  while(!isTransactionEnded) {
    if(BT.available()) {
      byte c = BT.read();
      
      if(c == 0xFF && TRANSACTION_POINTER != TR_MODE_WAIT_MESSAGE) return false;
      
      if(TRANSACTION_POINTER == TR_MODE_IDLE) {
        parseStartSignal(c);
      }
      else if(TRANSACTION_POINTER == TR_MODE_WAIT_CMD) {
        parseCommand(c);
      }
      else if(TRANSACTION_POINTER == TR_MODE_WAIT_MESSAGE) {
        parseMessage(c);
      }
      else if(TRANSACTION_POINTER == TR_MODE_WAIT_TIME) {
        parseTime(c);
      }
      else if(TRANSACTION_POINTER == TR_MODE_WAIT_ID) {
        parseId(c);
      }
      else if(TRANSACTION_POINTER == TR_MODE_WAIT_COMPLETE) {
        isTransactionEnded = parseEndSignal(c);
      }
      return true;
    }  // End of if(BTSerial.available())
    else {
      isTransactionEnded = true;
    }
  }  // End of while()
  return false;
}  // End of receiveBluetoothData()


void Zerowatch::parseStartSignal(byte c) {
  //drawLogChar(c);
  if(c == TRANSACTION_START_BYTE) {
    TRANSACTION_POINTER = TR_MODE_WAIT_CMD;
    TR_COMMAND = CMD_TYPE_NONE;
  }
}


void Zerowatch::parseCommand(byte c) {
  if(c == CMD_TYPE_RESET_NORMAL_OBJ) {
    TRANSACTION_POINTER = TR_MODE_WAIT_COMPLETE;
    TR_COMMAND = c;
    processTransaction();
    if (level == 0) {
        needredraw =1;
        screen_timeout = default_timeout;
        screen_dimmed = 0;
        screenneedstodim = 0;
    }
  }
  else if(c == CMD_TYPE_ADD_NORMAL_OBJ) {
    TRANSACTION_POINTER = TR_MODE_WAIT_MESSAGE;
    TR_COMMAND = c;
    if(c == CMD_TYPE_ADD_NORMAL_OBJ) {
      msgParsingChar = 0;
      if(msgParsingLine >= MSG_COUNT_MAX || msgParsingLine < 0)
        msgParsingLine = 0;
    }
  }
  else if(c == CMD_TYPE_DELETE_NORMAL_OBJ) {
    TRANSACTION_POINTER = TR_MODE_WAIT_COMPLETE;
    TR_COMMAND = c;
  }
  else if(c == CMD_TYPE_SET_TIME) {
    TRANSACTION_POINTER = TR_MODE_WAIT_TIME;
    TR_COMMAND = c;
  }
  else if(c == CMD_TYPE_SET_CLOCK_STYLE || c == CMD_TYPE_SET_INDICATOR) {
    TRANSACTION_POINTER = TR_MODE_WAIT_ID;
    TR_COMMAND = c;
  }
  else {
    TRANSACTION_POINTER = TR_MODE_IDLE;
    TR_COMMAND = CMD_TYPE_NONE;
  }
}

void Zerowatch::parseMessage(byte c) {
  if(c == TRANSACTION_END_BYTE) {
    processTransaction();
    TRANSACTION_POINTER = TR_MODE_IDLE;
  }
  if(TR_COMMAND == CMD_TYPE_ADD_NORMAL_OBJ) {
    if(msgParsingChar < MSG_BUFFER_MAX) {
      if(msgParsingChar > 1) {
        msgBuffer[msgParsingLine][msgParsingChar] = c;
      }
      msgParsingChar++;
    }
    else {
      TRANSACTION_POINTER = TR_MODE_WAIT_COMPLETE;
    }
  }
}


void Zerowatch::parseTime(byte c) {
  if(TR_COMMAND == CMD_TYPE_SET_TIME) {
    if(timeParsingIndex >= 0 && timeParsingIndex < TIME_BUFFER_MAX) {
      timeBuffer[timeParsingIndex] = (int)c;
      timeParsingIndex++;
    }
    else {
      processTransaction();
      TRANSACTION_POINTER = TR_MODE_WAIT_COMPLETE;
    }
  }
}

void Zerowatch::parseId(byte c) {
  if(TR_COMMAND == CMD_TYPE_SET_CLOCK_STYLE) {
    clockStyle = c;
    needredraw = 1;
    clock_needs_redraw = 1;
    processTransaction();
  }
  else if(TR_COMMAND == CMD_TYPE_SET_INDICATOR) {
    if(c == INDICATOR_ENABLE)
      updateIndicator = true;
    else
      updateIndicator = false;
    processTransaction();
  }
  TRANSACTION_POINTER = TR_MODE_WAIT_COMPLETE;
}

void Zerowatch::setTimeValue() {
  iWeek = timeBuffer[2];    // 1: SUN, MON, TUE, WED, THU, FRI,SAT
  iAmPm = timeBuffer[3];    // 0:AM, 1:PM
  iHour = timeBuffer[4];
  iMinutes = timeBuffer[5];
}

bool Zerowatch::parseEndSignal(byte c) {
  if(c == TRANSACTION_END_BYTE) {
    TRANSACTION_POINTER = TR_MODE_IDLE;
    return true;
  }
  return false;
}

void Zerowatch::processTransaction() {
  if(TR_COMMAND == CMD_TYPE_RESET_NORMAL_OBJ) {
    init_msg_array();//init_emg_array();
  }
  else if(TR_COMMAND == CMD_TYPE_ADD_NORMAL_OBJ) {
    msgBuffer[msgParsingLine][0] = 0x01;
    msgParsingChar = 0;
    msgParsingLine++;
    if(msgParsingLine >= MSG_COUNT_MAX)
      msgParsingLine = 0;
    // update screen immediately
  }
  else if(TR_COMMAND == CMD_TYPE_DELETE_NORMAL_OBJ) {
    // Not available yet.
  }
  else if(TR_COMMAND == CMD_TYPE_SET_TIME) {
    setTimeValue();
    timeParsingIndex = 0;
    // update screen immediately
  }
  if(TR_COMMAND == CMD_TYPE_SET_CLOCK_STYLE || CMD_TYPE_SET_INDICATOR) {
    // update screen immediately
  }
}


void Zerowatch::Shell_LaunchApp(char* params)
{
  _shell.Launch(params);
}



void Zerowatch::clear_screen()
{
  ucg.clearScreen();
}


void Zerowatch::setColor(uint8_t r, uint8_t g, uint8_t b)
{
  ucg.setColor(r,g,b);
}

void Zerowatch::draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  ucg.drawFrame(x,y,w,h);
}
void Zerowatch::fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  ucg.drawBox(x,y,w,h);
}
void Zerowatch::setPrintPos(uint8_t x, uint8_t y)
{
  ucg.setPrintPos(x,y);
}
void Zerowatch::print(char* text)
{
  ucg.print(text);
}
void Zerowatch::printint(int text)
{
  ucg.print(text);
}
void Zerowatch::setFont(uint8_t type)
{
	if (type == 0) {
		ucg.setFont(font1); //large
	} else if (type == 1) {
		ucg.setFont(font2); //medium
	} else if (type == 2) {
		ucg.setFont(font3); //small
	} else {
		ucg.setFont(font1);
	}
}


