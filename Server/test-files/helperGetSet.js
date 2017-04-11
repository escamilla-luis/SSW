// var util = require('util');
// var SerialPort = require('serialport');
// var xbee_api = require('xbee-api');
// var C = xbee_api.constants;


// // Path to serialport may be different on different machines
// var pathToPort = '/dev/ttyUSB1';

// // Writes a value to an xbee
// // -value: value to write
// // -callback: optional callback if we want to handle data returned by an xbee
// exports.writeToXbee = function writeToXbee(value, callback) {

//     // Refresh port every time we call writeToXbee(..)
//     var port = new SerialPort(pathToPort, {
//         autoOpen: false,
//         baudrate: 57600
//     });

//     // Set listener for port opening. We write to the xbee in callback.
//     port.on('open', function() {
//         port.on('data', function (fromXbee) {


//             //console.log('Data received: ' + fromXbee);
//             //If the user passed a callback function, we run it
//             if (callback) {
//                 // Handle data from xbee
//                 console.log('Data received: ' + fromXbee);
//                 callback(fromXbee);
//             }
            
//             port.close();
//         });
        
//         console.log('Writing value to xbee');
//         port.write(value);
//     });
    
//     // We open the port, which will fire off our .on('open') callback
//     // function that writes to the xbee.
//     port.open(function (err) {
//         if (err) {
//             console.log('Error opening port: ', err.message);
//             console.log('Retrying to open port...');
//             // If there is a problem opening the port, we retry opening
//             // in 500 ms. In most cases, a port will not open because
//             // it is currently opened by another file.
//             setTimeout(function() {
//                 writeToXbee(value);
//             }, 1000);
//         } else {
//             console.log('Port opened');
//         }
//     });
// }




var util = require('util');
var SerialPort = require('serialport');

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



function initOpen() {

    // Open port once
    port.open(function (err) {
      if (err) {
        return console.log('Error opening port: ', err.message);
      } 
      
    });

}



// Write data to the port to be caught
function writeData(toXbee) {
    port.on('open', function() {
    	//console.log(toXbee + '');
        port.write(toXbee + '');
        setTimeout(collectData, 100);
    });
}

// exports.initOpen = initOpen;
// exports.writeData = writeData;


function collectData() {

	port.on('data', function (fromXbee) {

        console.log('Data: ' + fromXbee);

        port.close();
        // line = Buffer.from(data);
        //console.log(new Buffer(data, 'hex')[0]);
        
    });
}

function manipData(theData) {
    initOpen();

    setTimeout(writeData(theData + ''), 10);

    //setTimeout(getData, 10);
}

exports.manipData = manipData;

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
