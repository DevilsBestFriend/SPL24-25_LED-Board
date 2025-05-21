#include "TextScroller.h"
#include <FastLED.h>

cLEDText ScrollingMsg;
String displayText = "Hello World! ";

void initText() {
  ScrollingMsg.SetFont(MatriseFontData);
  ScrollingMsg.Init((cLEDMatrixBase*)&leds, leds.Width(), ScrollingMsg.FontHeight() + 1, 0, 0);
}

void textTask(void *pvParameters) {
  initText();
  
  while(1) {
    ScrollingMsg.SetText((unsigned char*)displayText.c_str(), displayText.length());
    if(ScrollingMsg.UpdateText() == -1) {
      ScrollingMsg.ResetText();
    }
    FastLED.show();
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}
