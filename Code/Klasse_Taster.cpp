#include <Arduino.h>

class TasterEntpreller {
private:
    const int pin;
    bool istGedrueckt = false;
    unsigned long letzteZeit;
    unsigned long gedruecktZeitpunkt = 0; // Zeitpunkt, wann der Taster gedrückt wurde
    const unsigned long entprellZeit = 50; // Entprellzeit in Millisekunden
    const unsigned long langDruckDauer = 1000; // Dauer für langen Tastendruck (1 Sekunde)

public:
    TasterEntpreller(int pin) : pin(pin), letzteZeit(0), gedruecktZeitpunkt(0) {
        pinMode(pin, INPUT_PULLUP);
    }

    void aktualisiere() {
        // Tasterzustand lesen
        bool tasterStatus = !digitalRead(pin); // invertiert, da Pullup-Widerstand verwendet wird

        // Wenn sich der Tasterzustand ändert, Zeit speichern
        if (tasterStatus != istGedrueckt) {
            letzteZeit = millis();
        }

        // Wenn die Entprellzeit abgelaufen ist und der Zustand sich stabilisiert hat
        if (millis() - letzteZeit > entprellZeit) {
            if (tasterStatus && !istGedrueckt) {
                // Taster wurde gerade gedrückt, Zeitpunkt speichern
                gedruecktZeitpunkt = millis();
            }
            istGedrueckt = tasterStatus;
        }
    }

    bool istGedruecktJetzt() {
        return istGedrueckt;
    }

    bool wurdeGedrueckt() {
        // Prüfen, ob der Taster gerade losgelassen wurde (Flankenerkennung)
        if (!istGedrueckt && !digitalRead(pin)) { 
            return true;
        }
        return false;
    }

    bool wurdeLangGedrueckt() {
        // Prüfen, ob der Taster länger als 1 Sekunde gedrückt wurde
        if (istGedrueckt && (millis() - gedruecktZeitpunkt >= langDruckDauer)) {
            return true;
        }
        return false;
    }
};

// Pin für den Taster
const uint8_t tasterPin = 32; // Beispiel-Pin, an dem der Taster angeschlossen ist

// TasterEntpreller-Objekt erstellen
TasterEntpreller taster(tasterPin);

void setup() {
    Serial.begin(9600);
}

void loop() {
    // Aktualisiere den Tasterentpreller
    taster.aktualisiere();

    // Gib den aktuellen entprellten Tasterstatus aus
    if (taster.istGedruecktJetzt()) {
        Serial.println("Taster ist gedrückt");
    }

    // Überprüfe, ob der Taster seit dem letzten Mal kurz gedrückt wurde
    if (taster.wurdeGedrueckt()) {
        Serial.println("Taster wurde kurz gedrückt");
    }

    // Überprüfe, ob der Taster lange gedrückt wurde
    if (taster.wurdeLangGedrueckt()) {
        Serial.println("Taster wurde lange gedrückt");
    }

    delay(100); // Kleine Verzögerung für die Schleife
}