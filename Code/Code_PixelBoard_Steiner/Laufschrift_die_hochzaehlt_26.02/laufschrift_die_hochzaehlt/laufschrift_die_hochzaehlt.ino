#include <FastLED.h>
#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>

#define LED_PIN        25
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B

#define MATRIX_WIDTH   36
#define MATRIX_HEIGHT  8
#define MATRIX_TYPE    VERTICAL_ZIGZAG_MATRIX

cLEDMatrix<-MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;
cLEDText ScrollingMsg;

String TxtDemo = "Wert: ";
int x = 0;

void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());
  FastLED.setBrightness(10);
  FastLED.clear(true);
  delay(500);
  FastLED.show();

  ScrollingMsg.SetFont(MatriseFontData);
  ScrollingMsg.Init(&leds, leds.Width(), ScrollingMsg.FontHeight() + 1, 0, 0);
  ScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0x00, 0xff);
}

void loop() {
  String xString = String(x);
  String updatedText = TxtDemo + xString;
  
  ScrollingMsg.SetText((unsigned char *)updatedText.c_str(), updatedText.length());

  while (ScrollingMsg.UpdateText() == 0) {
    FastLED.show();
    delay(50);  // Scrolling-Geschwindigkeit anpassen
  }

  x++;
  if (x > 99) x = 0;  // Reset x wenn es 100 erreicht
  delay(1000);  // 1 Sekunde warten
}
