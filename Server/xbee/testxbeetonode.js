var util = require('util');
var SerialPort = require('serialport');

// currently not in use
// var xbee_api = require('xbee-api');
// var C = xbee_api.constants;
// var xbeeAPI = new xbee_api.XBeeAPI({
//     api_mode: 1
// });

// For the computer's specific port
var serialPortId = '/dev/tty.usbserial-DN01J8BJ';

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
        console.log('Error opening port: ', err.message);
      } else {
        console.log('Port opened');
      }
    });
}

//setState();

getState();

function getState() {

    setTimeout(function() {
      initOpen();
      writeData('31');
    }, 10);

    //setTimeout(getData, 10);
}

// Write data to the port to be caught
function writeData(toXbee) {
    console.log('write');
    
    port.on('open', function() {
        console.log("open");
        // listener
        port.on('data', function (fromXbee) {
          console.log('Data received: ' + fromXbee);
          console.log('close');
          port.close();
          // line = Buffer.from(data);
          //console.log(new Buffer(data, 'hex')[0]);
        });
        
        port.write(toXbee + '');
    })
}

function setState() {
    initOpen();

    setTimeout(function() {
      writeData('33');
    }, 10);
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

