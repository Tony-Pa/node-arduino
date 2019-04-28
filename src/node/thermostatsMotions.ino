#include "DHT.h"
#include "DallasTemperature.h"
#include "OneWire.h"

#define SERIAL_BAUDRATE          115200
#define MOTION_SENSORS_NUMBER    1

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
#define OPC_SET_MOTION_WATCH     0x0E
#define OPC_HEALTH_CHECK         0x10

int currentMS = 1;
int motionSensors[MOTION_SENSORS_NUMBER] = {12};
int motionSensorsValue[MOTION_SENSORS_NUMBER] = {1};

long pinVal = 0;
long inpVal = 0;
long outVal = 0;

int timeDelta = 500;
long timeM;

void setup() {
    Serial.begin(SERIAL_BAUDRATE);

    timeM = millis();
}

void loop() {
    pinVal = 0, inpVal = 0, outVal = 0;
    if (Serial.available() > 0) {
    delay(1);
        switch (Serial.read()) {
            case OPC_HEALTH_CHECK: {
                delay(1);
                Serial.println("hc/");
                break;
            }
            case OPC_PIN_MODE: {
                delay(1);
                pinVal = Serial.read();
                inpVal = Serial.read();
                pinMode(pinVal, inpVal);
                break;
            }
            case OPC_PIN_MODE_DEFAULT: {
                delay(1);
                pinVal = Serial.read();
                inpVal = Serial.read();
                outVal = Serial.read();
                pinMode(pinVal, inpVal);
                digitalWrite(pinVal, outVal);
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
            case OPC_READ_TEMP: {
                delay(1);
                pinVal = Serial.read();
                int num = Serial.read();
                OneWire oneWire(pinVal);
                DallasTemperature sensors(&oneWire);
                sensors.begin();
                sensors.requestTemperatures();
                Serial.print(pinVal);
                Serial.print('/');
                for(int i = 0; i < num; i++) {
                  Serial.print(sensors.getTempCByIndex(i)); 
                  if (i + 1 < num) {
                    Serial.print('/');
                  }
                }
                Serial.println();
                break;
            }
            case OPC_READ_TEMP_HUM: {
                delay(1);
                pinVal = Serial.read();
                DHT dht(pinVal, DHT22);
                dht.begin();
                float h = dht.readHumidity();
                float t = dht.readTemperature();
                if (!isnan(h) && !isnan(t)) {
                  float hic = dht.computeHeatIndex(t, h, false);
                  Serial.print(pinVal);
                  Serial.print('/');
                  Serial.print(h);
                  Serial.print('/');
                  Serial.print(t);
                  Serial.print('/');
                  Serial.println(hic);
                }
                break;
            }
            case OPC_SET_MOTION_WATCH: {
                delay(1);
                pinVal = Serial.read();
                inpVal = digitalRead(pinVal);
                outVal = pinVal << 16 | inpVal;
                Serial.println(outVal);

                break;
            }
        }
    }
    else {
        if (millis() - timeM > timeDelta / MOTION_SENSORS_NUMBER) {
            pinVal = motionSensors[currentMS];
            int tmp = digitalRead(pinVal);

            if (tmp != motionSensorsValue[currentMS]) {
                motionSensorsValue[currentMS] = tmp;
                outVal = pinVal << 16 | tmp;
                Serial.println(outVal);
            }

            timeM = millis();
        }
    }
}
