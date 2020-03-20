#include "Lifter.h"

#include <Servo.h>

#include "Arduino.h"

Lifter::Lifter(int pin) : _msUp(1000), _msDown(2000) {
    pinMode(pin, OUTPUT);
    _pin = pin;
    _servo.attach(pin);
}

void Lifter::up() {
    _servo.writeMicroseconds(_msUp);
}

void Lifter::down() {
    _servo.writeMicroseconds(_msDown);
}