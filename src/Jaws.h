#ifndef Jaws_h
#define Jaws_h

#include <Servo.h>

#include "Arduino.h"

class Jaws {
   public:
    Jaws(int pin);
    void open();
    void close();

   private:
    int _pin;
    Servo _servo;
    uint16_t _degOpen;
    uint16_t _degClosed;
};

#endif