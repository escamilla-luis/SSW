var util = require('util');
var SerialPort = require('serialport');

// currently not in use
// var xbee_api = require('xbee-api');
// var C = xbee_api.constants;
// var xbeeAPI = new xbee_api.XBeeAPI({
//     api_mode: 1
// });

// For the computer's specific port
var serialPortId = '/dev/ttyUSB3';

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

//etState();

function getState() {
    initOpen();

    setTimeout(writeData('4'), 10);

    //setTimeout(getData, 10);
}

// Write data to the port to be caught
function writeData(toXbee) {
    port.on('open', function() {
        port.write(toXbee + '');
            port.on('data', function (fromXbee) {
            console.log('Data: ' + fromXbee);
            port.close();
            // line = Buffer.from(data);
            //console.log(new Buffer(data, 'hex')[0]);
            
        });
    })
}

function setState() {
    initOpen();

    setTimeout(writeData('32'), 10);
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
    var serialPortId = '/dev/ttyUSB3';

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

// setTimeout( function () {

// }, 1000);
var util = require('util');
var SerialPort = require('serialport');

// currently not in use
// var xbee_api = require('xbee-api');
// var C = xbee_api.constants;
// var xbeeAPI = new xbee_api.XBeeAPI({
//     api_mode: 1
// });

// For the computer's specific port
var serialPortId = '/dev/ttyUSB3';

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

  // write errors will be emitted on the port since there is no callback to write
  port.write("32");
});

// the open event will always be emitted
port.on('open', function() {
  // open logic
});

//You can get updates of new data from the Serial Port as follows:
var line = 'none yet';

port.on('data', function (data) {

  	console.log('Data: ' + data);
  	// line = Buffer.from(data);
  	//console.log(new Buffer(data, 'hex')[0]);
  	
});

setInterval(function () {
	//console.log(line);
}, 1000);

// setTimeout( function () {

// }, 1000);