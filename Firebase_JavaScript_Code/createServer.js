var messenger = require('messenger');
var mysql = require('mysql');
var firebase = require('firebase');
var mysqlEvents = require('mysql-events');
var threads = require('threads');
var config = threads.config;
var spawn = threads.spawn;

// Initialize Firebase with our Spartan Superway database information
var config = {
	apiKey: "AIzaSyA4tiWKhfsGzygbRWgwfRb76LN4fg7gA5Q",
	authDomain: "spartan-superway.firebaseapp.com",
	databaseURL: "https://spartan-superway.firebaseio.com",
	storageBucket: "spartan-superway.appspot.com",
};

firebase.initializeApp(config);
var podSchedule = [];
var overflowSchedule = [];

//creates pod schedule array
createPodScheduleArray(numberOfRows());

// Setting up MySQL database for number of rows needed.
var conn = mysql.createConnection({
	host:'localhost',
	user:'timmahwork',
	password:'',
	database:'SSW'
})

// Firebase stuff
var database = firebase.database();

//creates MySQL database
createMySQLDatabase(numberOfRows());

fillIfAvailable('Frank');


// To check if the correct number of pods were inserted into the database
// con.query('SELECT * FROM pods', function (error, results, fields) {
// 	if (error) throw error;
// 	console.log('before select');
// 	for (var i = 0; i < results.length; i++) {
// 		console.log(results[i]);
// 	};
// });

// var i = 1;

// setInterval(function() {
	
// 	var thread = spawn('client.js');

// 	i = Math.floor(Math.random() * 10);

// 	console.log(i);

// 	thread
// 		.send({ do : i, conn : conn})
// 		.on('message', function(message) {
// 			//console.log('replied: ', message);
// 			thread.kill();
// 		});
	
// }, 1000);

//creates a new thread for work
function createNewJobThread(user) {

					//pull once
					// var user = database.ref('users').child(userId).once('value').then(function(snapshot) {
					// snapshot.child('status').val()
					// snapshot.child('status').set(100)	
					// })

					// push once
					// database.ref('users')child(userId).child('status').set(200)
	//some stored variables for user
	// var userTicket = database.ref('users').child(user).child('currentTicket');
	// var ticketStatus = userTicket.child('status');
	// var ticketTo = userTicket.child('to');
	// var ticketFrom = userTicket.child('from');
	// var ticketEta = userTicket.child('eta');

	//the node file that will be replicated for each thread needed
	var thread = spawn('client.js');

	//find which pod number the thread will use
	var podnumberInUse = podSchedule.index(user) + 1;
	
	thread
		.send({podnumber : podnumberInUse})
		.on('message', function(message) {
			console.log('replied: ', message);
			thread.kill();
		});
}

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

					// Handle ticket here
					var mutableUserRef = database.ref('users').child(userId);

					fillIfAvailable(userId);


					var mutableTicketRef = database.ref('users').child(userId).child('currentTicket');
					//mutableTicketRef.child('isNewTicket').set(false);

					// if (isNewTicket == true) {
					// 	console.log('Ticket is new!');
						
					// 	// Handle ticket here
					// 	var mutableTicketRef = database.ref('users').child(snapshot.key).child('currentTicket');
					// 	mutableTicketRef.child('isNewTicket').set(false);
						
					// 	// TEST CODE - remove later
					// 	assignTicketToClient(userId, count++);
					
					// 	// Do something relevant with userId...
					// 	var mutableEtaRef = mutableTicketRef.child('eta');
					// 	var mutableStatusRef = mutableTicketRef.child('status');
					// }
				});
			});
		});
}

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
			//create a stored userid value
			var useableUser = overflowSchedule[0];

			//add this user to pod schedule array and remove from overflow
			addUserToPodSchedule(useableUser);
			overflowSchedule.splice(0, 1);
			overflowSchedule.push(userID);

			//do work on user's current ticket
			createNewJobThread(useableUser);
		}

		//insert new user from the server
		else {

			//add user to pod schedule array
			addUserToPodSchedule(userID);

			//do work on user's current ticket
			createNewJobThread(userID);
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

//create the pod schedule array with the correct free values
function createPodScheduleArray(numberofFreeValues) {
	for (var i =  0; i < numberofFreeValues; i++) {
			podSchedule[i] = "free";
		};
}

//fills rows in MySQL table based on argument in command line
function numberOfRows() {

	//get argument from command line and make it an int
	var argumentValue = parseInt(process.argv[2]);

	//check if the argument is a number
	if (!isNaN(argumentValue)) {
		return argumentValue;
	}
	else {
		return 0;
	}
}

//creates the Database based on the input from command line
function createMySQLDatabase(numberOfRowsNeeded) {

	// Drop old Table
	conn.query('DROP TABLE pods', function (error, results, fields){
		if (error) throw error;
	});

	// Create new Table
	conn.query('CREATE TABLE pods (pod_num INT, station_from INT, station_to INT, status INT, time_left_station INT)', function (error, results, fields) {
		if (error) throw error;
	});

	// Insert values to represent the correct number of pods
	for (var i = 0; i < numberOfRowsNeeded; i++) {
		var podnumreal = i + 1;
		conn.query('INSERT INTO pods (pod_num) VALUES (' + podnumreal + ')', function (error, results, fields) {
			if (error) throw error;
		});
	};
}

//listenForTickets();

conn.end();