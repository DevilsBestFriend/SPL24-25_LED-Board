// Klasse_Taster.h
#ifndef KLASSE_TASTER_H
#define KLASSE_TASTER_H

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
    TasterEntpreller(int pin);
    void aktualisiere();
    bool istGedruecktJetzt();
    bool wurdeGedrueckt();
    bool wurdeLangGedrueckt();
};

#endif