/*
 *  node-arduino: Control your Arduino with Node
 *
 *  Copyright (c) 2017 Anton Pashchenko
 *  node-arduino is freely distributable under the terms of the MIT license.
 */

const arduino = require('../lib/arduino');
const board = arduino.connect('/dev/tty.usbmodem621');

const pin = 7;

board.pinMode(pin, arduino.INPUT);
setInterval(() => board.digitalRead(pin, (data) => console.log(data)), 100);
