#include "Klasse_Taster.cpp" // Annahme: Dateiname für die TasterEntpreller-Klasse

// Pin für den Taster
const int tasterPin = 32; // Beispiel-Pin, an dem der Taster angeschlossen ist

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

    // Überprüfe, ob der Taster seit dem letzten Mal gedrückt wurde
    if (taster.wurdeGedrueckt()) {
        Serial.println("Taster wurde gedrückt");
    }

    delay(100); // Kleine Verzögerung für die Schleife
}
