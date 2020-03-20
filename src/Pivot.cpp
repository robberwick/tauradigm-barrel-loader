#include "Pivot.h"

#include <Servo.h>

#include "Arduino.h"

Pivot::Pivot(int pin) : _msRed(1000), _msGreen(2000), _msCentre(1500) {
    pinMode(pin, OUTPUT);
    _pin = pin;
    _servo.attach(pin);
}

void Pivot::red() {
    _servo.writeMicroseconds(_msRed);
}

void Pivot::green() {
    _servo.writeMicroseconds(_msGreen);
}

void Pivot::reset() {
    _servo.writeMicroseconds(_msCentre);
}