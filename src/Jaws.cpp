#include "Jaws.h"

#include <Servo.h>

#include "Arduino.h"

Jaws::Jaws(uint8_t pin) {
    _pin = pin;
}

void Jaws::begin() {
    pinMode(_pin, OUTPUT);
    _servo.attach(_pin);
}

void Jaws::open() {
    if (_status == Status::CLOSED || _status == Status::CLOSING) {
        _status = Status::OPENING;
        _startMillis = millis();
    }
    update();
}

void Jaws::close() {
    if (_status == Status::OPEN || _status == Status::OPENING) {
        _status = Status::CLOSING;
        _startMillis = millis();
    }
    update();
}

void Jaws::update() {
    uint32_t currMillis = millis();
    uint8_t degStep;

    switch (_status) {
        case Status::OPEN:
            break;
        case Status::OPENING:
            if (_degPos == _degOpen) {
                _status = Status::OPEN;
            } else if ((currMillis - _startMillis) > _msStepTime) {
                degStep = (_degOpen > _degPos) ? 1 : -1;
                _degPos = _degPos + degStep;
                _servo.write(_degPos);
                _startMillis = currMillis;
            }
            break;
        case Status::CLOSING:
            if (_degPos == _degClosed) {
                _status = Status::CLOSED;
            } else if ((currMillis - _startMillis) > _msStepTime) {
                degStep = (_degClosed > _degPos) ? 1 : -1;
                _degPos = _degPos + degStep;
                _servo.write(_degPos);
                _startMillis = currMillis;
            }
            break;
        case Status::CLOSED:
            break;
    }
}

Jaws::Status Jaws::getStatus() {
    return _status;
}