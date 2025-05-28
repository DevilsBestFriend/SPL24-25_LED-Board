#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <FastLED.h>
#include <LEDMatrix.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// WLAN-Zugangsdaten
const char* ssid = "iPhone von David";
const char* password = "Fortnite4life";

// OpenWeatherMap API
const char* apiKey = "e2a048c7adfe57f26a2f98a7790eb912";
const char* city = "Innsbruck,AT";
String apiUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "&units=metric&appid=" + String(apiKey);

// Matrix-Konfiguration
#define DATA_PIN_UPPER 25
#define DATA_PIN_LOWER 26
#define NUM_LEDS_PER_STRIP 256
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 16
#define MATRIX_TYPE VERTICAL_ZIGZAG_MATRIX

CRGB leds_upper[NUM_LEDS_PER_STRIP];
CRGB leds_lower[NUM_LEDS_PER_STRIP];
CRGB leds_array[NUM_LEDS_PER_STRIP * 2];
cLEDMatrix<-MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;

// LED Mapping-Struktur
struct LedAddress {
  CRGB* array;
  int index;
};

// Mapping wie im Snake-Spiel
LedAddress mapXY(int x, int y) {
  LedAddress result;
  int led;

  if (y < 8) {
    led = x * 8;
    if ((x % 2) == 0) led += y;
    else led += 7 - y;
    result.array = leds_lower;
    result.index = led;
  } else {
    int flippedX = MATRIX_WIDTH - 1 - x;
    int flippedY = 15 - y;
    led = flippedX * 8;
    if ((flippedX % 2) == 0) led += flippedY;
    else led += 7 - flippedY;
    result.array = leds_upper;
    result.index = led;
  }
  return result;
}

// Font 3x5 (optimiert für Punkt & Einheiten)
const byte font3x5[][5] = {
  {0b111, 0b101, 0b101, 0b101, 0b111},  // 0
  {0b010, 0b110, 0b010, 0b010, 0b111},  // 1
  {0b111, 0b001, 0b111, 0b100, 0b111},  // 2
  {0b111, 0b001, 0b111, 0b001, 0b111},  // 3
  {0b101, 0b101, 0b111, 0b001, 0b001},  // 4
  {0b111, 0b100, 0b111, 0b001, 0b111},  // 5
  {0b111, 0b100, 0b111, 0b101, 0b111},  // 6
  {0b111, 0b001, 0b010, 0b100, 0b100},  // 7
  {0b111, 0b101, 0b111, 0b101, 0b111},  // 8
  {0b111, 0b101, 0b111, 0b001, 0b111},  // 9
  {0b000, 0b000, 0b000, 0b010, 0b000},  // . (10)
  {0b011, 0b100, 0b100, 0b100, 0b011},  // C (11)
  {0b101, 0b001, 0b010, 0b100, 0b101},  // % (12)
  {0b000, 0b110, 0b101, 0b101, 0b101},  // m (13)
  {0b001, 0b010, 0b010, 0b100, 0b000},  // / (14)
  {0b111, 0b100, 0b111, 0b001, 0b111}   // s (15)
};

// Werte
float temperature = 0.0;
int humidity = 0;
float windSpeed = 0.0;

// Zeichen anzeigen
void drawChar(int index, int x, int y, CRGB color) {
  if (index < 0 || index >= 16) return;
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 3; col++) {
      if ((font3x5[index][row] >> (2 - col)) & 0x01) {
        LedAddress addr = mapXY(x + col, y + (4 - row));  // Y-Korrektur!
        addr.array[addr.index] = color;
      }
    }
  }
}


// Text anzeigen
void drawText(String text, int x, int y, CRGB color) {
  int pos = 0;
  for (unsigned int i = 0; i < text.length(); i++) {
    char c = text.charAt(i);
    int index = -1;

    if (c >= '0' && c <= '9') index = c - '0';
    else if (c == '.') index = 10;
    else if (c == 'C') index = 11;
    else if (c == '%') index = 12;
    else if (c == 'm') index = 13;
    else if (c == '/') index = 14;
    else if (c == 's') index = 15;

    if (index != -1) {
      drawChar(index, x + pos, y, color);
      pos += 4;
    }
  }
}

// Wetterdaten abrufen
void fetchWeatherData(void *pvParameters) {
  while (true) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(apiUrl);
      int httpCode = http.GET();

      if (httpCode > 0) {
        String payload = http.getString();
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
          temperature = doc["main"]["temp"];
          humidity = doc["main"]["humidity"];
          windSpeed = doc["wind"]["speed"];
          Serial.printf("Temp: %.1f°C | Humidity: %d%% | Wind: %.1f m/s\n",
                        temperature, humidity, windSpeed);
        } else {
          Serial.println("JSON Fehler");
        }
      } else {
        Serial.println("HTTP Fehler");
      }
      http.end();
    }
    vTaskDelay(60000 / portTICK_PERIOD_MS);
  }
}

// Anzeige wechseln
void updateDisplay(void *pvParameters) {
  while (true) {
    char buffer[12];

    FastLED.clear();
    snprintf(buffer, sizeof(buffer), "%.1fC", temperature);
    drawText(String(buffer), 2, 6, CRGB::Green);
    FastLED.show();
    delay(3000);

    FastLED.clear();
    snprintf(buffer, sizeof(buffer), "%d%%", humidity);
    drawText(String(buffer), 4, 6, CRGB::Yellow);
    FastLED.show();
    delay(3000);

    FastLED.clear();
    snprintf(buffer, sizeof(buffer), "%.1fm/s", windSpeed);
    drawText(String(buffer), 0, 6, CRGB::Blue);
    FastLED.show();
    delay(3000);
  }
}

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<WS2812B, DATA_PIN_UPPER, GRB>(leds_upper, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, DATA_PIN_LOWER, GRB>(leds_lower, NUM_LEDS_PER_STRIP);
  FastLED.setBrightness(50);

  for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
    leds_array[i] = leds_lower[i];
    leds_array[i + NUM_LEDS_PER_STRIP] = leds_upper[i];
  }
  leds.SetLEDArray(leds_array);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWLAN verbunden.");

  xTaskCreatePinnedToCore(fetchWeatherData, "WeatherTask", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(updateDisplay, "DisplayTask", 4096, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}
