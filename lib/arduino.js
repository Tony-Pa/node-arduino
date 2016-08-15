"use strict";

const SerialPort = require('serialport');

const SERIAL_BAUD_RATE = 115200;

const OPC_PIN_MODE = 0x01;
const OPC_DIGITAL_READ = 0x02;
const OPC_DIGITAL_WRITE = 0x03;
const OPC_ANALOG_REFERENCE = 0x04;
const OPC_ANALOG_READ = 0x05;
const OPC_ANALOG_WRITE = 0x06;
const OPC_ANALOG_READ_AVERAGE = 0x07;
const OPC_PIN_MODE_DEFAULT = 0x08;

class Board {
    constructor(path) {
        this.callback = {};

        this.sp = new SerialPort(path, {
            baudRate: SERIAL_BAUD_RATE,
            parser: SerialPort.parsers.readline('\n')
        });

        this.sp.on('data', this.onData.bind(this));

        this.HIGH = 0x01;
        this.LOW = 0x00;

        this.INPUT = 0x00;
        this.OUTPUT = 0x01;

        this.true = 0x01;
        this.false = 0x00;

        this.EXTERNAL = 0x00;
        this.DEFAULT = 0x01;
        this.INTERNAL = 0x03;
    }

    onData(data) {
        data = +data;
        if (data && data > 1) {
            var pin = data >> 16;
            var value = data & 0xFFFF;

            if (typeof this.callback['pin' + pin] === 'function') {
                this.callback['pin' + pin](value);
            }
        }
    }

    pinMode(pin, mode) {
        this.sp.write(new Buffer([OPC_PIN_MODE, pin, mode]));
    }

    pinModeSetDefault(pin, mode, defaultVal) {
        this.sp.write(new Buffer([OPC_PIN_MODE_DEFAULT, pin, mode, defaultVal]));
    }

    digitalRead(pin, callback) {
        this.sp.write(new Buffer([OPC_DIGITAL_READ, pin]));

        if (typeof callback === 'function') {
            this.callback['pin' + pin] = callback;
        }
    }

    digitalWrite(pin, val) {
        this.sp.write(new Buffer([OPC_DIGITAL_WRITE, pin, val]));
    }

    analogReference(type) {
        this.sp.write(new Buffer([OPC_ANALOG_REFERENCE, type]));
    }

    analogRead(pin, callback) {
        this.sp.write(new Buffer([OPC_ANALOG_READ, pin]));

        if (typeof callback === 'function') {
            this.callback['pin' + pin] = callback;
        }
    }

    analogReadAverage(pin, count, callback) {
        this.sp.write(new Buffer([OPC_ANALOG_READ_AVERAGE, pin, count]));

        if (typeof callback === 'function') {
            this.callback['pin' + pin] = callback;
        }
    }

    analogWrite(pin, val) {
        this.sp.write(new Buffer([OPC_ANALOG_WRITE, pin, val]));
    }

    close() {
        this.sp.close();
    }
}

exports.connect = Board;
