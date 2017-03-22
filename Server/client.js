//var messenger = require('messenger');
var mysql = require('./interfaces/mysql-interface.js');
var firebase = require('./interfaces/firebase-interface.js');
var messenger = require('messenger');
var xbee = require('./interfaces/xbee-interface.js');
var SerialPort = require('serialport');

module.exports = function(input, done) {
    var pod_num = input.pod_num;
    var user_id = input.uid;
    var portNum = input.portNum;
    
    console.log('Creating listener');
    var listener = messenger.createListener(portNum);
    listener.on('request', function(message, data) {
        console.log('Got something from server');
        console.log(('str: ' + data.str));
        
        message.reply({str: "Client"});
    });
    
    console.log('kill me');
    done({podNum: pod_num, killThread: true});

//    firebase.getCurrentTicketJson(user_id, function(ticketdata) {
//        status = ticketdata.status;
//        console.log(status);
//    });
    
    
//    setTimeout(function() {
//        console.log('xbee.write');
//        console.log(input.pod_num + " " + input.uid);
//        done('DOne! ');
//    }, 100);

};

