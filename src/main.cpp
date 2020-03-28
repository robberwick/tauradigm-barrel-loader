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

unsigned long GRABBING_TIMEOUT = 50 * 10;
unsigned long LIFTING_TIMEOUT = 68 * 60;
unsigned long PIVOT_TIMEOUT = 28 * 40;
unsigned long RESET_TIMEOUT = max(PIVOT_TIMEOUT, max(GRABBING_TIMEOUT, LIFTING_TIMEOUT));

State state = State::WAIT;
Command currentCommand = Command::STOP;

uint8_t I2C_ADDR = 4;

uint8_t jawsPin = 12;  //0;
Jaws jaws(jawsPin);

uint8_t lifterPin = 11;  //2;
Lifter lifter(lifterPin);

uint8_t pivotPin = 10;  //1;
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

    jaws.begin();
    pivot.begin();
    lifter.begin();
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
    if ((currentCommand == Command::STOP) && (state != State::WAIT && state != State::RESETTING)) {
        lifter.down();
        pivot.setPosition(Pivot::Position::CENTER);
        jaws.open();
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
                // close jaws
                jaws.close();
                // set state to GRABBING
                state = State::GRABBING;
            }
            break;

        case State::GRABBING:
            // GRABBING - wait for the Jaws status to be CLOSED before moving to GRABBED
            if (jaws.getStatus() == Jaws::Status::CLOSED) {
                state = State::GRABBED;
            } else {
                jaws.update();
            }

            break;

        case State::GRABBED:
            // barrel grabbed - check colour
            switch (getBarrelColour()) {
                case Colour::RED:
                    // Set lifting servo to up position
                    lifter.up();
                    // set pivot servo to red position
                    pivot.setPosition(Pivot::Position::RED);
                    state = State::LIFTING;
                    break;

                case Colour::GREEN:
                    // Set lifting servo to up position
                    lifter.up();
                    // set pivot servo to green position
                    pivot.setPosition(Pivot::Position::GREEN);
                    state = State::LIFTING;
                    break;
            }

            if (getBarrelColour() == Colour::RED)
                break;

        case State::LIFTING:
            // if the LIFTING TIMEOUT has expired, open the jaws and move to RELEASING
            if (lifter.getStatus() == Lifter::Status::UP && pivot.getStatus() == Pivot::Status::STOPPED) {
                // Open the Jaws
                jaws.open();
                state = State::RELEASING;
            } else {
                lifter.update();
                pivot.update();
            }

            break;

        case State::RELEASING:
            if (jaws.getStatus() == Jaws::Status::OPEN) {
                lifter.down();
                pivot.setPosition(Pivot::Position::CENTER);
                ;
                state = State::RESETTING;
            } else {
                jaws.update();
            }
            break;

        case State::RESETTING:
            if (lifter.getStatus() == Lifter::Status::DOWN && pivot.getStatus() == Pivot::Status::STOPPED) {
                switch (currentCommand) {
                    case Command::RUN:
                        state = State::LOOKING;
                        break;
                    case Command::STOP:
                        state = State::WAIT;
                        break;
                }
            } else {
                lifter.update();
                pivot.update();
            }
    }
}