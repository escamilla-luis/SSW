var firebase = require('firebase');
var config = {
    apiKey: "AIzaSyA4tiWKhfsGzygbRWgwfRb76LN4fg7gA5Q",
    authDomain: "spartan-superway.firebaseapp.com",
    databaseURL: "https://spartan-superway.firebaseio.com",
    storageBucket: "spartan-superway.appspot.com",
};
firebase.initializeApp(config);

var database = firebase.database();
var usersRef = database.ref('users');

// Test file used to push data to Firebase
usersRef.child('Qvn71YOfXzMdmASoievQBboMEvI3')
        .child('currentTicket')
        .child('from')
        .set(1);

usersRef.child('Qvn71YOfXzMdmASoievQBboMEvI3')
        .child('currentTicket')
        .child('to')
        .set(3);
                    
usersRef.child('Qvn71YOfXzMdmASoievQBboMEvI3')
        .child('currentTicket')
        .child('isNewTicket')
        .set(true);
        
usersRef.child('Qvn71YOfXzMdmASoievQBboMEvI3')
        .child('currentTicket')
        .child('status')
        .set(100);
