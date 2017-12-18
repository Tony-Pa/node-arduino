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
const OPC_READ_TEMP = 0x09;
const OPC_READ_TEMP_HUM = 0x0A;
const OPC_READ_LIGHT_STATUS  = 0x0B;
const OPC_TOGGLE_RELAY  = 0x0C;
const OPC_TOGGLE_RELAYS  = 0x0D;
const OPC_HEALTH_CHECK = 0x10;

class Board {
    constructor(path) {
        this.callback = {};

        this.sp = new SerialPort(path, {
            autoOpen: false,
            baudRate: SERIAL_BAUD_RATE
        });
        const parser = this.sp.pipe(new SerialPort.parsers.Readline({ delimiter: '\r\n' }));

        parser.on('data', this.onData.bind(this));

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

    _registerCallback(pin, callback) {
        if (typeof callback === 'function') {
            this.callback['pin' + pin] = callback;
        }
    }

    onData(_data) {
        let pin = 'noData';
        let value = 'wrong pin';
        let data = _data.toString();
        let dataItems = data.split('/');

        if (dataItems.length) {
            pin = dataItems[0];
            value = dataItems.slice(1);
        }

        data = +_data;
        if (data >= 0) {
            pin = data >> 16;
            value = data & 0xFFFF;
        }

        if (typeof this.callback['pin' + pin] === 'function') {
            // console.log(pin, '-', value);
            this.callback['pin' + pin](value);
        }
    }

    healthCheck(callback) {
        this.sp.write(new Buffer([OPC_HEALTH_CHECK]));

        this._registerCallback('hc', callback);
    }

    pinMode(pin, mode) {
        this.sp.write(new Buffer([OPC_PIN_MODE, pin, mode]));
    }

    pinModeSetDefault(pin, mode, defaultVal) {
        this.sp.write(new Buffer([OPC_PIN_MODE_DEFAULT, pin, mode, defaultVal]));
    }

    digitalRead(pin, callback) {
        this.sp.write(new Buffer([OPC_DIGITAL_READ, pin]));

        this._registerCallback(pin, callback);
    }

    digitalWrite(pin, val) {
        this.sp.write(new Buffer([OPC_DIGITAL_WRITE, pin, val]));
    }

    analogReference(type) {
        this.sp.write(new Buffer([OPC_ANALOG_REFERENCE, type]));
    }

    analogRead(pin, callback) {
        this.sp.write(new Buffer([OPC_ANALOG_READ, pin]));

        this._registerCallback(pin, callback);
    }

    analogReadAverage(pin, count, callback) {
        this.sp.write(new Buffer([OPC_ANALOG_READ_AVERAGE, pin, count]));

        this._registerCallback(pin, callback);
    }

    analogWrite(pin, val) {
        this.sp.write(new Buffer([OPC_ANALOG_WRITE, pin, val]));
    }

    readTemp(pin, count, callback) {
        this.sp.write(new Buffer([OPC_READ_TEMP, pin, count]));

        this._registerCallback(pin, callback);
    }

    readTempHum(pin, callback) {
        this.sp.write(new Buffer([OPC_READ_TEMP_HUM, pin]));

        this._registerCallback(pin, callback);
    }

    readLightStatus(pin, callback) {
        this.sp.write(new Buffer([OPC_READ_LIGHT_STATUS, pin]));

        this._registerCallback(pin, callback);
    }

    toggleRelay(pin) {
        this.sp.write(new Buffer([OPC_TOGGLE_RELAY, pin]));
    }

    toggleLightRelay(pin) {
        const that = this.toggleLightRelay;
        that.pins = that.pins || [];
        that.pins.push(pin);

        clearTimeout(that.TO);
        that.TO = setTimeout(() => {
            this.toggleRelays(that.pins);
            that.pins = [];
        }, 100);
    }

    toggleRelays(pins) {
        console.log(pins);
        this.sp.write(new Buffer([OPC_TOGGLE_RELAYS, pins.length, ...pins]));
    }

    close() {
        this.sp.close();
    }
}

exports.connect = Board;
