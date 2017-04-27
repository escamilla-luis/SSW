var util = require('util');
var SerialPort = require('serialport');

// Path to serialport may be different on different machines
var pathToPort = '/dev/tty.usbserial-DN01J57O';

// Refresh port every time we call writeToXbee(..)
var port = new SerialPort(pathToPort, {
    autoOpen: false,
    baudrate: 57600
});

// Set listener for port opening. We write to the xbee in callback.
port.on('open', function() {
    port.on('data', function (fromXbee) {
        console.log('Data received: ' + fromXbee);
    });
});

// We open the port, which will fire off our .on('open') callback
// function that writes to the xbee.
port.open(function (err) {   
    if (err) {
        console.log('Error opening port: ', err.message);
        console.log('Retrying to open port...');
    } else {
        console.log('Port opened');
    }
});

setTimeout(function() {
    
    console.log('Writing value');
    port.write('06020003');
//    port.write('06060104');
//    port.write('06110000');
//    lightShow();
}, 200);


function lightShow() {
    var i = 1;
    setInterval(function() {
        port.write('0602000' + i);
        i += 2;
        if (i > 9) { i = 1; }
    }, 100);
}