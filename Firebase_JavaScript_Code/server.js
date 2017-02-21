var messenger = require('messenger');
var mysql = require('mysql');
var firebase = require('firebase');
var mysqlEvents = require('mysql-events');

// Initialize Firebase with our Spartan Superway database information
var config = {
	apiKey: "AIzaSyA4tiWKhfsGzygbRWgwfRb76LN4fg7gA5Q",
	authDomain: "spartan-superway.firebaseapp.com",
	databaseURL: "https://spartan-superway.firebaseio.com",
	storageBucket: "spartan-superway.appspot.com",
};
firebase.initializeApp(config);

// MySQL database stuff
//var con = mysql.createConnection({
//	host:'localhost',
//	user:'root',
//	password:'password',
//	database:'SSW'
//})
//
//var dsn = {
//	host:     'localhost',
//	user:     'root',
//	password: 'password'
//};
//
//con.connect(function(err){
//	if(err) {
//		console.log('Error connecting to Database')
//		return;
//	}
//	console.log('Connection established...')
//})


// Server/Client code

var speaker1 = messenger.createSpeaker(8001);
var speaker2 = messenger.createSpeaker(8002);
var speaker3 = messenger.createSpeaker(8003);
var speaker4 = messenger.createSpeaker(8004);

var podSchedule = [-1, -1, -1, -1];

// This callback gets executed when the client sends the server a reply 
// letting it know that it's task is completed
var clientCallback = function (data) {
	
	console.log("Client replied!")
	console.log("podNumber: " + data.podNumber);
	console.log("podStatus: " + data.podStatus);
	
	// Update pod schedule
	podSchedule[data.podNumber] = data.podStatus;
};

// We can call this function here when there is a new ticket to deal
// with from Firebase. The eventListener on Firebase will provide us
// the userId. From there we can provide this function the available client #
function assignTicketToClient(firebaseUserId, clientNumber) {
	
	var data = {firebaseUserId: firebaseUserId}
	
	switch (clientNumber) {
		case 1:
			console.log("speaker1.request()");
			speaker1.request('assignTicket', data, clientCallback);
			break;
		case 2:
			speaker2.request('assignTicket', data, clientCallback);
			break;
		case 3:
			speaker3.request('assignTicket', data, clientCallback);
			break;
		case 4:
			speaker4.request('assignTicket', data, clientCallback);
			break;
		default:
			console.log('Error: Invalid client number specified');
	}	
}

// Demo code that will send each client 'userId' every 2 seconds.
//var userId = 1234;
//var podNumber = 1;
//setInterval(function() {
//	assignTicketToClient(userId, podNumber);
//	
//	userId = userId + 1;	
//	podNumber = podNumber + 1;
//	if (podNumber > 4) {
//		podNumber = 1;
//	}
//}, 2000);


var isClientBusy = false;
// Firebase stuff
var database = firebase.database();

// Function that sets listener on each user (individually)
function listenForTickets() {
	
	var usersRef = database.ref('users');
	usersRef.once('value')
		.then(function(users) {
			users.forEach(function(user) {
				usersRef.child(user.key).on('value', function(snapshot) {
					
					// Only perform something relevant on ticket if 'isTicketAlive' == true
					var userId = snapshot.key;
					console.log('userId: ' + userId);

					var isNewTicket = snapshot.child('currentTicket').child('isNewTicket').val();
					if (isNewTicket == true) {
						console.log('Ticket is new!');
						
						// Handle ticket here
						var mutableTicketRef = database.ref('users').child(snapshot.key).child('currentTicket');
						mutableTicketRef.child('isNewTicket').set(false);
						
						// TEST - Send request to client 1 if it's not busy
						if (!isClientBusy) {
							assignTicketToClient(userId, 1);
						}
					
						// Do something relevant with userId...

						var mutableEtaRef = mutableTicketRef.child('eta');
						var mutableStatusRef = mutableTicketRef.child('status');
					} 
					
				});
			});
		});
}

listenForTickets();

