#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WLAN-Zugangsdaten
const char* ssid = "iPhone von David";
const char* password = "Fortnite4life";

// OpenWeatherMap API
const char* apiKey = "e2a048c7adfe57f26a2f98a7790eb912";
const char* city = "Innsbruck,AT";
String apiUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "&units=metric&appid=" + String(apiKey);

// OLED-Display Einstellungen
#define SCREEN_WIDTH 32
#define SCREEN_HEIGHT 8
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

// OLED-Display aktualisieren
void updateDisplay(void *pvParameters) {
  Serial.println("Display Task gestartet");

  while (1) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.println("Wetter in Innsbruck");
    display.print("Temp: ");
    display.print(temperature);
    display.println(" C");
    
    display.print("Wetter: ");
    display.println(weatherDescription);
    
    display.print("Luft: ");
    display.print(humidity);
    display.println("%");

    display.print("Wind: ");
    display.print(windSpeed);
    display.println(" m/s");

    display.display();
    
    vTaskDelay(5000 / portTICK_PERIOD_MS); // Alle 5 Sekunden aktualisieren
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // OLED-Display initialisieren
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306-Display nicht gefunden!");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Starte...");
  display.display();
  delay(2000);
  
  connectToWiFi();

  // FreeRTOS Tasks erstellen
  xTaskCreate(fetchWeatherData, "WetterTask", 4096, NULL, 1, NULL);
  xTaskCreate(updateDisplay, "DisplayTask", 2048, NULL, 1, NULL);
}

void loop() {
  vTaskDelay(portMAX_DELAY); // Hauptloop wird nicht genutzt
}
