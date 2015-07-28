class hwState
{
  #define __DELAY 500
  u8 __RDELAY;
  u8 __color;
  u8 __color1;
  u8 __color2;
  u8 __button_up_delay;
  u8 __button_down_delay;
  u8 __button_select_delay;
public:
  void __init_vars() {
    __color = 0;
    __RDELAY = 180;
    __color1 = 0;
    __color2 = 0;
    __button_up_delay = 0;
    __button_down_delay = 0;
    __button_select_delay = 0;
  }
  inline void __init_screen() {
    watch.clear_screen();
    watch.setFont(2);
    watch.setColor(255,255,255);
    watch.setPrintPos(0,10);
    watch.print("Hardware test!");
    watch.setPrintPos(0,20);
    watch.print("Used RAM (kb): ");
    watch.setPrintPos(0,30);
    watch.printint(watch.ram_usage(1));
    watch.setPrintPos(0,40);
    watch.print("Used RAM (%): ");
    watch.setPrintPos(0,50);
    watch.printint(watch.ram_usage(0));
  }

  
  int OnEvent(Event* e)
  {
    switch (e->Type)
    {
      case Event::OpenApp:
        __init_screen();
        __init_vars();
        break;

      case Event::buttn_up:
        if (__button_up_delay == 0) {
          __button_up_delay = __DELAY;
          __color = 255;
        }
        break;

        
      case Event::buttn_down:
        if (__button_down_delay == 0) {
          __button_down_delay = __DELAY;
          __color1 = 255;
        }
        break;

      case Event::buttn_sel:
        if (__button_select_delay == 0) {
          __button_select_delay = __DELAY;
          __color2 = 255;
        }
        break;
      case Event::None:
        delay(1);

        if (__RDELAY > 0) {
          __RDELAY--;
          if (__RDELAY == 0) {
            __RDELAY = 180;
            watch.setColor(0,0,0);
            watch.fill_rect(0,20,50,10);
            watch.fill_rect(0,40,50,10);
            watch.setColor(255,255,255);
            watch.setPrintPos(0,30);
            watch.printint(watch.ram_usage(1)); //show used kilobytes ;)
            watch.setPrintPos(0,50);
            watch.printint(watch.ram_usage(0)); //show used percent ;)
          }
        }
        if (__button_up_delay > 0) {
          __button_up_delay--;
          watch.setColor(__color,__color,__color);
          watch.fill_rect(120,0,8,8);
          if (__color > 0) {
            __color--;
          }
        }
        if (__button_down_delay > 0) {
          __button_down_delay--;
          watch.setColor(__color1,__color1,__color1);
          watch.fill_rect(120,120,8,8);
          if (__color1 > 0) {
            __color1--;
          }
        }
        if (__button_select_delay > 0) {
          __button_select_delay--;
          watch.setColor(__color2,__color2,__color2);
          watch.fill_rect(120,60,8,8);
          if (__color2 > 0) {
            __color2--;
          }
        }
        break;

      default:
        ;
    }
    return 0;
  }
};
INSTALL_APP(test_hardware,hwState);

