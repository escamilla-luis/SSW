var util = require('util');
var SerialPort = require('serialport');
var xbee_api = require('xbee-api');

var C = xbee_api.constants;
var xbeeAPI = new xbee_api.XBeeAPI({
    api_mode: 1
    //module: 'Any',
    //raw_frames: true
});

var serialPortId = '/dev/ttyUSB1';

var port = new SerialPort(serialPortId, {
    autoOpen: false,
    baudrate: 57600,
    //parser: xbeeAPI.rawParser()
});

port.open(function (err) {
  if (err) {
    return console.log('Error opening port: ', err.message);
  }

  // write errors will be emitted on the port since there is no callback to write
  port.write('main screen turn on');
});

// the open event will always be emitted
port.on('open', function() {
  // open logic
});

//You can get updates of new data from the Serial Port as follows:

port.on('data', function (data) {
  console.log('Data: ' + data);
});