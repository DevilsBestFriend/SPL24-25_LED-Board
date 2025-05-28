#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

/*
 * Klasse zur Verwaltung des DHT22-Sensors.
 */
class TemperaturSensor {
public:
  TemperaturSensor(uint8_t pin, uint8_t typ)
    : dht(pin, typ) {}

  void starten() {
    dht.begin();
  }

  float leseTemperatur() {
    return dht.readTemperature();
  }

private:
  DHT dht;
};

// WLAN-Zugangsdaten
const char* wlanName = "iPhone von David";
const char* wlanPasswort = "Fortnite4life";

// Webhook-URL
const char* webAppUrl =
  "https://script.google.com/macros/s/AKfycbw7HoXQEsoIS8YQ29FizEQhf903ahI2iOmNgdAKw1pcOHSjS4KIFGk_WvB23RArKvmC/exec";

// Sensor-Konfiguration
const uint8_t sensorPin = 33;
const uint8_t sensorTyp = DHT22;
TemperaturSensor temperaturSensor(sensorPin, sensorTyp);

// Task Handles (optional)
TaskHandle_t taskWlanHandle = NULL;
TaskHandle_t taskTemperaturHandle = NULL;

/*
 * Task zur WLAN-Verbindung
 */
void taskWLAN(void* parameter) {
  WiFi.begin(wlanName, wlanPasswort);
  Serial.println("Verbinde mit WLAN...");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }

  Serial.println("\nWLAN verbunden!");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());

  vTaskDelete(NULL);  // Task beenden
}

/*
 * Task zur Temperaturmessung und Datenübertragung
 */
void taskTemperatur(void* parameter) {
  while (true) {
    float temperatur = temperaturSensor.leseTemperatur();

    if (isnan(temperatur)) {
      Serial.println("Fehler beim Lesen der Temperatur!");
    } else {
      Serial.print("Temperatur: ");
      Serial.print(temperatur);
      Serial.println(" °C");

      sendeDaten(temperatur);
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS);  // 2 Sekunden Pause
  }
}

/*
 * Sendet Temperaturdaten an ein Google Sheet über Webhook.
 */
void sendeDaten(float temperatur) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = String(webAppUrl) + "?temperatur=" + temperatur;

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.print("HTTP Antwortcode: ");
      Serial.println(httpCode);
    } else {
      Serial.print("HTTP Fehler: ");
      Serial.println(http.errorToString(httpCode));
    }

    http.end();
  } else {
    Serial.println("Nicht mit WLAN verbunden!");
  }
}

void setup() {
  Serial.begin(9600);
  temperaturSensor.starten();

  // WLAN-Task starten (Core 0)
  xTaskCreatePinnedToCore(
    taskWLAN,               // Funktion
    "WLAN Verbindung",      // Name
    4096,                   // Stackgröße
    NULL,                   // Parameter
    1,                      // Priorität
    &taskWlanHandle,        // Handle
    0                       // Core 0
  );

  // Temperatur-Task starten (Core 1)
  xTaskCreatePinnedToCore(
    taskTemperatur,
    "Temperaturmessung",
    4096,
    NULL,
    1,
    &taskTemperaturHandle,
    1
  );
}

void loop() {
  // Wird nicht verwendet – FreeRTOS übernimmt die Steuerung
}
