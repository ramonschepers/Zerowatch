/*

  fb.ino
  
  A very simple example for a framebuffer using the SSD1351, code made by ramon schepers

  Universal uC Color Graphics Library
  
  Copyright (c) 2014, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

  18 Feb 2014:
    ROM Size	Font Mode
    9928		NONE
    10942		TRANSPARENT	(1014 Bytes)
    11650		SOLID  (1712 Bytes)
    12214		TRANSPARENT+SOLID
  
  
*/

#include "SdFatUtil.h"
#include <SPI.h>
#include "Ucglib.h"

Ucglib_SSD1351_18x128x128_HWSPI ucg(/*cd=*/ 7 , /*cs=*/ 8, /*reset=*/ 6);

uint8_t fb_pixeldata_red[128][128]; //set all pixels to white
uint8_t fb_pixeldata_green[128][128]; //set all pixels to white
uint8_t fb_pixeldata_blue[128][128]; //set all pixels to white

uint8_t col = 255;

void pushscreen(ucg_t *ucg) {
  for (int i = 0; i<128; i++) {
    for (int j = 0; j<128; j++) {
      ucg->arg.pixel.rgb.color[0] = fb_pixeldata_red[i][j];
      ucg->arg.pixel.rgb.color[1] = fb_pixeldata_green[i][j];
      ucg->arg.pixel.rgb.color[2] = fb_pixeldata_blue[i][j];
      ucg->arg.pixel.pos.x = i;
      ucg->arg.pixel.pos.y = j;
      ucg_DrawPixelWithArg(ucg);
    }
  }
}

void setup(void)
{
  delay(1000);
  Serial.begin(115200);
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  for (int i = 0; i<128; i++) {
    for (int j = 0; j<128; j++) {
      fb_pixeldata_red[i][j] = 255;
      fb_pixeldata_green[i][j] = 255;
      fb_pixeldata_blue[i][j] = 255;
    }
  }
}
void loop(void)
{
  uint32_t timestamp = millis();
  pushscreen(ucg.getUcg());
  uint32_t timestamp2 = millis();
  Serial.print(F("A screen refresh took: "));
  Serial.print((timestamp2-timestamp));
  Serial.println(F(" ms."));
  Serial.print(F("Used kilobytes of RAM: "));
  Serial.println(map(FreeRam(),0,98304,96,0));
  delay(500);
  col -=10;
  for (int i = 0; i<128; i++) {
    for (int j = 0; j<128; j++) {
      fb_pixeldata_red[i][j] = col;
      fb_pixeldata_green[i][j] = col;
      fb_pixeldata_blue[i][j] = col;
    }
  }
  if (col < 20) {
    col = 255;
  }
}