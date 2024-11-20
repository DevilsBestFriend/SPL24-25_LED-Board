#include "esp32-hal-gpio.h"
// Klasse_Taster.cpp
#include "Klasse_Taster.h"

TasterEntpreller::TasterEntpreller(int pin) : pin(pin), letzteZeit(0), gedruecktZeitpunkt(0) {
    pinMode(pin, INPUT_PULLUP);
}

void TasterEntpreller::aktualisiere() {
    // Tasterzustand lesen
    bool tasterStatus = !digitalRead(pin); // invertiert, da Pullup-Widerstand verwendet wird

    // Wenn sich der Tasterzustand ändert, Zeit speichern
    if (tasterStatus != istGedrueckt) {
        letzteZeit = millis();
    }

    // Wenn die Entprellzeit abgelaufen ist und der Zustand sich stabilisiert hat
    if (millis() - letzteZeit > entprellZeit) {
        if (tasterStatus && !istGedrueckt) {
            // Taster wurde gerade gedrückt, Zeitpunkt speichernn
            gedruecktZeitpunkt = millis();
        }
        istGedrueckt = tasterStatus;
    }
}

bool TasterEntpreller::istGedruecktJetzt() {
    return istGedrueckt;
}

bool TasterEntpreller::wurdeGedrueckt() {
    // Prüfen, ob der Taster gerade losgelassen wurde (Flankenerkennung)
    if (!istGedrueckt && !digitalRead(pin)) { 
        return true;
    }
    return false;
}

bool TasterEntpreller::wurdeLangGedrueckt() {
    // Prüfen, ob der Taster länger als 1 Sekunde gedrückt wurde
    if (istGedrueckt && (millis() - gedruecktZeitpunkt >= langDruckDauer)) {
        return true;
    }
    return false;
}