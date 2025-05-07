#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h> 
// WLAN-Zugangsdaten
const char* ssid = "iPhone von David";
const char* password = "Fortnite4life";

// OpenWeatherMap API
const char* apiKey = "e2a048c7adfe57f26a2f98a7790eb912";
const char* city = "Innsbruck,AT";
String apiUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "&units=metric&appid=" + String(apiKey);

// LED-Matrix Einstellungen
#define PIN 25  // Daten-Pin für die LED-Matrix
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 8
#define MATRIX_TYPE NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(MATRIX_WIDTH, MATRIX_HEIGHT, PIN, MATRIX_TYPE, NEO_GRB + NEO_KHZ800);

// Wetterdaten
float temperature = 0.0;
String weatherDescription = "";
int humidity = 0;
float windSpeed = 0.0;

// WiFi-Verbindung herstellen
void connectToWiFi() {
  Serial.print("Verbinde mit WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi verbunden.");
}

// Wetterdaten abrufen
void fetchWeatherData(void *pvParameters) {
  Serial.println("Wetter Task gestartet");
  
  while (1) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(apiUrl);
      int httpCode = http.GET();

      if (httpCode > 0) {
        String payload = http.getString();
        Serial.println("Antwort erhalten:");
        Serial.println(payload);

        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        
        if (!error) {
          temperature = doc["main"]["temp"];
          weatherDescription = doc["weather"][0]["description"].as<String>();
          humidity = doc["main"]["humidity"];
          windSpeed = doc["wind"]["speed"];

          Serial.println("Wetterdaten aktualisiert!");
        } else {
          Serial.print("JSON-Parsing Fehler: ");
          Serial.println(error.c_str());
        }
      } else {
        Serial.print("HTTP-Fehler: ");
        Serial.println(httpCode);
      }
      http.end();
    } else {
      Serial.println("WiFi nicht verbunden");
    }

    vTaskDelay(60000 / portTICK_PERIOD_MS); // Alle 60 Sekunden aktualisieren
  }
}

// LED-Matrix aktualisieren
void updateDisplay(void *pvParameters) {
  Serial.println("Display Task gestartet");
  
  while (1) {
    matrix.fillScreen(0);
    matrix.setCursor(0, 0);
    matrix.setTextColor(matrix.Color(0, 255, 0));
    matrix.setTextSize(1);

    matrix.print("Temp: ");
    matrix.print(temperature);
    matrix.print("C");
    matrix.show();
    delay(3000);
    
    matrix.fillScreen(0);
    matrix.setCursor(0, 0);
    matrix.setTextColor(matrix.Color(255, 255, 0));
    matrix.print("Luft: ");
    matrix.print(humidity);
    matrix.print("%");
    matrix.show();
    delay(3000);

    matrix.fillScreen(0);
    matrix.setCursor(0, 0);
    matrix.setTextColor(matrix.Color(0, 0, 255));
    matrix.print("Wind: ");
    matrix.print(windSpeed);
    matrix.print("m/s");
    matrix.show();
    delay(3000);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(10);  // Helligkeit anpassen
  
  connectToWiFi();

  // FreeRTOS Tasks erstellen
  xTaskCreate(fetchWeatherData, "WetterTask", 4096, NULL, 1, NULL);
  xTaskCreate(updateDisplay, "DisplayTask", 2048, NULL, 1, NULL);
}

void loop() {
  vTaskDelay(portMAX_DELAY); // Hauptloop wird nicht genutzt
}