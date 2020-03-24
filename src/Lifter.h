#ifndef Lifter_h
#define Lifter_h

#include <Servo.h>

#include "Arduino.h"

class Lifter {
   public:
    enum Status {
        UP,
        DOWN,
        LIFTING,
        LOWERING
    };
    Lifter(int pin);
    void up();
    void down();
    void update();
    Status getStatus();

   private:
    int _pin;
    Servo _servo;
    uint16_t _degUp = 55;
    uint16_t _degDown = 123;
    Status _status;
    uint8_t _msStepTime = 60;
    uint32_t _startMillis;
    uint8_t _degPos;
};

#endif