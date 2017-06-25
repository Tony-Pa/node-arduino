#define SERIAL_BAUDRATE 115200

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
#define OPC_SET_LIGHT_RELAY      0x0C

long pinVal = 0;
long inpVal = 0;
long outVal = 0;
long currentPin = 0;

int SIG_pin1 = A6;
int SIG_pin2 = A7;

int SIG_pin3 = 10;
int SIG_pin4 = 11;

int maxChannel = 32;
int controlPinLight[] = {2, 3, 4, 5};
int controlPinRelay[] = {6, 7, 8, 9};
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
    for(int i = 0; i < 4; i ++){
        digitalWrite(controlPinLight[i], OUTPUT);
        digitalWrite(controlPinLight[i], LOW);
        digitalWrite(controlPinRelay[i], OUTPUT);
        digitalWrite(controlPinRelay[i], LOW);
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
            case OPC_ANALOG_READ_AVERAGE: {
                delay(1);
                pinVal = Serial.read();
                int count = Serial.read();

                inpVal = analogReadAverage(pinVal, count);

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
            case OPC_SET_LIGHT_RELAY: {
                delay(1);
                pinVal = Serial.read();
                inpVal = Serial.read();

                readMuxDigital(pinVal, inpVal);
                break;
            }
        }
    }
    else {
      if (millis() - timeM > 5000 / maxChannel) {
        inpVal = readMux(currentPin);

        outVal = pinVal << 16 | inpVal;
        Serial.println(outVal);
        currentPin++;
        if (currentPin > maxChannel) {
          currentPin = 0;
        }
        timeM = millis();
      }
    }
}

int analogReadAverage(int pinVal, int count) {
  int avArray[count];
  for (int i = 0; i < count; i++) {
      avArray[i] = analogRead(pinVal);
      delay(1);
  }

  int t;
  for(int i = 1; i < count; i++)
      for(int j = 0; j < count - i; j++)
          if(avArray[j] > avArray[j+1]) {
              t = avArray[j];
              avArray[j] = avArray[j + 1];
              avArray[j + 1] = t;
          }

  int center = count / 2;
  return avArray[center];
}

int readMux(int _channel){
    int channel = _channel - 1;

    int SIG_pin = SIG_pin1;
    if (channel >= 16) {
        channel -= 16;
        SIG_pin = SIG_pin2;
    }

    for(int i = 0; i < 4; i ++){
        digitalWrite(controlPinLight[i], muxChannel[channel][i]);
    }

    int val = analogReadAverage(SIG_pin, 10);
    return val;
}

void readMuxDigital(int _channel, int value){
    int channel = _channel - 1;

    int SIG_pin = SIG_pin3;
    if (channel >= 16) {
        channel -= 16;
        SIG_pin = SIG_pin4;
    }

    for(int i = 0; i < 4; i ++){
        digitalWrite(controlPinLight[i], muxChannel[channel][i]);
    }

    digitalWrite(SIG_pin, value);
}
