#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// WLAN-Zugangsdaten
const char* ssid = "iPhone von Hendrik";
const char* password = "hst123456";

// HiveMQ Cloud Einstellungen
const char* mqtt_server = "f92b6cc16e1243c4bee490c764e600de.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "hendrik999";
const char* mqtt_pass = "Hst123456";

const char* sub_topic = "#";  // Alle Topics abonnieren
const char* client_id = "esp32-raw-logger";

WiFiClientSecure secureClient;
PubSubClient client(secureClient);

void connectToWiFi() {
  Serial.print("Verbinde mit WLAN...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWLAN verbunden | IP: " + WiFi.localIP().toString());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Nur die reine Nachricht ausgeben
  String message;
  for(unsigned int i=0; i<length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("MQTT-Verbindung...");
    if(client.connect(client_id, mqtt_user, mqtt_pass)) {
      client.subscribe(sub_topic);
      Serial.println("erfolgreich!");
    } else {
      Serial.print("Fehler: ");
      Serial.print(client.state());
      Serial.println(" - Neuer Versuch in 5s...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  
  secureClient.setInsecure(); // Nur für Tests
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if(!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
}
