#ifndef JOYSTICKLIB_H
#define JOYSTICKLIB_H

#include <Arduino.h>

// Struktur zum Speichern der X- und Y-Koordinaten des Joysticks
struct JoystickWerte {
  int x; // X-Koordinate
  int y; // Y-Koordinate
};

// Klasse zur Handhabung eines entprellten Tasters und der Joystick-Werte
class EntprellterTaster {
  private:
    int pin;                          // Pin des Tasters
    int xPin, yPin;                   // Pins für die X- und Y-Achse
    bool entprellterZustand;          // Entprellter Zustand des Tasters
    unsigned long letzteAenderung;   // Zeit der letzten Änderung des Zustands
    unsigned long druckBeginn;       // Zeitpunkt, an dem der Taster gedrückt wurde
    bool gedruecktGemeldet;           // Flag, ob der Tasterzustand bereits gemeldet wurde
    static const unsigned long ENT_PRELL_ZEIT = 50;    // Entprellzeit in Millisekunden
    static const unsigned long LANGE_DRUCK_ZEIT = 1000; // Zeit für langen Druck in Millisekunden

  public:
    // Konstruktor: Initialisiert die Pins und den Tasterzustand
    EntprellterTaster(int tasterPin, int xPin, int yPin);

    // Methode zur Aktualisierung des Tasterzustands
    void aktualisiere();

    // Prüft, ob der Taster gedrückt ist
    bool istGedrueckt();

    // Prüft, ob der Taster kurz gedrückt wurde
    bool wurdeGedrueckt();

    // Prüft, ob der Taster lange gedrückt wurde
    bool wurdeLangeGedrueckt();

    // Methode zum Auslesen der X- und Y-Werte des Joysticks
    JoystickWerte leseXY();
};

#endif
