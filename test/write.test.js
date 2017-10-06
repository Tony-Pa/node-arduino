/*
 *  node-arduino: Control your Arduino with Node
 *
 *  Copyright (c) 2010 Anton Pashchenko
 *  node-arduino is freely distributable under the terms of the MIT license.
 */

const arduino = require('../lib/arduino');
const board = arduino.connect('/dev/tty.usbserial-A700fkLn');
let val = arduino.LOW;

const pin = 13;

board.pinMode(pin, arduino.OUTPUT);

setInterval(() => {
    val = val === arduino.LOW ? arduino.HIGH : arduino.LOW;

    board.digitalWrite(pin, val);
}, 500);
