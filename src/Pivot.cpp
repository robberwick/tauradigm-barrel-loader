#include "Pivot.h"

#include <Servo.h>

#include "Arduino.h"

Pivot::Pivot(int pin) {
    pinMode(pin, OUTPUT);
    _pin = pin;
    _servo.attach(pin);
}

void Pivot::setPosition(Pivot::Position commandedPosition) {
    _commandedPosition = commandedPosition;
    _startMillis = millis();
    update();
}

void Pivot::reset() {
    _servo.write(_degCentre);
}

void Pivot::update() {
    uint32_t currMillis = millis();
    uint8_t targetPos;
    int8_t degStep;

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
            _servo.write(_degPos);
        }
    }
}

Pivot::Status Pivot::getStatus() {
    return _status;
}