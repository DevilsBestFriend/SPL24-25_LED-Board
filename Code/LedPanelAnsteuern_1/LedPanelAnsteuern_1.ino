#include <FastLED.h>
#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>

#define LED_PIN 25
#define LED_COUNT 256 // 32x8 Matrix
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 8
#define BRIGHTNESS 25 // 10% von 255
#define COLOR_ORDER GRB

// LED und Matrix-Setup
CRGB leds[LED_COUNT];
// Matrix im vertikalen Zick-Zack-Muster initialisieren
cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, VERTICAL_ZIGZAG_MATRIX> ledMatrix;
// LEDText für Laufschrift
cLEDText scrollingText;

String inputMessage = " "; // Eingabetext
bool newMessageReceived = false;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    // Warten, bis der serielle Monitor bereit ist (nützlich für Debugging auf einigen Geräten)
  }

  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, LED_COUNT);
  FastLED.clear();
  FastLED.setBrightness(BRIGHTNESS);

  ledMatrix.SetLEDArray(leds);
  scrollingText.SetFont(MatriseFontData);
  scrollingText.Init(&ledMatrix, ledMatrix.Width(), ledMatrix.Height(), 0, 0);
  scrollingText.SetScrollDirection(SCROLL_LEFT);
  scrollingText.SetFrameRate(30); // Framerate für den Text
  scrollingText.SetTextColrOptions(COLR_SINGLE, CRGB::Magenta);

  showStartupSequence();
  Serial.println("Gib eine Nachricht ein und drücke Enter:");
}

void loop() {
  // Nachricht vom seriellen Monitor einlesen
  handleSerialInput();

  // Falls neue Nachricht empfangen wurde, Laufschrift initialisieren
  if (newMessageReceived) {
    initializeScrollingText(inputMessage);
    newMessageReceived = false;
  }

  // Laufschrift anzeigen
  if (scrollingText.UpdateText()) {
    FastLED.show();
  }
}

void showStartupSequence() {
  // Zeige nacheinander Rot, Grün, Blau, Weiß für 1 Sekunde
  fillMatrixWithColor(CRGB::Red);
  delay(1000);
  fillMatrixWithColor(CRGB::Green);
  delay(1000);
  fillMatrixWithColor(CRGB::Blue);
  delay(1000);
  fillMatrixWithColor(CRGB::White);
  delay(1000);
  FastLED.clear();
  FastLED.show();
}

void fillMatrixWithColor(const CRGB& color) {
  for (int i = 0; i < LED_COUNT; i++) {
    leds[i] = color;
  }
  FastLED.show();
}

void handleSerialInput() {
  if (Serial.available() > 0) {
    String receivedMessage = Serial.readStringUntil('\n');
    receivedMessage.trim(); // Entfernt führende und folgende Leerzeichen
    if (receivedMessage.length() > 0) {
      inputMessage = "    " + receivedMessage; // Fügt Leerzeichen vor der Nachricht hinzu
      newMessageReceived = true;
    }
  }
}

void initializeScrollingText(const String& message) {
  char textBuffer[message.length() + 1];
  message.toCharArray(textBuffer, message.length() + 1);
  scrollingText.SetText((unsigned char*)textBuffer, message.length());
}
