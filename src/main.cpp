#include <Arduino.h>
#include <Wire.h>

#include "SoftwareWire.h"

void receiveEvent(int bytesRead);
void requestEvent();

SoftwareWire softWire(A0, A1);

uint8_t status = 0;

uint8_t I2C_ADDR = 4;

void setup() {
    Wire.begin(I2C_ADDR);          // join i2c bus with address #4
    Wire.onReceive(receiveEvent);  // register event
    Wire.onRequest(requestEvent);

    softWire.begin();    // join alternative i2c bus as master
    Serial.begin(9600);  // start serial for output
}

void loop() {
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int bytesRead) {
    while (Wire.available()) {
        // set current status to received byte
        // TODO bounds checking
        status = Wire.read();
        Serial.print(status);
    }
}

void requestEvent() {
    Wire.write(status);
}