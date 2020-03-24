#ifndef Pivot_h
#define Pivot_h

#include <Servo.h>

#include "Arduino.h"

class Pivot {
   public:
    enum Status {
        MOVING,
        STOPPED
    };
    enum Position {
        CENTER,
        RED,
        GREEN,
    };
    Pivot(int pin);
    void setPosition(Position);
    void reset();
    void update();
    Status getStatus();

   private:
    int _pin;
    Servo _servo;
    Position _commandedPosition;
    uint16_t _degRed = 55;
    uint16_t _degGreen = 110;
    uint16_t _degCentre = 82;
    Status _status;
    uint8_t _msStepTime = 10;
    uint32_t _startMillis;
    uint8_t _degPos;
};

#endif