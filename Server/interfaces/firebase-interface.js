// Configure Firebase
var firebase = require('firebase');
var config = {
    apiKey: "AIzaSyA4tiWKhfsGzygbRWgwfRb76LN4fg7gA5Q",
    authDomain: "spartan-superway.firebaseapp.com",
    databaseURL: "https://spartan-superway.firebaseio.com",
    storageBucket: "spartan-superway.appspot.com",
};
// Initialize Firebase with our configuration
firebase.initializeApp(config);
// Reference to our database & users
var database = firebase.database();
var usersRef = database.ref('users');

/*** --= INTERNAL FUNCTIONS =-- ***/
// Used interally by interface, not to be called outside of this fuile

// Returns a REFERENCE to the currentTicket of a particular user
// -userId: String of the user's id in Firebase
function getCurrentTicketFromUser(userId) {
    return usersRef.child(userId).child('currentTicket');
}

// Returns a snapshot of the reference in firebase; used for querying data
// -ref: Reference to node in Firebase
// -callback: Function to call after retrieving the snapshot
function getSnapshot(ref, callback) {
    ref.once('value')
       .then(function(snapshot) {
            callback(snapshot);
        });
}

/*** --= QUERY FUNCTIONS =-- ***/

// Parses the ticket into a JSON object that is returned in a callback function
// -userId: String of the user's id in Firebase
// -onDataReceived: callback function which we pass our JSON ticket to
exports.getCurrentTicketJSON = function getCurrentTicketJSON(userId, onDataReceived) {
    var ticketRef = getCurrentTicketFromUser(userId);
    ticketRef.once('value')
             .then(function(snapshot) {
                var eta = snapshot.child('eta').val();
                var from = snapshot.child('from').val();
                var to = snapshot.child('to').val();
                var status = snapshot.child('status').val();
                var isNewTicket = snapshot.child('isNewTicket').val();
                var timerOn = snapshot.child('timerOn').val();
                
                onDataReceived({
                    eta: eta,
                    from: from,
                    to: to,
                    status: status,
                    isNewTicket: isNewTicket,
                    timerOn: timeOn
                });
            });
}

// Returns a list of Firebase REFERENCES to all current tickets
// -onReceive: The function to call after retrieving all the tickets
exports.getAllCurrentTicketsRefs = function getAllCurrentTicketsRefs(onRetrieve) {
    var currentTicketReferences = [];
    usersRef.once('value')
            .then(function (users) {
                // Iterate through each user and add their currentTicket Id's
                users.forEach(function(user) {
                    var userId = user.key;
                    var ticketRef = usersRef.child(userId).child('currentTicket');
                    currentTicketReferences.push(ticketRef);
                });
                
                onRetrieve(currentTicketReferences);
            });
}


/*** --= STORE FUNCTIONS =-- ***/

// Sets the eta time for a ticket under a particular user
// -userId: String of user's id in Firebase
// -time: Integer of time left
exports.setEta = function setEta(userId, time) {
    var ticketRef = getCurrentTicketFromUser(userId);
    ticketRef.child('eta').set(time);
}

// Sets the status code for a ticket under a particular user
// -userId: String of user's id in Firebase
// -statusCode: Integer that specifies the status code
exports.setStatus = function setStatus(userId, statusCode) {
    var ticketRef = getCurrentTicketFromUser(userId);
    ticketRef.child('status').set(statusCode);
}
