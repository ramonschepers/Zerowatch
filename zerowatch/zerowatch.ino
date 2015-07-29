
#include <zerowatch_core.h>
#include "app_main.h"
#include <SPI.h>
#include <pt2.h>
#include "Ucglib.h"

Zerowatch watch;
void setup() {
  watch.__init();
}

void loop() {
  // put your main code here, to run repeatedly:
  watch.__Zerowatch_loop();
}
