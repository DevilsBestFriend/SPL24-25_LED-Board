#include <FastLED.h>
#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>
#include <WiFi.h>
#include "time.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

// WLAN-Zugangsdaten
const char* ssid = "iPhone von Hendrik";
const char* password = "hst123456";

// NTP-Server
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600; // GMT+1
const int daylightOffset_sec = 3600; // Sommerzeit

// Hardware-Konfiguration
#define LED_PIN_UPPER   25
#define LED_PIN_LOWER   26
#define COLOR_ORDER     GRB
#define CHIPSET         WS2812B
#define MATRIX_WIDTH    32
#define MATRIX_HEIGHT   8
#define MATRIX_TYPE     VERTICAL_ZIGZAG_MATRIX

// RTOS Einstellungen
#define TASK_STACK_SIZE 4096
#define PRIO_HIGH 3
#define PRIO_NORMAL 2

// Globale Objekte
cLEDMatrix<MATRIX_WIDTH, -MATRIX_HEIGHT, MATRIX_TYPE> upperMatrix;
cLEDMatrix<-MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> lowerMatrix;
cLEDText upperScrollingMsg;
cLEDText lowerScrollingMsg;

// Shared Resources
SemaphoreHandle_t xTextMutex;
SemaphoreHandle_t xLEDMutex;
unsigned char upperTextBuffer[75];
unsigned char lowerTextBuffer[75];
int upperTextLength = 0;
int lowerTextLength = 0;

// Für Scroll-Logik
volatile bool upperNeedsUpdate = false;
volatile bool lowerNeedsUpdate = false;
char nextUpperText[75] = "";
char nextLowerText[75] = "";

// Task Handles
TaskHandle_t xTimeTaskHandle;
TaskHandle_t xScrollTaskHandle;

// --- Text aktualisieren (mit Mutex) ---
void updateText(const char* upper, const char* lower) {
  if(xSemaphoreTake(xTextMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
    strlcpy((char*)upperTextBuffer, upper, sizeof(upperTextBuffer));
    strlcpy((char*)lowerTextBuffer, lower, sizeof(lowerTextBuffer));
    upperTextLength = strlen((char*)upperTextBuffer);
    lowerTextLength = strlen((char*)lowerTextBuffer);

    if(xSemaphoreTake(xLEDMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      upperScrollingMsg.SetText(upperTextBuffer, upperTextLength);
      lowerScrollingMsg.SetText(lowerTextBuffer, lowerTextLength);
      xSemaphoreGive(xLEDMutex);
    }
    xSemaphoreGive(xTextMutex);
  }
}

// --- Zeit-Task ---
void timeTask(void *pvParameters) {
  while(1) {
    struct tm timeInfo;
    if(getLocalTime(&timeInfo, 100)) {
      char timeStr[32], dateStr[32];
      strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeInfo);
      strftime(dateStr, sizeof(dateStr), "%d.%m.%Y", &timeInfo);

      // Nächste Texte vorbereiten
      snprintf(nextUpperText, sizeof(nextUpperText), "      %s", timeStr);
      snprintf(nextLowerText, sizeof(nextLowerText), "      %s", dateStr);

      // Markiere, dass nach dem nächsten Scrollzyklus aktualisiert werden soll
      upperNeedsUpdate = true;
      lowerNeedsUpdate = true;

      // Debug-Ausgabe
      Serial.print("Neue Uhrzeit: "); Serial.println(timeStr);
      Serial.print("Neues Datum: "); Serial.println(dateStr);
    }
    vTaskDelay(pdMS_TO_TICKS(1000)); // Jede Sekunde neue Zeit holen
  }
}

// --- Scroll-Task ---
void scrollTask(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while(1) {
    if(xSemaphoreTake(xLEDMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
      // Update Laufschrift
      bool upperDone = upperScrollingMsg.UpdateText();
      bool lowerDone = lowerScrollingMsg.UpdateText();
      FastLED.show();
      xSemaphoreGive(xLEDMutex);

      // Wenn Laufschrift zu Ende UND ein Update steht an, dann Text wechseln
      if (upperDone && upperNeedsUpdate) {
        updateText(nextUpperText, (const char*)lowerTextBuffer); // Nur obere Zeile aktualisieren
        upperNeedsUpdate = false;
      }
      if (lowerDone && lowerNeedsUpdate) {
        updateText((const char*)upperTextBuffer, nextLowerText); // Nur untere Zeile aktualisieren
        lowerNeedsUpdate = false;
      }
    }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(30)); // ~33 FPS
  }
}

void setup() {
  Serial.begin(115200);

  // LED Matrix Initialisierung
  FastLED.addLeds<CHIPSET, LED_PIN_UPPER, COLOR_ORDER>(upperMatrix[0], upperMatrix.Size());
  FastLED.addLeds<CHIPSET, LED_PIN_LOWER, COLOR_ORDER>(lowerMatrix[0], lowerMatrix.Size());
  FastLED.setBrightness(10);
  FastLED.clear(true);

  // Text Scroller Konfiguration
  upperScrollingMsg.SetFont(MatriseFontData);
  upperScrollingMsg.Init(&upperMatrix, upperMatrix.Width(), upperScrollingMsg.FontHeight() + 1, 1, 0);
  upperScrollingMsg.SetScrollDirection(SCROLL_LEFT);
  upperScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0x00, 0xff, 0xff); // Cyan

  lowerScrollingMsg.SetFont(MatriseFontData);
  lowerScrollingMsg.Init(&lowerMatrix, lowerMatrix.Width(), lowerScrollingMsg.FontHeight() + 1, 0, 0);
  lowerScrollingMsg.SetScrollDirection(SCROLL_LEFT);
  lowerScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0x80, 0x00); // Orange

  // Mutexe erstellen
  xTextMutex = xSemaphoreCreateMutex();
  xLEDMutex = xSemaphoreCreateMutex();

  // Starttext setzen
  updateText("      Verbinde mit WLAN...", "      Verbinde mit WLAN...");

  // WLAN verbinden mit Timeout
  Serial.print("Verbinde mit WLAN");
  WiFi.begin(ssid, password);
  unsigned long wifiTimeout = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - wifiTimeout < 20000)) {
    delay(250);
    Serial.print(".");
  }
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi-Timeout! Starte neu...");
    updateText("      WLAN-Fehler!", "      Kein Netz!");
    delay(3000);
    ESP.restart();
  }
  Serial.println("\nWLAN verbunden!");

  // NTP konfigurieren
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Begrüßungstext
  updateText("      System bereit!", "      System bereit!");

  // RTOS Tasks erstellen
  xTaskCreatePinnedToCore(
    timeTask, "TimeTask", TASK_STACK_SIZE, NULL, PRIO_HIGH, &xTimeTaskHandle, 1
  );
  xTaskCreatePinnedToCore(
    scrollTask, "ScrollTask", TASK_STACK_SIZE, NULL, PRIO_NORMAL, &xScrollTaskHandle, 0
  );
}

void loop() {
  vTaskDelete(NULL); // FreeRTOS übernimmt alles!
}
