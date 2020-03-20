#include "Jaws.h"

#include <Servo.h>

#include "Arduino.h"

Jaws::Jaws(int pin) : _msOpen(1000), _msClosed(2000) {
    pinMode(pin, OUTPUT);
    _pin = pin;
    _servo.attach(pin);
}

void Jaws::open() {
    _servo.writeMicroseconds(_msOpen);
}

void Jaws::close() {
    _servo.writeMicroseconds(_msClosed);
}