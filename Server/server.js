var messenger = require('messenger');
var mysql = require('mysql');
var firebase = require('./interfaces/firebase-interface.js');
var mysqlEvents = require('mysql-events');
//var xbee = require('./interfaces/xbee-interface.js');
var SerialPort = require('serialport');
var threads = require('threads');
// Put 'config' and 'spawn' AFTER 'threads' module
var config = threads.config;
var spawn = threads.spawn;

/** -== Xbee code ==- **/
// Path to serialport may be different on other machines
var pathToPort = '/dev/tty.usbserial-DN01J8BJ';
var databuffer = [];
var stream = '';

// Refresh port every time we call writeToXbee(..)
var port = new SerialPort(pathToPort, {
	autoOpen: false,
	baudrate: 57600
});

// Set listener for port opening. We write to the xbee in callback.
port.on('open', function() {
	port.on('data', function (dataFromXbee) {
		// dataFromXbee is a Buffer data type
		var stringData = dataFromXbee.toString('utf8');
		console.log('Data from xbee: ' + stringData);
		queueBuffer(stringData);
	});
});

// We open the port, which will fire off our .on('open') callback functiondatabuffer that writes to the xbee.
port.open(function (err) {
	if (err) {
		console.log('Error opening port: ', err.message);
	} else {
		console.log('Port opened');
	}
});

function queueBuffer(input) {
	console.log(input);
	for (var i = 0; i < input.length; i++) {databuffer
		databuffer.push(input[i]);
	}
	
	addToStream();
}

function addToStream(){	
	// Dequeue buffer to go to stream
	while (databuffer.length > 0 && databuffer[0] != 'e') {
		stream += databuffer.shift();
	}
	
	if (databuffer.length > 0) {
		if (databuffer[0] == 'e') {
			databuffer.shift();
			processStream();
		}
	}
}

function processStream() {
	// 'stream' is expected to be a complete 8-char string here
	console.log('Stream: ' + stream);
	
	var podNum = getPodNumber(stream);
	var actionId = getActionId(stream);
	var actionInfo = getActionInfo(stream);
	
	console.log('Pod #: ' + podNum);
	console.log('Action Id: ' + actionId);
	console.log('Action Info: ' + actionInfo);
	
	stream = '';
}

function getPodNumber(streamData) {
	return streamData.charAt(0) + streamData.charAt(1);
}

function getActionId(streamData) {
	return streamData.charAt(2) + streamData.charAt(3);
}

function getActionInfo(streamData) {
	return streamData.charAt(4) + streamData.charAt(5) + streamData.charAt(6) + streamData.charAt(7);
}


/** -== Server/Client code ==- **/
var podSchedule = ['free', 'free', 'free', 'free'];
var overflowQueue = []; // .push() to enqueue, .shift() to dequeue

// This callback gets executed when the client sends the server a reply 
// letting it know that it's task is completed
var clientCallback = function (data) {
	console.log("Worker replied!")
	console.log("xbeeCommand: " + data.xbeeCommand);
	
	// Update pod schedule
	podSchedule[data.podNumber] = data.podStatus;
};

var speaker = messenger.createSpeaker(8001);
var onReplyCallback = function(replyData) {
	console.log('onReplyCallback');
	console.log(replyData.message);
}
//setInterval(function() {
//	speaker.request('messageFromPod', {podMessage: 'arrivedAtPickup'}, onReplyCallback);
//}, 2000);

//spawnClientThread(1, "Qvn71YOfXzMdmASoievQBboMEvI3", 8001);

firebase.setListenerForAllCurrentTickets(function(snapshot) {
		// snapshot = snapshot of ticket
		var userId = snapshot.key;
		var firstName = snapshot.child('firstName').val();
		var lastName = snapshot.child('lastName').val();

		// Only assign ticket to client-thread if 'isTicketAlive' == true
		// Worker threads will fire off listeners as they make changes to the ticket data,
		// we use isTicketAlive to negate the server handling the same ticket over and over
		var isNewTicket = snapshot.child('currentTicket').child('isNewTicket').val();
		if (isNewTicket == true) {
			console.log('New ticket detected!');
			console.log('userId: %s, firstName: %s, lastName: %s', userId, firstName, lastName);
							
			// Handle new ticket
			var userId = snapshot.key;
			var ticketRef = firebase.getUserTicketRef(userId);
			ticketRef.child('isNewTicket').set(false);
			
			// Queue pod 
			fillIfAvailable(userId);
		}
});

function spawnClientThread(podNum, userId, portNum) {
	console.log('spawning client');
	var thread = spawn('client.js');
	thread
		.send({
			podNum: podNum,
			userId: userId,
			portNum: portNum
		})
		.on('done', function(message) {
			if (message.killThread) {
				// Client thread asking to be killed
				console.log('Killing thread');
				thread.kill();
				
				// Free pod schedule and dequeue users off overflow (if any)
				podSchedule[message.podNum] = 'free';
				dequeueOverflow();
			} else {
				// Client thread is sending request for something (to run action or obtain information
				var podNum = message.podNum;
				var podMessage = message.podMessage;
				console.log('podNum: ' + podNum + ' has sent message: ' + podMessage);
//				xbee.write(podMessage);
			}
		});
}

/** -== POD SCHEDULING FUNCTIONS ==- **/

// A function that fill users based on available pod status
// If no pods are available, it will add the user to the overflowQueue
function fillIfAvailable(userId) {
	
	// Check if its available
	if (checkForAvailablePod()) {
		addUserToPodSchedule(userId);
		return;
	}
	
	// Enqueue user to pod
	overflowQueue.push(userId);
}

// Searches for an available pod and add a user to the schedule
function addUserToPodSchedule(userId) {
	console.log('addUserToPodSchedule()');
	podSchedule.every(function(item, index) {
		if (item == 'free') {
			podSchedule[index] = userId;
			spawnClientThread(index, userId, 8001 + index);
			return false;  // Break out of loop
		}
		return true; 		// Continue loop
	});
}

// We can call this function here when there is a new ticket to deal
// with from Firebase. The eventListener on Firebase will provide us
// the userId. From there we can provide this function the available client #
function assignTicketToClient(firebaseUserId, clientNumber) {
	
	var data = {firebaseUserId: firebaseUserId};
	console.log("Assigning " + data.firebaseUserId + " to " + clientNumber);
	spawnClientThread(clientNumber, data.firebaseUserId);
}

// Dequeues a user off of the overflow schedule (if any) and 
// adds that user to the overflow schedule
function dequeueOverflow() {
	var userId; // Firebase userId
	if (userId = overflowQueue.shift()) {
		addUserToPodSchedule(userId);
	}
}

// Searches the pod schedule for an available pod. 
// Returns true if there is an available pod, false otherwise
function checkForAvailablePod() {
	
	var isAvailable = false;
	podSchedule.every(function (item) { // .every works like .forEach() except it allows us to break out of the loop
		if (item == 'free') {
			isAvailable = true;
			return false; // this breaks out of the .every() function.
		}
		return true; // this continues the .every() function
	});
	return isAvailable;
}

// Searches for a user in the pod schedule and removes the user
function removeUserFromPodSchedule(userId) {
	
	podSchedule.forEach(function(item, index) {
		if (item == userId) {
			podSchedule[index] = 'free';
		}
	});
}