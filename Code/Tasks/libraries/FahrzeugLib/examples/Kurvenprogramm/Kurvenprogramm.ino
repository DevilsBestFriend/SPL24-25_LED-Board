//Einbinden der Fahrzeug-Library
#include <FahrzeugLib.h>

/*
 * geschw ist ein Wert von 0 bis 255
 * form gibt den Kurvenradius an: form = 0 -> geradeaus
 *                                form = -1 -> Linkskurve, linkes Rad ist halb so schnell wie rechtes
 *                                form = 1 -> Rechtskurve, rechtes Rad ist halb so schnell wie linkes
 *                                
 * Methoden für Klasse CMotor:
    CMotor(int pinPWM, int pinNichtPWM);
    void Bewegen(bool vorwaerts, int geschw);
    void Stop();

 * Methoden für Klasse CAntrieb:
    CAntrieb(CMotor *motor1, CMotor *motor2);
    void Gerade(bool vorwaerts, int geschw);
    void Kurve(double form, int geschw, bool vorwaerts);
    void Stop();
    void Wende(bool links, int geschw);
 */

//Erstellen von zwei Objekten vom Typ "C-Motor"
CMotor MotorLinks(13,12);  // Der Pin, der im ersten Parameter angegeben ist, muss PWM-fähig sein, der zweite nicht unbedingt
CMotor MotorRechts(9,8);

//Erstellen eines Objektes vom Type "CAntrieb"
CAntrieb Antrieb(&MotorLinks, &MotorRechts);

void setup() {
  MotorLinks.Bewegen(true,200);
  MotorRechts.Bewegen(true,200);
  delay(1000);
  MotorLinks.Stop();
  MotorRechts.Stop();
  delay(1000);
}

void loop() {

delay(1000);

//Rechtskurve vorwaerts, 1 --> inneres Rad bei der Kurve
//dreht sich nur halb so schnell wie das äußere Rad
Antrieb.Kurve(1,255, true);
delay(5000);

//Gefährt stoppen
Antrieb.Stop();
delay(1000);

//Rechtskurve mit etwas größerem Radius vorwaerts
Antrieb.Kurve(0.80,255, true);
delay(10000);

//Gefährt stoppen
Antrieb.Stop();
delay(1000);

//Rechtskurve mit noch etwas größerem Radius vorwaerts
Antrieb.Kurve(0.65,255, true);
delay(15000);

//Gefährt stoppen
Antrieb.Stop();
delay(1000);

//Rechtskurve mit gleichem Radius wie vorher vorwaerts, jedoch mit einer geringeren Geschwindigkeit
Antrieb.Kurve(0.65,200, true);
delay(20000);

//Gefährt stoppen
Antrieb.Stop();
delay(1000);

//Linkskurve vorwaerts
Antrieb.Kurve(-0.75,255, true);
delay(10000);

//Gefährt stoppen
Antrieb.Stop();
delay(1000);
}
