//nur ausführen, falls das Flag mit diesem Namen noch nicht gesetzt wurde
#ifndef CAntrieb_h

//Flag definieren
#define CAntrieb_h

#include "Arduino.h"


class CMotor
{
  public:
    CMotor(int pinPWM, int pinNichtPWM);
    void Bewegen(bool vorwaerts, int geschw);
    void Stop();
  private:
    int _pinPWM;
    int _pinNichtPWM;
};

class CAntrieb
{
  public:
    CAntrieb(CMotor *motor1, CMotor *motor2);
    void Gerade(bool vorwaerts, int geschw);
    void Kurve(double form, int geschw, bool vorwaerts);
    void Stop();
    void Wende(bool links, int geschw);
  private:
    CMotor *MotorLinks;
    CMotor *MotorRechts;
};

#endif
