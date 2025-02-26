class Taster {
private:
    int pin;
    bool aktuellerZustand;
    bool vorherigerZustand;
    unsigned long gedruecktStartZeit;
    unsigned long entprellZeit;
    bool wurdeGedruecktFlag;
    bool wurdeLangeGedruecktFlag;
    const unsigned long langeDruckSchwelle = 1000; // 1 Sekunde

public:
    Taster(int tasterPin) : pin(tasterPin), aktuellerZustand(false), vorherigerZustand(false),
                            gedruecktStartZeit(0), entprellZeit(50), 
                            wurdeGedruecktFlag(false), wurdeLangeGedruecktFlag(false) {
        pinMode(pin, INPUT);
    }

<<<<<<< HEAD
    // Wenn die Entprellzeit abgelaufen ist und der Zustand sich stabilisiert hat
    if (millis() - letzteZeit > entprellZeit) {
        if (tasterStatus && !istGedrueckt) {
            // Taster wurde gerade gedrückt, Zeitpunkt speichernn
            gedruecktZeitpunkt = millis();
=======
    void aktualisiere() {
        bool gelesen = digitalRead(pin);
        if (gelesen != aktuellerZustand) {
            if (millis() - gedruecktStartZeit >= entprellZeit) {
                vorherigerZustand = aktuellerZustand;
                aktuellerZustand = gelesen;

                if (aktuellerZustand) { // Taster wird gedrückt
                    gedruecktStartZeit = millis();
                } else { // Taster wird losgelassen
                    if (millis() - gedruecktStartZeit >= langeDruckSchwelle) {
                        wurdeLangeGedruecktFlag = true;
                    } else {
                        wurdeGedruecktFlag = true;
                    }
                }
            }
        } else {
            if (aktuellerZustand && (millis() - gedruecktStartZeit >= langeDruckSchwelle)) {
                wurdeLangeGedruecktFlag = true;
            }
>>>>>>> f7c4a6a67c622cf01bf06e58d66bd610d196010e
        }
    }

    bool istGedrueckt() {
        return aktuellerZustand;
    }

    bool wurdeGedrueckt() {
        if (wurdeGedruecktFlag) {
            wurdeGedruecktFlag = false; // Zurücksetzen
            return true;
        }
        return false;
    }

    bool wurdeLangeGedrueckt() {
        if (wurdeLangeGedruecktFlag) {
            wurdeLangeGedruecktFlag = false; // Zurücksetzen
            return true;
        }
        return false;
    }
};
