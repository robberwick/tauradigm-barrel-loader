#include "Lifter.h"

#include <Servo.h>

#include "Arduino.h"

Lifter::Lifter(uint8_t pin) {
    _pin = pin;
}

void Lifter::begin() {
    pinMode(_pin, OUTPUT);
    _servo.attach(_pin);
}

void Lifter::up() {
    if (_status == Status::DOWN || _status == Status::LOWERING) {
        _status = Status::LIFTING;
        _startMillis = millis();
    }
    update();
}

void Lifter::down() {
    if (_status == Status::UP || _status == Status::LIFTING) {
        _status = Status::LOWERING;
        _startMillis = millis();
    }
    update();
}

void Lifter::update() {
    uint32_t currMillis = millis();
    uint8_t degStep;
    switch (_status) {
        case Status::DOWN:
            break;
        case Status::LIFTING:
            if (_degPos == _degUp) {
                _status = Status::UP;
            } else if ((currMillis - _startMillis) > _msStepTime) {
                degStep = (_degUp > _degPos) ? 1 : -1;
                _degPos = _degPos + degStep;
                _servo.write(_degPos);
                _startMillis = currMillis;
            }
            break;
        case Status::LOWERING:
            if (_degPos == _degDown) {
                _status = Status::DOWN;
            } else if ((currMillis - _startMillis) > _msStepTime) {
                degStep = (_degDown > _degPos) ? 1 : -1;
                _degPos = _degPos + degStep;
                _servo.write(_degPos);
                _startMillis = currMillis;
            }
            break;
        case Status::UP:
            break;
    }
}

Lifter::Status Lifter::getStatus() {
    return _status;
}