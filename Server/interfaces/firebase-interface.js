// Configure Firebase
var firebase = require('firebase');
var config = {
    apiKey: "AIzaSyA4tiWKhfsGzygbRWgwfRb76LN4fg7gA5Q",
    authDomain: "spartan-superway.firebaseapp.com",
    databaseURL: "https://spartan-superway.firebaseio.com",
    storageBucket: "spartan-superway.appspot.com",
};
firebase.initializeApp(config);
// Reference to our database & users
var database = firebase.database();
var usersRef = database.ref('users');

/*** --= INTERNAL FUNCTIONS (Do NOT call outside of this interface) =-- ***/
// Used internally by interface, not to be called outside of this file

/** Returns a REFERENCE to the currentTicket of a particular user
    -userId: String of the user's id in Firebase
**/
function getCurrentTicketFromUser(userId) {
    return usersRef.child(userId).child('currentTicket');
}

/** Returns a snapshot of the reference in firebase; used for querying data
    -ref: Reference to node in Firebase
    -callback: Function to call after retrieving the snapshot
**/
function getSnapshot(ref, callback) {
    ref.once('value')
       .then(function(snapshot) {
            callback(snapshot);
        });
}

/** Parses ticket data into a JSON object, used internally by getAllCurrentTicketTicketsAsJsonArray(..)
**/
function getCurrentTicketJson(userId, onDataReceived) {
    var ticketRef = getCurrentTicketFromUser(userId);
    ticketRef.once('value')
             .then(function(snapshot) {
                var eta = snapshot.child('eta').val();
                var from = snapshot.child('from').val();
                var to = snapshot.child('to').val();
                var status = snapshot.child('status').val();
                var isNewTicket = snapshot.child('isNewTicket').val();
                var timerOn = snapshot.child('timerOn').val();
                
                // Return JSON data in callback
                onDataReceived({
                    eta: eta,
                    from: from,
                    to: to,
                    status: status,
                    isNewTicket: isNewTicket,
                    timerOn: timerOn
                });
            });
}

exports.getUserTicketRef = function getUserTicketRef(userId) {
    return database.ref('users').child(userId).child('currentTicket');
}


/*** --= LISTENER FUNCTIONS =-- ***/

/** Sets a listener on a particular user's current ticket in Firebase
    -userId: String of user's id in Firebase
    -onDataChanged: Callback function that executes every time there is a change in the user's ticket data.
                    Parameter passed to this function is a 'snapshot' object that represents the ticket
**/
exports.setListenerForTicket = function setListenerForTicket(userId, onDataChanged) {
    var ticketRef = getCurrentTicketFromUser(userId);
    ticketRef.on('value', onDataChanged);
}

/** Sets a listener on ALL current ticket in Firebase
    -onDataChanged: Callback function that executes every time there is a change in a user's ticket data.
                    Parameter passed to this function is a 'snapshot' object that represents the ticket
**/
exports.setListenerForAllCurrentTickets = function setListenerForAllCurrentTickets(onDataChanged) {
    var usersRef = database.ref('users');
        usersRef.once('value')
            .then(function(users) {
                users.forEach(function(user) {
                        usersRef.child(user.key).on('value', onDataChanged);
                });
            });
}

/*** --= QUERY FUNCTIONS =-- ***/

/** Parses the ticket into a JSON object that is returned in a callback function
    -userId: String of the user's id in Firebase
    -onDataReceived: callback function which we pass the JSON ticket to
**/
exports.getCurrentTicketJson = function getCurrentTicketJson(userId, onDataReceived) {
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
                    timerOn: timerOn
                });
            });
}

/** Parses tickets into an array of JSON objects
    -onRetrieve: the callback function which we will pass the JSON array to
**/
exports.getAllCurrentTicketsAsJsonArray = function getAllCurrentTicketsAsJsonArray(onRetrieve) {
    var currentTicketsJsonArray = [];
    usersRef.once('value')
            .then(function (usersSnapshot) {
                
                // Iterate through each user and add their currentTicket Id's
                var userCount = 0;
                usersSnapshot.forEach(function(user) {
                    userCount = userCount + 1;
                });
                
                // Dirty way to ensure each ticket is pushed as we go through multiple
                // (network) callback functions to add json to the array
                var count = 0;
                usersSnapshot.forEach(function(user) {
                    var userId = user.key;
                    getCurrentTicketJson(userId, function(ticketData) {
                        currentTicketsJsonArray.push(ticketData);
                        count = count + 1;
                        if (count == userCount) {
                            onRetrieve(currentTicketsJsonArray);
                            return;
                        }
                    });
                });
            });
}

/** Returns a list of Firebase REFERENCES to all current tickets
    -onReceive: The function to call after retrieving all the tickets
**/
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

/** Sets the eta time for a ticket under a particular user
    -userId: String of user's id in Firebase
	-time: Integer of time left
**/
exports.setEta = function setEta(userId, time) {
    var ticketRef = getCurrentTicketFromUser(userId);
    ticketRef.child('eta').set(time);
}

/** Sets the timerOn value for a user's current ticket data
    -userId: String of user's id in Firebase
    -statusCode: Integer that specifies the status code
**/
exports.setStatus = function setStatus(userId, statusCode) {
    var ticketRef = getCurrentTicketFromUser(userId);
    ticketRef.child('status').set(statusCode);
}

/** Sets the timerOn value for a user's current ticket data
    -userId: String of user's id in Firebase
    -timerOn: Boolean that specifies of the timer is on or not
**/
exports.setTimerOn = function setTimerOn(userId, timerOn) {
    var ticketRef = getCurrentTicketFromUser(userId);
    ticketRef.child('timerOn').set(timerOn);
}

/** Sets the isNewTicket value for a user's current ticket data
    -userId: String of the user's id in Firebase
    -isNewTicket: Boolean value that specifies if isNewTicket is true or false
**/
exports.setIsNewTicket = function setIsNewTicket(userId, isNewTicket) {
    var ticketRef = getCurrentTicketFromUser(userId);
    ticketRef.child('isNewTicket').set(isNewTicket);
}