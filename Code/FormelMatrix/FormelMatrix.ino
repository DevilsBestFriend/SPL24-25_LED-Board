/**
 * LED-Matrix-Steuerung für SPL24-25_LED-Board
 * 2 Panels à 32x8: oben an P25 (gespiegelt), unten an P26
 * Gemeinsames Mapping zu 32x16-Matrix
 */

#include <FastLED.h>

#define MATRIX_WIDTH     32
#define MATRIX_HEIGHT    16
#define COLOR_ORDER      GRB
#define CHIPSET          WS2812
#define BRIGHTNESS       64

// Pins für obere und untere Panels
#define DATA_PIN_OBEN    25  // P25
#define DATA_PIN_UNTEN   26  // P26

// Gesamtanzahl LEDs pro Panel
#define NUM_LEDS_PANEL   (MATRIX_WIDTH * 8)
#define NUM_LEDS_TOTAL   (NUM_LEDS_PANEL * 2)

// LED-Arrays für beide Panels
CRGB ledsOben[NUM_LEDS_PANEL];
CRGB ledsUnten[NUM_LEDS_PANEL];

// Mapping-Funktion für 32x16-Koordinaten
CRGB& getPixel(int x, int y) {
  // Bereich prüfen
  if (x < 0 || x >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT) {
    static CRGB dummy;
    return dummy;
  }

  // Unteres Panel
  if (y < 8) {
    int realY = y;
    int index = x * 8;

    if (x % 2 == 0)
      index += realY;
    else
      index += 7 - realY;

    return ledsUnten[index];
  }

  // Oberes Panel (Y gespiegelte Logik!)
  else {
    int realY = y - 8;
    int index = x * 8;

    if (x % 2 == 0)
      index += 7 - realY;
    else
      index += realY;

    return ledsOben[index];
  }
}

void setup() {
  FastLED.addLeds<CHIPSET, DATA_PIN_UNTEN, COLOR_ORDER>(ledsUnten, NUM_LEDS_PANEL);
  FastLED.addLeds<CHIPSET, DATA_PIN_OBEN,  COLOR_ORDER>(ledsOben,  NUM_LEDS_PANEL);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();

  // Testmuster: Ecken + Mitte einfärben
  getPixel(0, 0)      = CRGB::Red;      // unten links
  getPixel(31, 0)     = CRGB::Green;    // unten rechts
  getPixel(0, 15)     = CRGB::Blue;     // oben links
  getPixel(31, 15)    = CRGB::Yellow;   // oben rechts
  getPixel(16, 8)     = CRGB::White;    // Mitte

  FastLED.show();
}

void loop() {
  // Keine Animation – statisches Testbild
}
