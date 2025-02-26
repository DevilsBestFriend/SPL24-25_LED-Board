#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"

// WLAN-Zugangsdaten
const char* ssid = "FITZ!Box 7590 HN";
const char* password = "fitzinger";

// OpenWeatherMap-API-Details
const char* weatherApiKey = "965631ef20117397bfee0d02ee665406";
const char* city = "Innsbruck";
const char* weatherApiUrl = "http://api.openweathermap.org/data/2.5/weather?q=Innsbruck&appid=";

// NTP-Server
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600; // GMT+1
const int daylightOffset_sec = 3600; // Sommerzeit

void setup() {
  Serial.begin(115200);
  
  // WLAN verbinden
  WiFi.begin(ssid, password);
  Serial.print("Verbinde mit WLAN");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWLAN verbunden!");

  // Zeit von NTP-Server holen
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  // Zeit und Datum abrufen
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
Serial.println("NTP-Konfiguration abgeschlossen, Zeit wird abgerufen...");
struct tm timeInfo;
if (getLocalTime(&timeInfo)) {
  Serial.println("Zeit erfolgreich abgerufen:");
  Serial.printf("%02d:%02d:%02d %02d-%02d-%04d\n",
                timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec,
                timeInfo.tm_mday, timeInfo.tm_mon + 1, timeInfo.tm_year + 1900);
} else {
  Serial.println("Fehler beim Abrufen der Zeit.");
}


  // Wetterdaten abrufen
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(weatherApiUrl) + weatherApiKey;
    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode == 200) {
      String payload = http.getString();
      Serial.println("Wetterdaten empfangen:");
      parseAndPrintWeather(payload);
    } else {
      Serial.printf("Fehler beim Abrufen der Wetterdaten: HTTP %d\n", httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Nicht mit dem WLAN verbunden.");
  }

  delay(60000); // Alle 60 Sekunden wiederholen
}

void parseAndPrintWeather(const String& json) {
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.println("Fehler beim Parsen der JSON-Daten.");
    return;
  }

  // Wetterbeschreibung auslesen
  const char* weatherDescription = doc["weather"][0]["description"];

  // Temperatur als float auslesen
  float temperature = doc["main"]["temp"].as<float>() - 273.15; // Kelvin in Celsius umrechnen

  // Ausgabe
  Serial.printf("Wetter: %s\n", weatherDescription);
  Serial.printf("Temperatur: %.2f °C\n", temperature);
}

