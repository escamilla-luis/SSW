var threads = require('threads');
var config = threads.config;
var spawn = threads.spawn;
var messenger = require('messenger');
var mysql = require('mysql');
var firebase = require('./interfaces/firebase-interface.js');
var mysqlEvents = require('mysql-events');
var xbee = require('./interfaces/xbee-interface.js');
var SerialPort = require('serialport');

// Server/Client code
var podSchedule = ['free', 'free', 'free', 'free'];
var overflowQueue = []; // .push() to enqueue, .shift() to dequeue

// This callback gets executed when the client sends the server a reply 
// letting it know that it's task is completed
var clientCallback = function (data) {
	
	console.log("Client replied!")
	console.log("podNumber: " + data.podNumber);
	console.log("podStatus: " + data.podStatus);
	
	// Update pod schedule
	podSchedule[data.podNumber] = data.podStatus;
};

function spawnClientThread(pod_num, user_id) {
	var thread = spawn('client.js');
	thread
		.send({pod_num: pod_num, uid: user_id})
		.on('message', function(message) {
				console.log(message + 'Pod completed Schedule');
	});
}


firebase.setListenerForAllCurrentTickets(function(snapshot) {
		// snapshot = snapshot of ticket
		var userId = snapshot.key;
		var firstName = snapshot.child('firstName').val();
		var lastName = snapshot.child('lastName').val();
		console.log('userId: %s, firstName: %s, lastName: %s', userId, firstName, lastName);

		// Only assign ticket to client-thread if 'isTicketAlive' == true
		// Worker threads will fire off listeners as they make changes to the ticket data,
		// we use isTicketAlive to negate the server handling the same ticket over and over
		var isNewTicket = snapshot.child('currentTicket').child('isNewTicket').val();
		if (isNewTicket == true) {
			console.log('New ticket detected!');
							
			// Handle new ticket
			var mutableTicketRef = database.ref('users').child(snapshot.key).child('currentTicket');
			mutableTicketRef.child('isNewTicket').set(false);
										
			// TODO: Get scheduling assignment right
			assignTicketToClient(userId);
			
			// Do something relevant with userId...
			var mutableEtaRef = mutableTicketRef.child('eta');
			var mutableStatusRef = mutableTicketRef.child('status');
		}
	});


/** -== POD SCHEDULING FUNCTIONS ==- **/

// We can call this function here when there is a new ticket to deal
// with from Firebase. The eventListener on Firebase will provide us
// the userId. From there we can provide this function the available client #
function assignTicketToClient(firebaseUserId, clientNumber) {
	
	var data = {firebaseUserId: firebaseUserId};
	console.log("Assigning " + data.firebaseUserId + " to " + clientNumber);
	spawnClientThread(clientNumber, data.firebaseUserId);
}

// Searches the pod schedule for an available pod. Returns true if there
// is an available pod, false otherwise
function checkForAvailablePod() {
	
	var isAvailable = false;
	podSchedule.every(function (item) { // .every works like .forEach() except it allows us to break out of the loop
		if (item == 'free') {
			isAvailable = true;
			return false; // breaks out of .every() function.
		}
		return true; // continues .every() function
	});
	return isAvailable;
}

// A function that fill users based on available pod status
//  If no pods are available, it will add the user to the overflowQueue
function fillIfAvailable(userId) {
	
	// Check if its available
	if (checkForAvailablePod()) {
		addUserToPodSchedule(userId);
		return;
	}
	
	overflowSchedule.push(userID);
}

// Searches for an available pod and add a user to the schedule
function addUserToPodSchedule(userID) {
	
	podSchedule.every(function(item, index) {
		if (item == 'free') {
			podSchedule[index] = userId;
			return false;  // Break out of loop
		}
		return true; 		// Continue loop
	});
}

// Searches for a user in the pod schedule and removes the user
function removeUserFromPodSchedule(userId) {
	
	podSchedule.forEach(function(item, index) {
		if (item == userId) {
			podSchedule[index] = 'free';
		}
	});
}