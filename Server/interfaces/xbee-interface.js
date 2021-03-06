var util = require('util');
var SerialPort = require('serialport');

// Path to serialport may be different on different machines
var pathToPort = '/dev/tty.usbserial-DN01J8BJ';

// Writes a value to an xbee
// -value: value to write
// -callback: optional callback if we want to handle data returned by an xbee
function writeToXbee(value, callback) {

    // Refresh port every time we call writeToXbee(..)
    var port = new SerialPort(pathToPort, {
        autoOpen: false,
        baudrate: 57600
    });

    // Set listener for port opening. We write to the xbee in callback.
    port.on('open', function() {
        port.on('data', function (fromXbee) {

            // If the user passed a callback function, we run it
            if (callback) {
                // Handle data from xbee
                console.log('Data received: ' + fromXbee);
                callback(fromXbee);
            }
            
            port.close();
        });
        
        console.log('Writing value to xbee');
        port.write(value);
    });
    
    // We open the port, which will fire off our .on('open') callback
    // function that writes to the xbee.
    port.open(function (err) {
        if (err) {
            console.log('Error opening port: ', err.message);
            console.log('Retrying to open port...');
            // If there is a problem opening the port, we retry opening
            // in 500 ms. In most cases, a port will not open because
            // it is currently opened by another file.
            setTimeout(function() {
                writeToXbee(value);
            }, 1000);
        } else {
            console.log('Port opened');
        }
    });
}