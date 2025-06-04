#include <FastLED.h>
#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>
#include <WiFi.h>
#include "time.h"

// WLAN-Zugangsdaten
const char* ssid = "iPhone von Hendrik";
const char* password = "hst123456";

// NTP-Server
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600; // GMT+1
const int daylightOffset_sec = 3600; // Sommerzeit

// Ändere die folgenden Defines, um deine Matrix anzupassen
#define LED_PIN_UPPER   25  // Oberes Panel (Uhrzeit)
#define LED_PIN_LOWER   26  // Unteres Panel (Datum)
#define COLOR_ORDER     GRB
#define CHIPSET         WS2812B

#define MATRIX_WIDTH    32
#define MATRIX_HEIGHT   8
#define MATRIX_TYPE     VERTICAL_ZIGZAG_MATRIX

// Zwei separate LED-Matrizen definieren
cLEDMatrix<MATRIX_WIDTH, -MATRIX_HEIGHT, MATRIX_TYPE> upperMatrix;
cLEDMatrix<-MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> lowerMatrix;

// Zwei separate Scrolling-Textobjekte
cLEDText upperScrollingMsg;
cLEDText lowerScrollingMsg;

// Buffer für die anzuzeigenden Texte
unsigned char upperTextBuffer[75]; // Für Uhrzeit
unsigned char lowerTextBuffer[75]; // Für Datum
int upperTextLength = 0;
int lowerTextLength = 0;

// Timer für verschiedene Aktualisierungsintervalle
unsigned long lastTimeUpdate = 0;
unsigned long lastScrollUpdate = 0;
const unsigned long timeUpdateInterval = 1000; // Zeitupdate jede Sekunde
const unsigned long scrollUpdateInterval = 100; // Basis-Scrolling-Intervall

// Variable zum Verfolgen der Scrollposition
int upperScrollCount = 0;
int lowerScrollCount = 0;
int upperTextWidth = 0;
int lowerTextWidth = 0;
bool textNeedsUpdate = false;

void setup() {
  Serial.begin(115200);
  
  // LED Matrix Setup für beide Panels
  FastLED.addLeds<CHIPSET, LED_PIN_UPPER, COLOR_ORDER>(upperMatrix[0], upperMatrix.Size());
  FastLED.addLeds<CHIPSET, LED_PIN_LOWER, COLOR_ORDER>(lowerMatrix[0], lowerMatrix.Size());
  FastLED.setBrightness(10);
  FastLED.clear(true);
  
  // Initialisiere die Laufschrift für das obere Panel (Uhrzeit)
  upperScrollingMsg.SetFont(MatriseFontData);
  upperScrollingMsg.Init(&upperMatrix, upperMatrix.Width(), upperScrollingMsg.FontHeight() + 1, 1, 0);
  upperScrollingMsg.SetScrollDirection(SCROLL_LEFT); // Von rechts nach links für umgedrehtes Panel
  upperScrollingMsg.SetTextDirection(SCROLL_LEFT);   // Text Richtung umkehren
  upperScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0x00, 0xff, 0xff); // Farbe: Cyan
  
  // Initialisiere die Laufschrift für das untere Panel (Datum)
  lowerScrollingMsg.SetFont(MatriseFontData);
  lowerScrollingMsg.Init(&lowerMatrix, lowerMatrix.Width(), lowerScrollingMsg.FontHeight() + 1, 0, 0);
  lowerScrollingMsg.SetScrollDirection(SCROLL_LEFT);  // Normal von links nach rechts
  lowerScrollingMsg.SetTextDirection(SCROLL_LEFT);    // Normal
  lowerScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0x80, 0x00); // Farbe: Orange
  
  // Starttexte
  strcpy((char*)upperTextBuffer, "      Verbinde mit WLAN...");
  strcpy((char*)lowerTextBuffer, "      Verbinde mit WLAN...");
  upperTextLength = strlen((char*)upperTextBuffer);
  lowerTextLength = strlen((char*)lowerTextBuffer);
  upperScrollingMsg.SetText(upperTextBuffer, upperTextLength);
  lowerScrollingMsg.SetText(lowerTextBuffer, lowerTextLength);
  
  // WLAN verbinden
  WiFi.begin(ssid, password);
  Serial.print("Verbinde mit WLAN");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    // LED-Matrix aktualisieren, während wir auf WLAN warten
    updateScrollText();
    FastLED.show();
  }
  Serial.println("\nWLAN verbunden!");
  
  // Zeit von NTP-Server holen
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  // Text mit Begrüßung setzen
  strcpy((char*)upperTextBuffer, "      System bereit!");
  strcpy((char*)lowerTextBuffer, "      System bereit!");
  upperTextLength = strlen((char*)upperTextBuffer);
  lowerTextLength = strlen((char*)lowerTextBuffer);
  upperScrollingMsg.SetText(upperTextBuffer, upperTextLength);
  lowerScrollingMsg.SetText(lowerTextBuffer, lowerTextLength);
  
  // Berechne die ungefähre Breite der Texte
  upperTextWidth = upperTextLength * 6;
  lowerTextWidth = lowerTextLength * 6;
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Zeit aktualisieren (jede Sekunde)
  if (currentMillis - lastTimeUpdate >= timeUpdateInterval) {
    lastTimeUpdate = currentMillis;
    updateTimeDisplay();
  }
  
  // Text scrollen
  if (currentMillis - lastScrollUpdate >= scrollUpdateInterval) {
    lastScrollUpdate = currentMillis;
    updateScrollText();
  }
  
  // Zeige die aktualisierte Matrix an
  FastLED.show();
  
  // Wenn ein Update des Textes notwendig ist
  if (textNeedsUpdate) {
    updateCompleteDisplay();
    textNeedsUpdate = false;
  }
}

void updateScrollText() {
  // Update oberes Panel (Uhrzeit)
  upperScrollingMsg.UpdateText();
  upperScrollCount++;
  
  // Wenn der Text komplett durchgelaufen ist
  if (upperScrollCount >= (upperTextWidth + MATRIX_WIDTH - 35)) {
    upperScrollCount = 0;
    upperScrollingMsg.SetText(upperTextBuffer, upperTextLength);
  }
  
  // Update unteres Panel (Datum)
  lowerScrollingMsg.UpdateText();
  lowerScrollCount++;
  
  // Wenn der Text komplett durchgelaufen ist
  if (lowerScrollCount >= (lowerTextWidth + MATRIX_WIDTH - 40)) {
    lowerScrollCount = 0;
    lowerScrollingMsg.SetText(lowerTextBuffer, lowerTextLength);
  }
}

void updateTimeDisplay() {
  struct tm timeInfo;
  if (getLocalTime(&timeInfo)) {
    // Markiere, dass wir ein Update des Textes benötigen
    textNeedsUpdate = true;
  }
}

void updateCompleteDisplay() {
  // Aktuelle Zeit holen
  struct tm timeInfo;
  if (!getLocalTime(&timeInfo)) {
    Serial.println("Fehler beim Abrufen der Zeit");
    return;
  }
  
  // Text für oberes Panel (Uhrzeit) formatieren
  char timeText[75];
  sprintf(timeText, "      %02d:%02d:%02d", 
          timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
  
  // Text für unteres Panel (Datum) formatieren
  char dateText[75];
  sprintf(dateText, "      %02d.%02d.%04d",
          timeInfo.tm_mday, timeInfo.tm_mon + 1, timeInfo.tm_year + 1900);
  
  // Texte für LED-Matrizen setzen
  strcpy((char*)upperTextBuffer, timeText);
  strcpy((char*)lowerTextBuffer, dateText);
  upperTextLength = strlen((char*)upperTextBuffer);
  lowerTextLength = strlen((char*)lowerTextBuffer);
  
  // Berechne die ungefähre Breite der Texte
  upperTextWidth = upperTextLength * 6;
  lowerTextWidth = lowerTextLength * 6;
  
  // Texte nur neu setzen, wenn wir am Anfang des Scrolls sind
  if (upperScrollCount < 5) {
    upperScrollingMsg.SetText(upperTextBuffer, upperTextLength);
    upperScrollCount = 0;
  }
  
  if (lowerScrollCount < 5) {
    lowerScrollingMsg.SetText(lowerTextBuffer, lowerTextLength);
    lowerScrollCount = 0;
  }
}