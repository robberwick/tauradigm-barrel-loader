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
    Pivot(uint8_t pin, uint32_t stepTime);
    void begin();
    void setPosition(Position);
    void update();
    Status getStatus();

   private:
    uint8_t _pin;
    Servo _servo;
    Position _commandedPosition;
    uint16_t _degRed = 55;
    uint16_t _degGreen = 110;
    uint16_t _degCentre = 82;
    Status _status;
    uint32_t _msStepTime;
    uint32_t _startMillis;
    uint8_t _degPos;
};

#endif