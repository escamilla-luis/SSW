var util = require('util');
var SerialPort = require('serialport');
var xbee_api = require('xbee-api');

// xbee-api stuff
var C = xbee_api.constants;
var xbeeAPI = new xbee_api.XBeeAPI({
    api_mode: 1,
    module: 'Any',
    raw_frames: true
});


// End-Device should be master
// Coordinator should be worker
var serial_xbee_master = new SerialPort('/dev/tty.usbserial-DN01IYP9', {
    baudrate: 57600,
    parser: xbeeAPI.rawParser()
});

serial_xbee_master.on('data', function(data) {
    console.log('xbee data received:', data.type);
});

// 00 13 A2 00 41 48 C3 92
// 0013a2004148c392
serial_xbee_master.on('open', function(data) {
    console.log('opened!');
    var data = {
        type: 0x09, // xbee_api.constants.FRAME_TYPE.AT_COMMAND_QUEUE_PARAMETER_VALUE
        id: 0x01, // optional, nextFrameId() is called per default
        command: "AT",
        commandParameter: "Hello, World! SPARTAN SUPERWAY"
    }
    
    serial_xbee_master.write(xbeeAPI.buildFrame(data), function(err, res) {
        if (err) {
            console.log('err');
        } else {
            console.log('written bytes: ' + util.inspect(res));
        }
    });
    
});

// All frames parsed by the XBee will be emitted here
xbeeAPI.on('frame_object', function(frame) {
    console.log('OBJ> ' + util.inspect(frame));
});

// Something we might receive from an XBee...
var raw_frame = new Buffer([
    0x7E, 0x00, 0x13, 0x97, 0x55, 0x00, 0x13, 0xA2, 0x00, 0x40, 0x52, 0x2B,
    0xAA, 0x7D, 0x84, 0x53, 0x4C, 0x00, 0x40, 0x52, 0x2B, 0xAA, 0xF0
]);

//console.log(xbeeAPI.parseFrame(raw_frame));

var message = 'Num: ';
var counter = 1;
function write() {
    
    serial_xbee_master.open(function(err) {
        
        // TODO: Try writing this data to xbee
        var data = {
            type: 0x09, // xbee_api.constants.FRAME_TYPE.AT_COMMAND_QUEUE_PARAMETER_VALUE
            id: 0x01, // optional, nextFrameId() is called per default
            command: "AT",
            commandParameter: "Hello, World! SPARTAN SUPERWAY"
        }
        
        console.log('Writing serial data: ' + counter++);
        serial_xbee_master.write(counter, function(err, res) {
            if (err) {
                console.log(err);
            }
            serial_xbee_master.close();
        });
    });
}

// setTimeout(write, 50); // Wait 50 ms for initialization and for port to open
// setInterval(write, 1000) // Write data every 1 second