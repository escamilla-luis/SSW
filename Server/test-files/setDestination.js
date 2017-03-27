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

// set Destination form 1 to 2
//helper.manipData('00060102');

// set Destination form 1 to 3
helper.writeToXbee('01060103');

// set Destination form 1 to 4
//helper.manipData('01060104');

// set Destination form 2 to 1
//helper.manipData('01060201');

// set Destination form 2 to 3
//helper.manipData('01060203');

// set Destination form 2 to 4
//helper.manipData('01060204');

// set Destination form 3 to 1
//helper.manipData('01060301');

// set Destination form 3 to 2
//helper.manipData('01060302');

// set Destination form 3 to 4
//helper.manipData('01060304');

// set Destination form 4 to 1
//helper.manipData('01060401');

// set Destination form 4 to 2
//helper.manipData('01060402');

// set Destination form 4 to 3
//helper.manipData('01060403');

