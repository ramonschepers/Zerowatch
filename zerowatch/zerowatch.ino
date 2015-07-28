#include <SPI.h>
#include <pt2.h>
#include "Ucglib.h"
#include "app_main.h"
#include <zerowatch_core.h>

Zerowatch watch;
void setup() {
  watch.__init();
}

void loop() {
  // put your main code here, to run repeatedly:
  watch.__Zerowatch_loop();
}
