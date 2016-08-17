#define SERIAL_BAUDRATE 115200

#define OPC_PIN_MODE             0x01
#define OPC_DIGITAL_READ         0x02
#define OPC_DIGITAL_WRITE        0x03
#define OPC_ANALOG_REFERENCE     0x04
#define OPC_ANALOG_READ          0x05
#define OPC_ANALOG_WRITE         0x06
#define OPC_ANALOG_READ_AVERAGE  0x07

long pinVal = 0;
long inpVal = 0;
long outVal = 0;

void setup() {
    Serial.begin(SERIAL_BAUDRATE);
}

void loop() {
    pinVal = 0, inpVal = 0, outVal = 0;
    while (Serial.available() > 0) {
      Serial.println('-');
    delay(1);
        switch (Serial.read()) {
            case OPC_PIN_MODE: {
                delay(1);
                pinVal = Serial.read();
                outVal = Serial.read();
                pinMode(pinVal, outVal);
                break;
            }
            case OPC_DIGITAL_READ: {
                delay(1);
                pinVal = Serial.read();
                inpVal = digitalRead(pinVal);
                outVal = pinVal << 16 | inpVal;
                Serial.println(outVal);
                break;
            }
            case OPC_DIGITAL_WRITE: {
                delay(1);
                inpVal = Serial.read();
                outVal = Serial.read();
                digitalWrite(inpVal, outVal);
                break;
            }
            case OPC_ANALOG_REFERENCE: {
                inpVal = Serial.read();
                analogReference(inpVal);
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
                inpVal = avArray[center];
                outVal = pinVal << 16 | inpVal;
                Serial.println(outVal);
                break;
            }
            case OPC_ANALOG_WRITE: {
                delay(1);
                inpVal = Serial.read();
                outVal = Serial.read();
                analogWrite(inpVal, outVal);
                break;
            }
        }
    }
}
