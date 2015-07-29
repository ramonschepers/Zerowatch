class hwState
{
public:
  #define __DELAY 1000
  uint8_t __RDELAY;
  uint8_t __color;
  uint8_t __color1;
  uint8_t __color2;
  uint8_t cal1;
  uint8_t cal2;
  uint8_t cal3;
  uint8_t cal4;
  uint8_t cal5;
  uint8_t cal6;
  uint8_t cal7;
  uint8_t cal8;
  uint8_t cal_average;
  uint8_t __button_up_delay;
  uint8_t __button_down_delay;
  uint8_t __button_select_delay;
  void __init_vars() {
    cal1 = watch.GetBatteryPercent();
    cal2 = watch.GetBatteryPercent();
    cal3 = watch.GetBatteryPercent();
    cal4 = watch.GetBatteryPercent();
    cal5 = watch.GetBatteryPercent();
    cal6 = watch.GetBatteryPercent();
    cal7 = watch.GetBatteryPercent();
    cal8 = watch.GetBatteryPercent();
    cal_average = (cal1+cal2+cal3+cal4)/4;
    __color = 0;
    __RDELAY = 600;
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
    watch.setPrintPos(0,60);
    watch.print("Battery (%): ");
    watch.setPrintPos(0,70);
    watch.printint(cal_average);
  }

  
  int OnEvent(Event* e)
  {
    switch (e->Type)
    {
      case Event::OpenApp:
        __init_vars();
        __init_screen();
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
            __RDELAY = 600;
            cal1 = watch.GetBatteryPercent();
            cal2 = watch.GetBatteryPercent();
            delay(30);
            cal3 = watch.GetBatteryPercent();
            cal4 = watch.GetBatteryPercent();
            delay(30);
            cal5 = watch.GetBatteryPercent();
            cal6 = watch.GetBatteryPercent();
            delay(30);
            cal7 = watch.GetBatteryPercent();
            cal8 = watch.GetBatteryPercent();
            cal_average = (cal1+cal2+cal3+cal4)/4;
            watch.setColor(0,0,0);
            watch.fill_rect(0,20,50,10);
            watch.fill_rect(0,40,50,10);
            watch.fill_rect(0,60,50,10);
            watch.setColor(255,255,255);
            watch.setPrintPos(0,30);
            watch.printint(watch.ram_usage(1)); //show used kilobytes ;)
            watch.setPrintPos(0,50);
            watch.printint(watch.ram_usage(0)); //show used percent ;)
            watch.setPrintPos(0,70);
            watch.printint(cal_average);
          }
        }
        if (__button_up_delay > 0) {
          __button_up_delay--;
          watch.setColor(__color,__color,__color);
          watch.fill_rect(120,0,8,8);
          if (__color > 1) {
            __color-=2;
          }
        }
        if (__button_down_delay > 0) {
          __button_down_delay--;
          watch.setColor(__color1,__color1,__color1);
          watch.fill_rect(120,120,8,8);
          if (__color1 > 1) {
            __color1-=2;
          }
        }
        if (__button_select_delay > 0) {
          __button_select_delay--;
          watch.setColor(__color2,__color2,__color2);
          watch.fill_rect(120,60,8,8);
          if (__color2 > 1) {
            __color2-=2;
          }
        }
        break;

      default:
        ;
    }
    return 0;
  }
};
INSTALL_APP(hardware_test,hwState);
