#ifndef JOYSTICK_LIB_H
#define JOYSTICK_LIB_H

#include <Arduino.h>

class EntprellterTaster {
  private:
    int pin;                          // Pin des Tasters
    int xPin;                         // Pin für X-Koordinate (optional)
    int yPin;                         // Pin für Y-Koordinate (optional)
    bool entprellterZustand;          // Entprellter Zustand des Tasters
    unsigned long letzteAenderung;   // Zeit der letzten Statusänderung
    unsigned long druckBeginn;       // Zeitpunkt, wann der Taster gedrückt wurde
    bool gedruecktGemeldet;          // Ob der Druck schon gemeldet wurde
    static const unsigned long entprellZeit = 50;    // Entprellzeit in ms
    static const unsigned long langeDruckZeit = 1000; // Lange Druckzeit in ms

  public:
    EntprellterTaster(int tasterPin, int xPin = -1, int yPin = -1); // Konstruktor mit optionalen Pins
    void aktualisiere();                // Aktualisiert den Tasterstatus
    bool istGedrueckt();                // Gibt zurück, ob der Taster gedrückt ist
    bool wurdeGedrueckt();              // Gibt zurück, ob der Taster kurz gedrückt wurde
    bool wurdeLangeGedrueckt();         // Gibt zurück, ob der Taster lange gedrückt wurde
    int leseX();                        // Liest die X-Koordinate des Joysticks
    int leseY();                        // Liest die Y-Koordinate des Joysticks
};

#endif
