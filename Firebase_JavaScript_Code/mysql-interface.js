// Configure MySQL
var mysql = require('mysql');
var mysqlEvents = require('mysql-events');

//Create Connection to MySQL Server
var conn = mysql.createConnection({
    host:'localhost',
    user:'timmahwork',
    password:'',
    database:'SSW'
})

/*** --= QUERY FUNCTIONS =-- ***/

// Returns a reference to the currentTicket of a particular user
// -userId: String of the user's id in MySQL
function getCurrentTicketFromUser(userId) {
    return usersRef.child(userId).child('currentTicket');
}

// Returns a snapshot of the reference in firebase - used for querying data
// -ref: Reference to node in MySQL
// -func: Function to call after retrieving the snapshot
function getSnapshot(ref, func) {
    ref.once('value')
       .then(function(snapshot) {
            func(snapshot);
        });
}

// Returns a list of REFERENCES to all current tickets
// -func: The function to call after retrieving all the tickets
function getAllCurrentTickets(func) {
    var currentTicketReferences = [];
    usersRef.once('value')
            .then(function (users) {
                // Iterate through each user and add their currentTicket Id's

                users.forEach(function(user) {
                    var userId = user.key;
                    var ticketRef = usersRef.child(userId).child('currentTicket');
                    currentTicketReferences.push(ticketRef);
                });
                
                func(currentTicketReferences);
            });
}



/*** --= STORE FUNCTIONS =-- ***/

// Sets the eta time for a ticket under a particular user
// -userId: String of user's id in MySQL
// -time: Integer of time left
function setEta(userId, time) {
    var ticketRef = getCurrentTicketFromUser(userId);
    ticketRef.child('eta').set(time);
}

// Sets the status code for a ticket under a particular user
// -userId: String of user's id in MySQL
// -statusCode: Integer that specifies the status code
function setStatus(userId, statusCode) {
    var ticketRef = getCurrentTicketFromUser(userId);
    ticketRef.child('status').set(statusCode);
}


/*** --= EXAMPLE CODE =-- ***/
// Remember, these function calls are ASYNCHRONOUS, which may affect how you write your code

// Get a list of references that is returned as an argument of our callback function
getAllCurrentTickets(function(listOfRef) {
    // Iterate through each reference
    listOfRef.forEach(function(ref) {
        // Get the snapshot of the returns returned as an argument of our callback function
        getSnapshot(ref, function(snapshot) {
            // Log the value after retrieving the snapshot in the callback function
            console.log('status: ' + snapshot.child('status').val());
        });
    });
});
