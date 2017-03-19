var util = require('util');
var SerialPort = require('serialport');

// currently not in use
// var xbee_api = require('xbee-api');
// var C = xbee_api.constants;
// var xbeeAPI = new xbee_api.XBeeAPI({
//     api_mode: 1
// });

// For the computer's specific port
var serialPortId = '/dev/tty.usbserial-DN01J57O';

// Create the port that we will be using. 
// Note the baudrate and how it needs to be forceably opened.
var port = new SerialPort(serialPortId, {
    autoOpen: false,
    baudrate: 57600,
    //parser: xbeeAPI.rawParser()
});

function initOpen() {

    // Open port once
    port.open(function (err) {
      if (err) {
        return console.log('Error opening port: ', err.message);
      } 
      
    });

}

//setState();

getState();

function getState() {
    initOpen();

    setTimeout(writeData('31'), 10);

    //setTimeout(getData, 10);
}

// Write data to the port to be caught
function writeData(toXbee) {

    port.on('open', function() {
        console.log("open");
        port.write(toXbee + '');
        
        // listener
        port.on('data', function (fromXbee) {
          console.log('Data received: ' + fromXbee);
          console.log('close');
          port.close();
          // line = Buffer.from(data);
          //console.log(new Buffer(data, 'hex')[0]);
        });
    })
}

function setState() {
    initOpen();

    setTimeout(writeData('33'), 10);
}

// the open event will always be emitted
// port.on('open', function() {
//   // open logic
// });

//You can get updates of new data from the Serial Port as follows:



//writeData('31');

//setTimeout(getData, 10);

// Catch data from xbee
function getData() {
    port.on('data', function (fromXbee) {
        console.log('Data: ' + fromXbee);
        port.close();
        // line = Buffer.from(data);
        //console.log(new Buffer(data, 'hex')[0]);
        
    });
}

function openPort() {
    var serialPortId = '/dev/tty.usbserial-DN01J57O';

    var port = new SerialPort(serialPortId, {
        autoOpen: false,
        baudrate: 57600,
        //parser: xbeeAPI.rawParser()
    });

    //var parser = port.pipe(readline({delimiter: '\n'}));

    port.open(function (err) {
      if (err) {
        return console.log('Error opening port: ', err.message);
      }
    });
}

function getLocation(data) {

}

