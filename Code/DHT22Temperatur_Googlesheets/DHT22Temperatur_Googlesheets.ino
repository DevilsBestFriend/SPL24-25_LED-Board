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
const char* wlanName = "zTLatte";
const char* wlanPasswort = "yfbk69420";

// Webhook-URL (z. B. von IFTTT oder Google Script Web App)
const char* webAppUrl =
  "https://script.google.com/macros/s/AKfycbw7HoXQEsoIS8YQ29FizEQhf903ahI2iOmNgdAKw1pcOHSjS4KIFGk_WvB23RArKvmC/exec";

const uint8_t sensorPin = 33;
const uint8_t sensorTyp = DHT22;
TemperaturSensor temperaturSensor(sensorPin, sensorTyp);

unsigned long letzterZeitstempel = 0;
const unsigned long intervall = 2000;  // 2 Sekunden

void setup() {
  Serial.begin(115200);

  temperaturSensor.starten();

  WiFi.begin(wlanName, wlanPasswort);

  Serial.print("Verbinde mit WLAN...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWLAN verbunden!");
  Serial.println("IP-Adresse: " + WiFi.localIP().toString());
}

void loop() {
  unsigned long aktuelleZeit = millis();

  if (aktuelleZeit - letzterZeitstempel >= intervall) {
    letzterZeitstempel = aktuelleZeit;

    float temperatur = temperaturSensor.leseTemperatur();

    // Fehlerbehandlung
    if (isnan(temperatur)) {
      Serial.println("Fehler beim Lesen der Temperatur!");
      return;
    }

    Serial.print("Temperatur: ");
    Serial.print(temperatur);
    Serial.println(" °C");

    sendeDaten(temperatur);
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