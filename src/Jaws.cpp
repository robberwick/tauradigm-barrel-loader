#include "Jaws.h"

#include <Servo.h>

#include "Arduino.h"

Jaws::Jaws(int pin) : _degOpen(140), _degClosed(90) {
    pinMode(pin, OUTPUT);
    _pin = pin;
    _servo.attach(pin);
}

void Jaws::open() {
    _servo.write(_degOpen);
}

void Jaws::close() {
    _servo.write(_degClosed);
}