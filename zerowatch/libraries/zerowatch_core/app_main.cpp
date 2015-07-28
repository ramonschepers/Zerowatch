//***************************************************
//app loading and stuff
//*************************************************** 
/*************************************************** 
  Written by Rossumer for the microtouch.
  Ported to the Zerowatch by Ramon Schepers.
  BSD license, all text above must be included in any redistribution
****************************************************/
#include <string.h>
#include <avr/pgmspace.h>
#include "app_main.h"

//app buffer
#define MAX_APPS 200


int appCount = 0;

typedef struct    // Apps register themselves from static constructors
{
  CONST_PCHAR name;
  AppProc proc;
  int stateLen;
} AppDefinition;

AppDefinition appList[MAX_APPS];
//***************************************************
//utils
//***************************************************

typedef int (*AppProc)(Event* e, void* appState);

void _Shell::init2(){
    force_exit = 0;
    btn_up = 0;
    btn_sel = 0;
    btn_down = 0;
    _appResult = 0;
    _proc = 0;
    _appIndex = 0;
    _launch[0] = 0;
  }
  AppProc _Shell::FindApp(const char* name)
  {
    _appIndex = appCount;
    while (_appIndex--)
    {
      if (strcmp(name,appList[_appIndex].name) == 0) // TODO
        return appList[_appIndex].proc;
    }
    _appIndex = 0;
    return appList[0].proc;  // not found, run shell
  }


void _Shell::Loop2()
  {
  if (force_exit) {
    _proc = 0;
  } else {
    //  Launch an app with params based on name
    if (_proc == 0 || _launch[0])
    {
      if (!_launch[0])
        strncpy(_launch,appList[0].name,sizeof(_launch));  // back to shell

      _proc = FindApp(_launch);
      Event e;
      e.Type = Event::OpenApp;
      _appResult = _proc(&e,_buffer);    // Open app
      _launch[0] = 0;
    }
   
   if (btn_up) {Event e;
        btn_up = 0;
        e.Type = Event::buttn_up;
        _appResult = _proc(&e,_buffer);
   } else if (btn_sel) {Event e;
        btn_sel = 0;
        e.Type = Event::buttn_sel;
        _appResult = _proc(&e,_buffer);
   } else if (btn_down) {Event e;
        btn_down = 0;
        e.Type = Event::buttn_down;
        _appResult = _proc(&e,_buffer);
   } else {Event e;
        e.Type = Event::None;
        _appResult = _proc(&e,_buffer);
   }
    if (_appResult != 0)
    {
      // Save state
      //SaveStateEvent s;
      //_proc(&s,_buffer,sizeof(_buffer));
      _proc = 0;      // App is terminating
    }
 }
 }
 
 const char* _Shell::AppName()
  {
    return appList[_appIndex].name;
  }
  
  void _Shell::Launch(const char* app)
  {
    strncpy(_launch,app,sizeof(_launch));
  }
  void _Shell::exit() {
    force_exit =1;
  }
  void _Shell::Loop() {
    Loop2();
  }
  void _Shell::init() {
    init2();
  }
//  Default app is shell
int proc_shell(Event* e, void* state);

//  Auto register the app with the Shell, will be called before Shell_Init
RegisterApp::RegisterApp(const char* name, AppProc proc, int stateLen)
{
  if (appCount == MAX_APPS || (stateLen > MAX_APP_BUFFER))
    return;

  AppDefinition& app = appList[appCount++];
  app.name = name;
  app.proc = proc;
  app.stateLen = stateLen;

  //  Swap shell app to first in list
  if (proc_shell == proc)
  {
    AppDefinition tmp = appList[0];
    appList[0] = app;
    app = tmp;
  }
};
bool Shell_GetAppName(int index, char* name, int len)
{
  if (index < 0 || index >= appCount) {
    return false;
  }
  strncpy(name,appList[index].name,len);
  return true;
}

int Shell_AppCount()
{
  return appCount;
}




