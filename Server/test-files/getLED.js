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

helper.manipData('01030000');