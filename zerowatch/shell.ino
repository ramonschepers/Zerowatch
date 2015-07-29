class ShellState
{
#define CELL_WIDTH 128
#define CELL_HEIGHT 12
#define CELL_COLS 1
#define MAX_SHELL_CHARS 20
#define MAX_PAGES 9
#define APPS_PER_PAGE 11
public:
  uint8_t page;
  uint8_t appCount; //how many apps per page?
  uint8_t _last;
  uint8_t x;
  uint8_t y;
  uint8_t apps_on_page;
  uint8_t xxxx;
  uint8_t _needredraw;
  uint8_t tot_pages;
  uint8_t _appMap[APPS_PER_PAGE];
  uint8_t _appMap2[MAX_PAGES][APPS_PER_PAGE];
  uint8_t sel;
  uint8_t i;
  uint8_t j;
  uint8_t gui_changed;
  uint8_t k;
  
  void GetName(uint8_t index, char* name, int len)
  {
    if (index < appCount)
    {
      watch.GetAppName(_appMap[index],name,len);
    }
  }
  void Draw(uint8_t page, uint8_t index, bool hilite)
  {
    x = index % CELL_COLS;
    y = index / CELL_COLS;
    x = x*CELL_WIDTH;// + CELL_WIDTH /2;
    y = y*CELL_HEIGHT;// + CELL_HEIGHT /2;
    if (hilite) {
      watch.setColor(255,0,0);
      watch.draw_rect(x,y,CELL_WIDTH,CELL_HEIGHT);
      watch.draw_rect(x+1,y,CELL_WIDTH-2,CELL_HEIGHT);
    } else {
      watch.setColor(255,255,255);
      watch.draw_rect(x,y,CELL_WIDTH,CELL_HEIGHT);
      watch.draw_rect(x+1,y,CELL_WIDTH-2,CELL_HEIGHT);
    }
  }

  void Draw2(uint8_t page, uint8_t index, bool hilite)
  {
    char name[32];
    Shell_GetAppName(_appMap2[page][index],name,sizeof(name));
    x = index % CELL_COLS;
    y = index / CELL_COLS;
    x = x*CELL_WIDTH;// + CELL_WIDTH /2;
    y = y*CELL_HEIGHT;// + CELL_HEIGHT /2;
    if (hilite) {
      watch.setColor(255,0,0);
      watch.draw_rect(x,y,CELL_WIDTH,CELL_HEIGHT);
      watch.draw_rect(x+1,y,CELL_WIDTH-2,CELL_HEIGHT);
    } else {
      watch.setColor(255,255,255);
      watch.draw_rect(x,y,CELL_WIDTH,CELL_HEIGHT);
      watch.draw_rect(x+1,y,CELL_WIDTH-2,CELL_HEIGHT);
    }
    watch.setColor(0,0,0);
    watch.fill_rect(x+2,y+1,CELL_WIDTH-4,CELL_HEIGHT-2);
    watch.setColor(255,255,255);
    watch.setFont(3);
    watch.setPrintPos(x+5,y+8);
    watch.print(name);
    watch.setPrintPos(x+5,126);
  }

  int OnEvent(Event* e)
  {
    switch (e->Type)
    {
     case Event::OpenApp:
        _needredraw = 0;
        apps_on_page = 0;
        tot_pages = 1;
        xxxx = 0;
        sel = 0;
        gui_changed=0;
        page = 0;
        watch.clear_screen();
        j = 0;
            //  Count built in apps
            appCount = 0;
            k = Shell_AppCount();
            for (i = 1; i < k; i++)
            {
              char name[32];              
              GetName(_appMap[appCount],name,sizeof(name));
              _appMap[appCount++] = i;
              if (appCount >= (APPS_PER_PAGE*MAX_PAGES)){
                break;
              }
            }
            for (i = 1; i < Shell_AppCount(); i++){
              char name[32];
              GetName(_appMap[appCount],name,sizeof(name));

              if ((i %APPS_PER_PAGE) == 0) {
                page++;
                tot_pages++;
                j = 0;
                if (page+1 == MAX_PAGES) {
                  break;
                }
              }
              _appMap2[page][j] = i;
              j++;
             }
            page = 0;
            xxxx = 0;
            i = APPS_PER_PAGE-1;
            while (i--)
            {
              if (i == sel) {
                Draw2(page, i,true);
              } else {
                Draw2(page, i,false);
              }
            }
          page = 0;
          i = 0;
          j = 0;
       break;

      case Event::None:
        if (_needredraw) {
           i = APPS_PER_PAGE-1;
           while (i--) {
                  if (i == sel) {
                    Draw2(page, i,true);
                  } else {
                    Draw2(page, i,false);
                  }
          }
          _needredraw = 0;
        }
        if (gui_changed) {
          gui_changed = 0;
          i = APPS_PER_PAGE-1;
          while (i--) {
                  if (i == sel) {
                    Draw(page, i,true);
                  } else {
                    Draw(page, i,false);
                  }
          }
          }
        break;

      case Event::buttn_up:
        xxxx = page;
        if (sel > 0) {
          sel--;
        } else {
          if (page > 0) {
            page-=1;
            if (xxxx != page) {
              sel = APPS_PER_PAGE-2;
              _needredraw = 1;
              if (page < 2) {
                page = 1;
              }
            }
          }
        }
        gui_changed = 1;
        break;

      case Event::buttn_down:
      xxxx = page;
        if (sel < APPS_PER_PAGE-2) {
          sel++;
        } else {
          if (page < MAX_PAGES) {
            page++;
            if (page >= tot_pages-1) {
              page = tot_pages-1;
            }
            if (xxxx != page) {
              sel = 0;
              _needredraw = 1;
            }
          }
        }
        gui_changed = 1;
        break;

      case Event::buttn_sel:
        char appname[32];
        GetName(_appMap2[page][sel-1],appname,sizeof(appname));
        watch.Shell_LaunchApp(appname);
        break;

      default:
      ;
    }
    return 0;
  }
};
INSTALL_APP(shell,ShellState);


