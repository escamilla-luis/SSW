var utils = require('./utils.js');
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

var podSchedule = ['free', 'free', 'free'];
var overflowSchedule = [];

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
		default:
			console.log('Error: Invalid client number specified');
	}	
}

// Firebase stuff
var database = firebase.database();

// Function that sets listener on each user (individually)
function listenForTickets() {


	// TEST for demo - removelater
	var count = 1;	
	
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
						
						// TEST CODE - remove later
						console.log('count: ' + count);
						assignTicketToClient(userId, count++);
						if (count > 3) {
							count = 1;
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

//a function that checks for an available pod
function checkForAvailablePod() {

	var thecheck = false;

	for (var i = 0; i < podSchedule.length; i++) {
		if (podSchedule[i] == 'free') {
			thecheck = true;
			break;
		}
	};

	return thecheck;

}


//a function that fill users based on available pod status
function fillIfAvailable(userID) {
	
	//check if its available
	if (checkForAvailablePod()) {

		//check overflow for users waiting for a ride
		if (overflowSchedule.length != 0) {
			console.log('adding from overflow')
			addUserToPodSchedule(overflowSchedule[0]);
			overflowSchedule.splice(0, 1);
		}

		//insert new user from the server
		else {
			addUserToPodSchedule(userID);
		}
	}

	//add a user to the overflow if the main queue is full
	else {
		overflowSchedule.push(userID);
	}
}

//a function that adds a user to the schedule
function addUserToPodSchedule(userID){
	for (var i = 0; i < podSchedule.length; i++) {
		if(podSchedule[i] == 'free') {
			podSchedule[i] = userID;
			i = 5;
		}
	};
}

//remove a user from the pod schedule when done
function removeUserFromPodSchedule(userID){
	for (var i = 0; i < podSchedule.length; i++) {
		if (podSchedule[i] == userID) {
			podSchedule[i] = 'free';
			i = 5;
		}
	};
}

utils.getCurrentTickets;