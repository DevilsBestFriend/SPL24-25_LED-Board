#include <Arduino.h>

class TasterEntpreller {
private:
    const int pin;
    bool istGedrueckt;
    unsigned long letzteZeit;
    const unsigned long entprellZeit = 50; // Entprellzeit in Millisekunden

public:
    TasterEntpreller(int pin) : pin(pin), istGedrueckt(false), letzteZeit(0) {
        pinMode(pin, INPUT_PULLUP);
    }

    void aktualisiere() {
        // Tasterzustand lesen
        bool tasterStatus = !digitalRead(pin); // invertiert, da Pullup-Widerstand verwendet wird

        // Entprellung überprüfen
        if (tasterStatus != istGedrueckt) {
            letzteZeit = millis();
        }

        if (millis() - letzteZeit > entprellZeit) {
            istGedrueckt = tasterStatus;
        }
    }

    bool istGedruecktJetzt() {
        return istGedrueckt;
    }

    bool wurdeGedrueckt() {
        bool result = istGedrueckt && !digitalRead(pin); // erneut invertiert lesen
        istGedrueckt = digitalRead(pin); // Zustand aktualisieren
        return result;
    }
};
