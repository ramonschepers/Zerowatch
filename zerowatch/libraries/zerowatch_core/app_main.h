#ifndef __zerowatch_apps_H__
#define __zerowatch_apps_H__


#define MAX_APP_BUFFER 16384 //can be set up to 16384bytes. (16kilobytes)
#include <inttypes.h>
#include <avr/pgmspace.h>

#ifndef abs
#define abs(_x) ((_x) < 0 ? -(_x) : (_x))
#endif
#define CONST_PCHAR const prog_char*
#define ROMSTRING(_s) extern const char _s[] PROGMEM

//***************************************************
//define stuff
//***************************************************
// default Color definitions
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF


class PStr
{
	char _str[32];
public:
	PStr(const char* pstr)
	{
		strncpy(_str,pstr,sizeof(_str));
	}
	operator const char*()
	{
		return _str;
	}
};


class Event
{
public:
  enum EType
    {
        None,
        OpenApp,
        buttn_up,
        buttn_sel,
        buttn_down
    };
  EType Type;
};
typedef int (*AppProc)(Event* e, void* appState);

//  Application installer macro
//  _n is the human readable name of the application
//  _s is an object with a OnEvent method

#define INSTALL_APP(_n,_s) \
	ROMSTRING(S_##_n); \
	const char S_##_n[] = #_n; \
	int proc_##_n(Event* e, void* s) {return ((_s*)s)->OnEvent(e);} \
	RegisterApp app_##_n(S_##_n,(AppProc)proc_##_n,sizeof(_s))

//	Auto register the app with the Shell, will be called before Shell_Init
class RegisterApp
{
public:
	RegisterApp(const char* name, AppProc proc, int stateSize);
};



bool Shell_GetAppName(int index, char* name, int len);
int Shell_AppCount();
void Shell_LaunchApp(const char* params);
void Shell_LaunchFile(const char* filename);

class _Shell
{
  bool open;
  AppProc _proc;
  int _appResult;
  uint8_t  _appIndex;
  char _launch[32];
  unsigned char _buffer[MAX_APP_BUFFER];  // Applications run in this buffer
  bool force_exit;
  void init2();
  AppProc FindApp(const char* name);
 public:
  bool btn_up;
  bool btn_sel;
  bool btn_down;
  const char* AppName();
  void Launch(const char* app);
  void exit();
  void Loop();
  void init();
};

#endif