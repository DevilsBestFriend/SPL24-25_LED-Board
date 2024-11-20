// Pin für den Joystick-Taster
int buttonPin = 32; // Pin für den Taster

void setup() {
  Serial.begin(115200); // Serielle Kommunikation starten
  pinMode(buttonPin, INPUT_PULLUP); // Taster-Pin als Input mit Pullup-Widerstand
}

void loop() {
  // Taster Status lesen
  int buttonState = digitalRead(buttonPin);

  // Taster-Status ausgeben
  Serial.println(buttonState == LOW ? "Taster gedrückt" : "Taster nicht gedrückt");

  delay(200); // Wartezeit zwischen den Auslesungen
}
