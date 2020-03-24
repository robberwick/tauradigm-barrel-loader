#ifndef Jaws_h
#define Jaws_h

#include <Servo.h>

#include "Arduino.h"

class Jaws {
   public:
    enum Status {
        OPEN,
        CLOSED,
        OPENING,
        CLOSING
    };
    Jaws(int pin);
    void open();
    void close();
    void update();
    Status getStatus();

   private:
    int _pin;
    Servo _servo;
    uint16_t _degOpen = 140;
    uint16_t _degClosed = 90;
    Status _status;
    uint8_t _msStepTime = 10;
    uint32_t _startMillis;
    uint8_t _degPos;
};

#endif