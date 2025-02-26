#include <Arduino.h>

class TasterEntpreller {
private:
    const int pin;
    bool istGedrueckt = false;           // Aktueller Zustand des Tasterss
    bool langerDruckErkannt = false;     // Flag für langen Druck
    unsigned long gedruecktZeitpunkt = 0; // Zeitpunkt des Drückens
    const unsigned long entprellZeit = 50; // Entprellzeit in Millisekunden
    const unsigned long langDruckSchwelle = 1000; // Schwelle für langen Druck (1 Sekunde)
    unsigned long letzteZeit = 0;        // Letzte Zeit für Entprellung

public:
    TasterEntpreller(int pin) : pin(pin) {
        pinMode(pin, INPUT_PULLUP);      // Pullup-Widerstand aktivieren
    }

    void aktualisiere() {
        bool tasterStatus = !digitalRead(pin); // Invertiert wegen Pullup

        // Entprellung: Nur weiter machen, wenn sich der Zustand stabilisiert hat
        if (tasterStatus != istGedrueckt && millis() - letzteZeit > entprellZeit) {
            letzteZeit = millis();       // Zeit aktualisieren

            if (tasterStatus) {          // Wenn der Taster gerade gedrückt wurde
                gedruecktZeitpunkt = millis();  // Zeitpunkt speichern
                langerDruckErkannt = false;     // Reset des langen Drucks
            } else {                     // Wenn der Taster losgelassen wurde
                if (!langerDruckErkannt && millis() - gedruecktZeitpunkt < langDruckSchwelle) {
                    Serial.println("Taster wurde kurz gedrückt");
                }
            }

            istGedrueckt = tasterStatus;
        }

        // Langer Druck: Wenn der Taster länger als die Schwelle gedrückt wird
        if (istGedrueckt && !langerDruckErkannt && millis() - gedruecktZeitpunkt >= langDruckSchwelle) {
            Serial.println("Taster wurde lange gedrückt");
            langerDruckErkannt = true;   // Langer Druck erkannt, um Mehrfachausgabe zu verhindern
        }
    }

    bool istGedruecktJetzt() {
        return istGedrueckt;
    }
};

// Pin für den Taster
const uint8_t tasterPin = 32;

// TasterEntpreller-Objekt erstellen
TasterEntpreller taster(tasterPin);

void setup() {
    Serial.begin(9600);
}

void loop() {
    taster.aktualisiere();  // Aktualisiere den Tasterstatus in jeder Schleife

    // Gib IMMER aus, wenn der Taster gedrückt ist (egal ob kurz oder lang)
    if (taster.istGedruecktJetzt()) {
        Serial.println("Taster ist gerade gedrückt!");
    }

    delay(100);  // Kleine Verzögerung zur Reduzierung der Ausgabefrequenz
}