#include <FastLED.h>
#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

// Hardware-Konfiguration
#define LED_PIN_UPPER   25
#define LED_PIN_LOWER   26
#define COLOR_ORDER     GRB
#define CHIPSET         WS2812B
#define MATRIX_WIDTH    32
#define MATRIX_HEIGHT   8
#define MATRIX_TYPE     VERTICAL_ZIGZAG_MATRIX
#define TEMP_PIN        33

// RTOS-Einstellungen
#define PRIORITY_HIGH   2
#define PRIORITY_NORMAL 1
#define STACK_SIZE      4096

// Globale Objekte
cLEDMatrix<MATRIX_WIDTH, -MATRIX_HEIGHT, MATRIX_TYPE> leds_upper;
cLEDMatrix<-MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds_lower;
cLEDText ScrollingUpper;
cLEDText ScrollingLower;

// Shared Resources
SemaphoreHandle_t xLEDMutex;
float currentTemp = 0.0;
float currentHumi = 0.0;

void setup() {
  Serial.begin(115200);
  
  // LED-Matrix Initialisierung
  FastLED.addLeds<CHIPSET, LED_PIN_UPPER, COLOR_ORDER>(leds_upper[0], leds_upper.Size());
  FastLED.addLeds<CHIPSET, LED_PIN_LOWER, COLOR_ORDER>(leds_lower[0], leds_lower.Size());
  FastLED.setBrightness(30);
  FastLED.clear(true);
  FastLED.show();

  // Text-Scroller Konfiguration
  ScrollingUpper.SetFont(MatriseFontData);
  ScrollingUpper.Init(&leds_upper, leds_upper.Width(), ScrollingUpper.FontHeight() + 1, 0, 0);
  ScrollingUpper.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 255, 0, 255);

  ScrollingLower.SetFont(MatriseFontData);
  ScrollingLower.Init(&leds_lower, leds_lower.Width(), ScrollingLower.FontHeight() + 1, 0, 0);
  ScrollingLower.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0, 255, 0);

  // Mutex für LED-Zugriff
  xLEDMutex = xSemaphoreCreateMutex();

  // RTOS-Tasks erstellen
  xTaskCreatePinnedToCore(
    tempUpdateTask,    // Task-Funktion
    "TempUpdate",      // Task-Name
    STACK_SIZE,        // Stack-Größe
    NULL,              // Parameter
    PRIORITY_HIGH,     // Priorität
    NULL,              // Task-Handle
    1                  // Core 1
  );

  xTaskCreatePinnedToCore(
    upperPanelTask,    // Task-Funktion
    "UpperPanel",      // Task-Name
    STACK_SIZE,        // Stack-Größe
    NULL,              // Parameter
    PRIORITY_NORMAL,   // Priorität
    NULL,              // Task-Handle
    0                  // Core 0
  );

  xTaskCreatePinnedToCore(
    lowerPanelTask,    // Task-Funktion
    "LowerPanel",      // Task-Name
    STACK_SIZE,        // Stack-Größe
    NULL,              // Parameter
    PRIORITY_NORMAL,   // Priorität
    NULL,              // Task-Handle
    0                  // Core 0
  );
}

float readTemp() {
  int raw = analogRead(TEMP_PIN);
  float voltage = raw * (3.3 / 4095.0);
  return (voltage - 0.5) * 10.0;
}

void tempUpdateTask(void *pvParameters) {
  while(1) {
    currentTemp = readTemp();
    currentHumi = 45.0 + (millis() % 2000) / 100.0;
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void updatePanel(cLEDText &panel, const String &text) {
  if(xSemaphoreTake(xLEDMutex, portMAX_DELAY) == pdTRUE) {
    panel.SetText((unsigned char*)text.c_str(), text.length());
    while(panel.UpdateText() == 0) {
      FastLED.show();
      vTaskDelay(pdMS_TO_TICKS(40));
    }
    xSemaphoreGive(xLEDMutex);
  }
}

void upperPanelTask(void *pvParameters) {
  while(1) {
    String text = String((char)0x11) + "Luft: " + String(currentHumi,1) + "%";
    updatePanel(ScrollingUpper, text);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void lowerPanelTask(void *pvParameters) {
  while(1) {
    String text = String((char)0x11) + "Temp: " + String(currentTemp,1) + "C";
    updatePanel(ScrollingLower, text);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void loop() {
  // Leer lassen, da alles in RTOS-Tasks läuft
}
