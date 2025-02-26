#include "Arduino.h"
#include "FahrzeugLib.h"

////////////////////////////////////////////////////
///Methoden und Konstruktoren der Klasse "CMotor"///
///////////////////////////////////////////////////

//Konstuktor der Klasse "CMotor"
CMotor::CMotor(int pinPWM, int pinNichtPWM)
{
  _pinPWM = pinPWM;
  _pinNichtPWM = pinNichtPWM;

  //Pin-Mode setzen
  pinMode(_pinPWM, OUTPUT);
  pinMode(_pinNichtPWM, OUTPUT);

  //Beide Pins auf LOW setzen
  digitalWrite(_pinPWM, LOW);
  digitalWrite(_pinNichtPWM, LOW);
}

//Methode, um einen Motor in eine bestimmte Richtung mit einer bestimmten Geschwindigkeit drehen zu lassen
void CMotor::Bewegen(bool vorwaerts, int geschw)
{

  if (vorwaerts)
  {
    digitalWrite(_pinNichtPWM, HIGH);
    analogWrite(_pinPWM, 255 - geschw);

  }
  else
  {
    digitalWrite(_pinNichtPWM, LOW);
    analogWrite(_pinPWM, geschw);
  }
}

//Methode, um den Motor zum Stillstand zu bringen
void CMotor::Stop()
{
  digitalWrite(_pinPWM, LOW);
  digitalWrite(_pinNichtPWM, LOW);

}


////////////////////////////////////////////////////
///Methoden und Konstruktoren der Klasse "CAntrieb"///
///////////////////////////////////////////////////


//Konstruktor der Klasse "CAntrieb"
CAntrieb::CAntrieb(CMotor *motorLinks, CMotor *motorRechts)
{
  MotorLinks = motorLinks;
  MotorRechts = motorRechts;
}

//Methode, um das Fahrzeug vorwaerts oder rückwaerts mit einer bestimmten Geschwindigkeit fahren zu lassen
void CAntrieb:: Gerade(bool vorwaerts, int geschw)
{
  MotorLinks -> Bewegen(vorwaerts, geschw);
  MotorRechts -> Bewegen(vorwaerts, geschw);
}

//Methode, um beide Motoren des Antriebs zum Stillstand zu bringen
void CAntrieb::Stop()
{
  MotorLinks -> Stop();
  MotorRechts -> Stop();

}

//Methode, um eine Wende in die angegebene Drehrichtung mit der angegebenen Geschwindigkeit durchzuführen
void CAntrieb::Wende(bool links, int geschw)
{
  if (links)
  {
    MotorLinks->Bewegen(false, geschw);
    MotorRechts->Bewegen(true, geschw);
  }
  else
  {
    MotorLinks->Bewegen(true, geschw);
    MotorRechts->Bewegen(false, geschw);
  }
}

//Methode, um eine Kurve mit einer bestimmten Form, mit einer gewissen Geschwindigkeit sowie nach vorne und nach hinten auszuführen
void CAntrieb::Kurve(double form, int geschw, bool vorwaerts)
{
  if (form >= 0)
  {
    MotorLinks->Bewegen(vorwaerts, geschw);
    MotorRechts->Bewegen(vorwaerts, geschw / pow(2, form));
  }
  else
  {
    form *= -1;
    MotorRechts -> Bewegen(vorwaerts, geschw);
    MotorLinks -> Bewegen(vorwaerts, geschw / pow(2, form));

  }
}

