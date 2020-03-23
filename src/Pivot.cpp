#include "Pivot.h"

#include <Servo.h>

#include "Arduino.h"

Pivot::Pivot(int pin) : _degRed(55), _degGreen(110), _degCentre(82) {
    pinMode(pin, OUTPUT);
    _pin = pin;
    _servo.attach(pin);
}

void Pivot::red() {
    _servo.write(_degRed);
}

void Pivot::green() {
    _servo.write(_degGreen);
}

void Pivot::reset() {
    _servo.write(_degCentre);
}