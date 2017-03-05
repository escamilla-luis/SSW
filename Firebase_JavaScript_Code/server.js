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
		case 4:
			speaker4.request('assignTicket', data, clientCallback);
			break;
		default:
			console.log('Error: Invalid client number specified');
	}	
}

// Demo code that will send each client 'userId' every 2 seconds.
var userId = 1234;
var podNumber = 1;
setInterval(function() {
	assignTicketToClient(userId, podNumber);
	
	userId = userId + 1;	
	podNumber = podNumber + 1;
	if (podNumber > 4) {
		podNumber = 1;
	}
}, 2000);

//a function thats takes a userID and their current ticket and associates them with an available pod
//function 


//a function that checks for an available pod
function checkForAvailablePod() {
	for (var i = podSchedule.length - 1; i >= 0; i--) {
		if (podSchedule[i] < 0) {
			return true;
		}
		else {
			return false;
		}
	};
}

//a function that fill users based on available pod status
function fillIfAvailable(userID) {
	
	//check if its available
	if (checkForAvailablePod) {

		//check overflow for users waiting for a ride
		if (overflowSchedule.length != 0) {
			addUserToPodSchedule[overflowSchedule[0]];
			overflowSchedule.splice(0, 1);
			overflowSchedule.push(userID);
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
	for (var i = podSchedule.length - 1; i >= 0; i--) {
		if(podSchedule[i] < 0) {
			podSchedule[i] = userID;
			break;
		}
	};
}

//remove a user from the pod schedule when done
function removeUserFromPodSchedule(userID){
	for (var i = podSchedule.length - 1; i >= 0; i--) {
		if (podSchedule[i] == userID) {
			podSchedule[i] = -1;
			break;
		}
	};
}

// Gets an array of current tickets from Firebase
function getCurrentTickets() {
	var users = [];
	var tickets = [];
	
	usersStart = firebase.database().ref('users');

	//first start by getting the array of users
	usersStart.on('value', function(snapshot) {

		for (var i = Object.keys(snapshot.val()).length - 1; i >= 0; i--) {
			
			users.push(Object.keys(snapshot.val())[i]);
			//console.log(Object.keys(snapshot.val())[i]);
			
		};

		//then get the array of tickets
		for (var i = users.length - 1; i >= 0; i--) {
			var ticketStart = firebase.database().ref('users/' + users[i]+'/currentTicket');

			ticketStart.on('value', function(snapshot) {
				//console.log(snapshot.val());
				tickets.push(snapshot.val());
			});
		};

		console.log(tickets);
		return tickets;
	});
}

