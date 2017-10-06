/*
 *  node-arduino: Control your Arduino with Node
 *
 *  Copyright (c) 2017 Anton Pashchenko
 *  node-arduino is freely distributable under the terms of the MIT license.
 */

#define SERIAL_BAUDRATE 115200

#define timeDelta 2000

#define OPC_PIN_MODE             0x01
#define OPC_DIGITAL_READ         0x02
#define OPC_DIGITAL_WRITE        0x03
#define OPC_ANALOG_REFERENCE     0x04
#define OPC_ANALOG_READ          0x05
#define OPC_ANALOG_WRITE         0x06
#define OPC_ANALOG_READ_AVERAGE  0x07
#define OPC_PIN_MODE_DEFAULT     0x08
#define OPC_READ_TEMP            0x09
#define OPC_READ_TEMP_HUM        0x0A
#define OPC_READ_LIGHT_STATUS    0x0B
#define OPC_TOGGLE_RELAY         0x0C
#define OPC_TOGGLE_RELAYS        0x0D

long pinVal = 0;
long inpVal = 0;
long outVal = 0;

long currentPin = 0;

int SIG_pin1 = A6;
int SIG_pin2 = A7;

int maxChannel = 32;
int controlPinLight[] = {2, 3, 4, 5};
int muxChannel[16][4]={
  {0,0,0,0}, //channel 0
  {1,0,0,0}, //channel 1
  {0,1,0,0}, //channel 2
  {1,1,0,0}, //channel 3
  {0,0,1,0}, //channel 4
  {1,0,1,0}, //channel 5
  {0,1,1,0}, //channel 6
  {1,1,1,0}, //channel 7
  {0,0,0,1}, //channel 8
  {1,0,0,1}, //channel 9
  {0,1,0,1}, //channel 10
  {1,1,0,1}, //channel 11
  {0,0,1,1}, //channel 12
  {1,0,1,1}, //channel 13
  {0,1,1,1}, //channel 14
  {1,1,1,1}  //channel 15
};

long timeM;
void setup() {
    for (int i = 22; i< 55; i++) {
        pinMode(i, OUTPUT);
        digitalWrite(i, HIGH);
    }

    for(int i = 0; i < 4; i ++){
        pinMode(controlPinLight[i], OUTPUT);
        digitalWrite(controlPinLight[i], LOW);
    }

    Serial.begin(SERIAL_BAUDRATE);

    timeM = millis();
}

void loop() {
    pinVal = 0, inpVal = 0, outVal = 0;
    if (Serial.available() > 0) {
    delay(1);
        switch (Serial.read()) {
            case OPC_PIN_MODE: {
                delay(1);
                pinVal = Serial.read();
                inpVal = Serial.read();
                pinMode(pinVal, inpVal);
                break;
            }
            case OPC_ANALOG_READ: {
                delay(1);
                pinVal = Serial.read();
                inpVal = analogRead(pinVal);
                outVal = pinVal << 16 | inpVal;
                Serial.println(outVal);
                break;
            }
            case OPC_READ_LIGHT_STATUS: {
                delay(1);
                pinVal = Serial.read();

                inpVal = readMux(pinVal);

                outVal = pinVal << 16 | inpVal;
                Serial.println(outVal);
                break;
            }
            case OPC_TOGGLE_RELAY: {
                delay(1);
                inpVal = Serial.read();

                digitalWrite(inpVal, LOW);

                delay(200);

                digitalWrite(inpVal, HIGH);
                break;
            }
            case OPC_TOGGLE_RELAYS: {
                delay(1);
                inpVal = Serial.read();
                long length = Serial.read();
                int pins[length];

                for (int i = 0; i < length; i++) {
                    pinVal = Serial.read();
                    pins[i] = pinVal;
                    digitalWrite(pinVal, LOW);
                }

                delay(200);

                for (int i = 0; i < length; i++) {
                    digitalWrite(pins[i], HIGH);
                }
                break;
            }
        }
    }
    else {
      if (millis() - timeM > timeDelta / (maxChannel - 1)) {
        inpVal = readMux(currentPin);

        outVal = currentPin << 16 | inpVal;
        Serial.println(outVal);
        currentPin++;
        if (currentPin >= maxChannel) {
          currentPin = 0;
        }
        timeM = millis();
      }
    }
}

int readMux(int _channel){
    int channel = _channel;
    int SIG_pin = SIG_pin1;
    if (channel >= 16) {
        channel -= 16;
        SIG_pin = SIG_pin2;
    }

    for(int i = 0; i < 4; i ++){
        digitalWrite(controlPinLight[i], muxChannel[channel][i]);
    }

    return analogRead(SIG_pin);
}
