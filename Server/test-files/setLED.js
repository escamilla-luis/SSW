var util = require('util');
var SerialPort = require('serialport');
var helper = require('./helperGetSet.js');

// currently not in use
// var xbee_api = require('xbee-api');
// var C = xbee_api.constants;
// var xbeeAPI = new xbee_api.XBeeAPI({
//     api_mode: 1
// });

// For the computer's specific port
var serialPortId = '/dev/ttyUSB1';

// Create the port that we will be using. 
// Note the baudrate and how it needs to be forceably opened.
var port = new SerialPort(serialPortId, {
    autoOpen: false,
    baudrate: 57600,
    //parser: xbeeAPI.rawParser()
});

// Red
helper.manipData('02020001');

// Blue
//helper.manipData('02020002');

// Green
//helper.manipData('01020003');

// Flashing Red
//helper.manipData('01020004');

// Flashing Blue
//helper.manipData('01020005');

// Flashing Green
//helper.manipData('01020006');