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

//test array
var testUsersArray = ['this','that','then','there','thy','other','names']

//important arrays
var podSchedule = [];
var overflowSchedule = [];

//calculates the number of "free" values in podSchedule based on argument value
numberOfFreeValues();

setInterval(function() {
	
	if (!isNaN(parseInt(process.argv[2]))) {

		var checkforuseraddition = Math.floor(Math.random() * 6);
		if (checkforuseraddition < 7) {
			//use user
			fillIfAvailable(testUsersArray[checkforuseraddition]);
		}
		else {
			//dont use anything
		}

		console.log('available pod check:' + checkForAvailablePod());
		console.log('podSchedule:' + podSchedule);
		console.log('overflowSchedule:' + overflowSchedule);
		console.log();


		var randomAddFromOverflow = Math.floor(Math.random() * 6);
		var randomRemove = Math.floor(Math.random() * 3);

		if (overflowSchedule.length > randomAddFromOverflow) {
			console.log('remove this user for overflow:' + podSchedule[randomRemove])
			console.log();
			removeUserFromPodSchedule(podSchedule[randomRemove]);
		}
	}

	else if (process.argv[2] == null) {
		console.log("There is no number passed! How many pods are on the track?");
	}

	else {
		console.log("This is not a number! Please post how many pods you want on the track.");
	}
	
}, 2000);

//a function that checks for an available pod
function checkForAvailablePod() {

	var thecheck = false;

	for (var i = 0; i < podSchedule.length; i++) {
		if (podSchedule[i] == 'free') {
			thecheck = true;
			i = podSchedule.length
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
	for (var i = 0; i < podSchedule.length; i++) {
		if(podSchedule[i] == 'free') {
			podSchedule[i] = userID;
			i = podSchedule.length;
		}
	};
}

//remove a user from the pod schedule when done
function removeUserFromPodSchedule(userID){
	for (var i = 0; i < podSchedule.length; i++) {
		if (podSchedule[i] == userID) {
			podSchedule[i] = 'free';
			i = podSchedule.length;
		}
	};
}

//fills podSchedule array with "free" values based on argument in command line
function numberOfFreeValues() {

	//get argument from command line and make it an int
	var argumentValue = parseInt(process.argv[2]);

	//check if the argument is a number
	if (!isNaN(argumentValue)) {

		for (var i =  0; i < argumentValue; i++) {
			podSchedule[i] = "free";
		};

		return argumentValue;
	}
	else {
		return 0;
	}
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