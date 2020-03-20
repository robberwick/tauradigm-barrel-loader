#ifndef Pivot_h
#define Pivot_h

#include <Servo.h>

#include "Arduino.h"

class Pivot {
   public:
    Pivot(int pin);
    void red();
    void green();
    void reset();

   private:
    int _pin;
    Servo _servo;
    uint16_t _msRed;
    uint16_t _msGreen;
    uint16_t _msCentre;
};

#endif