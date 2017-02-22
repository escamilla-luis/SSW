var messenger = require('messenger');
var firebase = require('firebase');
var mysql = require('mysql');

// Initialize Firebase with our Spartan Superway database information
var config = {
    apiKey: "AIzaSyA4tiWKhfsGzygbRWgwfRb76LN4fg7gA5Q",
    authDomain: "spartan-superway.firebaseapp.com",
    databaseURL: "https://spartan-superway.firebaseio.com",
    storageBucket: "spartan-superway.appspot.com",
};

firebase.initializeApp(config);

var PORT = 8003;
var POD_NUM = 3;
var client = messenger.createListener(PORT);

var database = firebase.database();
client.on('assignTicket', function(message, data) {
    
    var firebaseUserId = data.firebaseUserId;
    console.log("userId passed from server: " + firebaseUserId);
    
    var onValueChange = database.ref('users').child(firebaseUserId).on('value', function(snapshot) {
        
        var mutableTicketRef = database.ref("users").child(snapshot.key).child("currentTicket");
        var mutableEtaRef = mutableTicketRef.child("eta");   
        var mutableStatusRef = mutableTicketRef.child("status");

        var statusCode = snapshot.child("currentTicket").child("status").val();
        if (statusCode === 100 || statusCode === 300) {
            // If timer is already on, forget it

            var timerOn = snapshot.child("currentTicket").child("timerOn").val();
            console.log("timerOn = " + timerOn);
            if (timerOn == true) {
                console.log("timer is already on");
            } else {
                console.log('timer == false');
                mutableTicketRef.child("timerOn").set(true);
                var etaValue = 5;
                var timer = setInterval(function() {
                    console.log("starting timer");
                    // Set eta from mutable ref
                    if (etaValue == 0) {
                        
                        clearInterval(timer);
                        if (statusCode == 100) {
                            // Set status from mutable ref
                            mutableStatusRef.set(200);
                        } else {
                            mutableStatusRef.set(400);
                        }
                        mutableTicketRef.child("timerOn").set(false);
                        
                        // Reply to server letting it know that task is complete
                        // TODO: Uncomment and fix later
    //                    console.log("Work done! Replying to server w/ updated status");
    //                    message.reply({podNumber: POD_NUM, podStatus: "free"});
                        
                        return;
                    }
                    console.log("setting eta value");
                    mutableEtaRef.set(etaValue);
                    etaValue = etaValue - 1;
                }, 1000);
            }
        }
        
        // 
        if (statusCode == 900) {
            console.log('statusCode = 900');
            database.ref('users').child(firebaseUserId).off('value', onValueChange);
        }
    });


});

