#ifndef Lifter_h
#define Lifter_h

#include <Servo.h>

#include "Arduino.h"

class Lifter {
   public:
    Lifter(int pin);
    void up();
    void down();

   private:
    int _pin;
    Servo _servo;
    uint16_t _degUp;
    uint16_t _degDown;
};

#endif