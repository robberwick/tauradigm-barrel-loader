#include <Arduino.h>
#include <Wire.h>

#include "SoftwareWire.h"

void receiveEvent(int howMany);

SoftwareWire softWire(PIN_WIRE_SDA, PIN_WIRE_SCL);

void setup() {
    pinMode(13, OUTPUT);
    Wire.begin(4);                 // join i2c bus with address #4
    Wire.onReceive(receiveEvent);  // register event

    softWire.begin();    // join alternative i2c bus as master
    Serial.begin(9600);  // start serial for output
}

void loop() {
    byte error, address;
    int nDevices;

    Serial.println("Scanning...");
    digitalWrite(13, HIGH);
    delay(1000);

    nDevices = 0;
    for (address = 1; address < 127; address++) {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        softWire.beginTransmission(address);
        error = softWire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println("  !");

            nDevices++;
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16) {
                Serial.print("0");
                Serial.println(address, HEX);
            }
        }
    }
    if (nDevices == 0) {
        Serial.println("No I2C devices found\n");
    } else {
        Serial.println("done\n");
        digitalWrite(13, LOW);
    }
    // delay(5000); // wait 5 seconds for next scan
    delay(1000);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
    while (1 < Wire.available())  // loop through all but the last
    {
        char c = Wire.read();  // receive byte as a character
        Serial.print(c);       // print the character
    }
    int x = Wire.read();  // receive byte as an integer
    Serial.println(x);    // print the integer
}