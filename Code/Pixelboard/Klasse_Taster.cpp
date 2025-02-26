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
