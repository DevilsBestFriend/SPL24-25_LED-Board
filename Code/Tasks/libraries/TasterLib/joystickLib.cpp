#include "joystickLib.h"

// Konstruktor: Initialisiert die Pins und den Tasterzustand
EntprellterTaster::EntprellterTaster(int tasterPin, int xPin, int yPin)
  : pin(tasterPin), xPin(xPin), yPin(yPin), entprellterZustand(false), letzteAenderung(0), druckBeginn(0), gedruecktGemeldet(true) {
  pinMode(pin, INPUT_PULLUP); // Taster-Pin als Eingang mit Pullup-Widerstand konfigurieren
}

// Aktualisiert den Tasterzustand und entprellt den Eingang
void EntprellterTaster::aktualisiere() {
  bool aktuellerZustand = !digitalRead(pin); // Zustand des Tasters einlesen (invertiert wegen Pullup)
  unsigned long aktuelleZeit = millis();

  // Prüfen, ob der Zustand sich geändert hat und ob die Entprellzeit verstrichen ist
  if (aktuellerZustand != entprellterZustand && (aktuelleZeit - letzteAenderung) > ENT_PRELL_ZEIT) {
    entprellterZustand = aktuellerZustand;
    letzteAenderung = aktuelleZeit;

    // Wenn der Taster gedrückt wurde, speichere den Beginn des Drucks
    if (entprellterZustand) {   
      druckBeginn = aktuelleZeit;
      gedruecktGemeldet = false;
    }
  }
}

// Gibt zurück, ob der Taster derzeit gedrückt ist
bool EntprellterTaster::istGedrueckt() {
  return entprellterZustand;
}

// Gibt zurück, ob der Taster kurz gedrückt wurde
bool EntprellterTaster::wurdeGedrueckt() {
  if (!entprellterZustand && !gedruecktGemeldet) {
    if (millis() - druckBeginn < LANGE_DRUCK_ZEIT) {
      gedruecktGemeldet = true;
      return true;
    }
  }
  return false;
}

// Gibt zurück, ob der Taster lange gedrückt wurde
bool EntprellterTaster::wurdeLangeGedrueckt() {
  if (entprellterZustand && !gedruecktGemeldet) {
    if (millis() - druckBeginn >= LANGE_DRUCK_ZEIT) {
      gedruecktGemeldet = true;
      return true;
    }
  }
  return false;
}

// Liest die X- und Y-Werte des Joysticks und gibt sie zurück
JoystickWerte EntprellterTaster::leseXY() {
  JoystickWerte werte;
  werte.x = analogRead(xPin); // X-Wert lesen
  werte.y = analogRead(yPin); // Y-Wert lesen
  return werte;
}
