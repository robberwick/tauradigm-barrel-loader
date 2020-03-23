#include <Arduino.h>
#include <Wire.h>

#include "Jaws.h"
#include "Lifter.h"
#include "Pivot.h"
#include "SoftwareWire.h"

SoftwareWire softWire(A0, A1);

enum class State {
    WAIT = 0,
    LOOKING = 1,
    GRABBING = 2,
    GRABBED = 3,
    LIFTING = 4,
    RELEASING = 5,
    RESETTING = 6
};

enum class Colour {
    RED,
    GREEN
};

enum class Command {
    STOP,
    RUN
};

unsigned long GRABBING_TIMEOUT = 10;
unsigned long LIFTING_TIMEOUT = 60;
unsigned long RESET_TIMEOUT = max(GRABBING_TIMEOUT, LIFTING_TIMEOUT);

State state = State::WAIT;
Command currentCommand = Command::STOP;

uint8_t I2C_ADDR = 4;

uint8_t jawsPin = 0;
Jaws jaws(jawsPin);

uint8_t lifterPin = 2;
Lifter lifter(lifterPin);

uint8_t pivotPin = 1;
Pivot pivot(pivotPin);

void receiveEvent(int bytesRead);
void requestEvent();
void processStatus();

void setup() {
    Wire.begin(I2C_ADDR);          // join i2c bus with address #4
    Wire.onReceive(receiveEvent);  // register event
    Wire.onRequest(requestEvent);

    softWire.begin();    // join alternative i2c bus as master
    Serial.begin(9600);  // start serial for output
}

void loop() {
    processStatus();
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int bytesRead) {
    while (Wire.available()) {
        // set current status to received byte
        // TODO bounds checking
        uint8_t readByte = Wire.read();

        if (readByte >= (uint8_t)Command::STOP && readByte <= (uint8_t)Command::RUN) {
            // Serial.println("Command: ");
            // Serial.print(readByte);
            currentCommand = (Command)readByte;
        }
    }
}

void requestEvent() {
    Wire.write((uint8_t)state);
}

boolean timeoutHasPassed(unsigned long startMillis, unsigned long timeoutLength) {
    return millis() - startMillis >= timeoutLength;
}

Colour getBarrelColour() {
    // attempt to do colour recognition stuff here:

    // for now though always just return Colour::RED
    return Colour::RED;
}

boolean findBarrel() {
    //TODO Barrel detection stuff
    return true;
}

void processStatus() {
    static long startMillis;
    if ((currentCommand == Command::STOP) && (state != State::WAIT && state != State::RESETTING)) {
        lifter.down();
        pivot.reset();
        jaws.open();
        startMillis = millis();
        state = State::RESETTING;
    }

    switch (state) {
        // Inactive state - waiting
        case State::WAIT:
            if (currentCommand == Command::RUN) {
                state = State::LOOKING;
            }
            break;

        case State::LOOKING:
            // LOOKING FOR BARREL
            if (findBarrel()) {
                // reset startMillis
                startMillis = millis();
                // close jaws
                jaws.close();
                // set state to GRABBING
                state = State::GRABBING;
            }
            break;

        case State::GRABBING:
            // GRABBING - wait for the timeout to complete before moving to GRABBED
            if (timeoutHasPassed(startMillis, GRABBING_TIMEOUT)) {
                state = State::GRABBED;
            }

            break;

        case State::GRABBED:
            // barrel grabbed - check colour
            switch (getBarrelColour()) {
                case Colour::RED:
                    // Set lifting servo to up position
                    lifter.up();
                    // set pivot servo to red position
                    pivot.red();
                    startMillis = millis();
                    state = State::LIFTING;
                    break;

                case Colour::GREEN:
                    // Set lifting servo to up position
                    lifter.up();
                    // set pivot servo to green position
                    pivot.green();
                    startMillis = millis();
                    state = State::LIFTING;
                    break;
            }

            if (getBarrelColour() == Colour::RED)
                break;

        case State::LIFTING:
            // if the LIFTING TIMEOUT has expired, open the jaws and move to RELEASING
            if (timeoutHasPassed(startMillis, LIFTING_TIMEOUT)) {
                // Open the Jaws
                jaws.open();
                startMillis = millis();
                state = State::RELEASING;
            }
            break;

        case State::RELEASING:
            if (timeoutHasPassed(startMillis, GRABBING_TIMEOUT)) {
                lifter.down();
                pivot.reset();
                startMillis = millis();
                state = State::RESETTING;
            }
            break;

        case State::RESETTING:
            if (timeoutHasPassed(startMillis, RESET_TIMEOUT)) {
                switch (currentCommand) {
                    case Command::RUN:
                        state = State::LOOKING;
                        break;
                    case Command::STOP:
                        state = State::WAIT;
                        break;
                }
            }
    }
}