#include <Arduino.h>
#include <Wire.h>

#include "Adafruit_TCS34725softi2c.h"
#include "Jaws.h"
#include "Lifter.h"
#include "Pivot.h"

enum class State {
    WAIT = 0,
    LOOKING = 1,
    GRABBING = 2,
    GRABBED = 3,
    LIFTING = 4,
    RELEASING = 5,
    RESETTING = 6
};

enum class Command {
    STOP,
    RUN
};

Adafruit_TCS34725softi2c tcs = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X, A0, A1);

typedef struct {  // struct to hold raw color data
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t clear;
} Color;

enum Colors {
    OTHER,
    RED,
    GREEN,
    BLUE,
    UNKNOWN
};

Colors classifyColour(uint16_t r, uint16_t g, uint16_t b, uint16_t c);

unsigned long GRABBING_TIMEOUT = 50 * 10;
unsigned long LIFTING_TIMEOUT = 68 * 60;
unsigned long PIVOT_TIMEOUT = 28 * 40;
unsigned long RESET_TIMEOUT = max(PIVOT_TIMEOUT, max(GRABBING_TIMEOUT, LIFTING_TIMEOUT));

State state = State::WAIT;
Command currentCommand = Command::STOP;

uint8_t I2C_ADDR = 4;

uint8_t jawsPin = 12;  //0;
Jaws jaws(jawsPin, 10);

uint8_t lifterPin = 11;        //2;
Lifter lifter(lifterPin, 10);  // 60 is the original value from the test rig

uint8_t pivotPin = 10;  //1;
Pivot pivot(pivotPin, 10);

void receiveEvent(int bytesRead);
void requestEvent();
void processStatus();

void setup() {
    Wire.begin(I2C_ADDR);          // join i2c bus with address #4
    Wire.onReceive(receiveEvent);  // register event
    Wire.onRequest(requestEvent);

    Serial.begin(9600);  // start serial for output

    if (tcs.begin()) {
        Serial.println("Found sensor");
    } else {
        while (!tcs.begin()) {
            Serial.println("No TCS34725 found ... check your connections");
        }
    }

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

Colors getBarrelColour() {
    uint16_t clear, red, green, blue;

    tcs.getRawData(&red, &green, &blue, &clear);

    return classifyColour(red, green, blue, clear);
}

boolean findBarrel() {
    Colors colourFound = getBarrelColour();
    return colourFound == Colors::RED || colourFound == Colors::GREEN;
}

Colors classifyColour(uint16_t r, uint16_t g, uint16_t b, uint16_t c) {
    if (min(min(r, g), b) < 1000) {
        return Colors::UNKNOWN;
    }
    uint8_t acceptanceFactor = 1.5;
    if (r > (acceptanceFactor * g) && (r > (acceptanceFactor * b))) {
        return Colors::RED;
    } else if (g > (acceptanceFactor * r) && (g > (acceptanceFactor * b))) {
        return Colors::GREEN;
    } else if (b > (acceptanceFactor * r) && (b > (acceptanceFactor * g))) {
        return Colors::BLUE;
    }
    return Colors::OTHER;
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
                case Colors::RED:
                    // Set lifting servo to up position
                    lifter.up();
                    // set pivot servo to red position
                    pivot.setPosition(Pivot::Position::RED);
                    state = State::LIFTING;
                    break;

                case Colors::GREEN:
                    // Set lifting servo to up position
                    lifter.up();
                    // set pivot servo to green position
                    pivot.setPosition(Pivot::Position::GREEN);
                    state = State::LIFTING;
                    break;

                case Colors::BLUE:
                case Colors::OTHER:
                case Colors::UNKNOWN:
                default:
                    break;
            }

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