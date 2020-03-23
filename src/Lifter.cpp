#include "Lifter.h"

#include <Servo.h>

#include "Arduino.h"

Lifter::Lifter(int pin) : _degUp(55), _degDown(123) {
    pinMode(pin, OUTPUT);
    _pin = pin;
    _servo.attach(pin);
}

void Lifter::up() {
    _servo.write(_degUp);
}

void Lifter::down() {
    _servo.write(_degDown);
}