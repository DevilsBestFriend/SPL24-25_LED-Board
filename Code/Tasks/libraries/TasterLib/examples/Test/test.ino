#include <joystickLib.h>

// Definition der Pins für den Taster und die Joystick-Achsen
const int TASTER_PIN = 32; // Pin des Tasters
const int X_PIN = 34;      // Pin für die X-Achse
const int Y_PIN = 35;      // Pin für die Y-Achse

EntprellterTaster joystick(TASTER_PIN, X_PIN, Y_PIN);

unsigned long letzteAusgabeZeit = 0; // Letzte Zeit, zu der Werte ausgegeben wurden
const unsigned long AUSGABE_INTERVAL = 100; // Intervall für die serielle Ausgabe in Millisekunden

void setup() {
  Serial.begin(9600); // Serielle Kommunikation starten
  Serial.println("Joystick-Test gestartet");
}

void loop() {
  // Aktualisiere den Zustand des Tasters
  joystick.aktualisiere();

  // Prüfe, ob der Taster kurz oder lange gedrückt wurde
  if (joystick.wurdeGedrueckt()) {
    Serial.println("Taster wurde kurz gedrückt");
  }

  if (joystick.wurdeLangeGedrueckt()) {
    Serial.println("Taster wurde lange gedrückt");
  }

  // Werte nur in definierten Intervallen ausgeben
  unsigned long aktuelleZeit = millis();
  if (aktuelleZeit - letzteAusgabeZeit >= AUSGABE_INTERVAL) {
    letzteAusgabeZeit = aktuelleZeit;

    // X- und Y-Werte auslesen und ausgeben
    JoystickWerte werte = joystick.leseXY();
    Serial.print("X: ");
    Serial.print(werte.x);
    Serial.print(" | Y: ");
    Serial.println(werte.y);
  }
}
