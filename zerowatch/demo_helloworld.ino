class hiState
{
public:
  int OnEvent(Event* e)
  {
    switch (e->Type)
    {
      case Event::OpenApp:
        watch.clear_screen();
        watch.setFont(1);
        watch.setColor(255,255,255);
        watch.setPrintPos(0,16);
        watch.print("Hello world!");
        break;
        
      case Event::None:
        delay(500);
        return -1;
        break;

      default:
        ;
    }
    return 0;
  }
};
INSTALL_APP(hello_world,hiState);
