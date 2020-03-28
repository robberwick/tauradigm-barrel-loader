#include "Pivot.h"

#include <Servo.h>

#include "Arduino.h"

Pivot::Pivot(uint8_t pin, uint32_t stepTime) {
    _pin = pin;
    _msStepTime = stepTime;
}

void Pivot::begin() {
    pinMode(_pin, OUTPUT);
    _servo.attach(_pin);
}

void Pivot::setPosition(Pivot::Position commandedPosition) {
    _commandedPosition = commandedPosition;
    _startMillis = millis();
    _status = Status::MOVING;
    update();
}

void Pivot::update() {
    uint32_t currMillis = millis();
    uint8_t targetPos;
    uint8_t degStep;

    switch (_commandedPosition) {
        case Position::CENTER:
            targetPos = _degCentre;
            break;

        case Position::GREEN:
            targetPos = _degGreen;
            break;

        case Position::RED:
            targetPos = _degRed;
            break;
    }

    if (_degPos == targetPos) {
        _status = Status::STOPPED;
        return;
    } else {
        if ((currMillis - _startMillis) >= _msStepTime) {
            degStep = (_degPos < targetPos) ? 1 : -1;
            _degPos = _degPos + degStep;
            _startMillis = currMillis;
            _servo.write(_degPos);
        }
    }
}

Pivot::Status Pivot::getStatus() {
    return _status;
}