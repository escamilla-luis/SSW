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

// Constants for Action ID's for pod commands
var podAction = {
	GET_LAST_STATION: '01',
	SET_STATE: '02',
	GET_STATE: '03',
	SET_SPEED: '04',
	GET_SPEED: '05',
	SET_DESTINATION: '06',
	GET_LOCATION: '07',
	SET_NEXT_STATION: '08',
	GET_CLOSEST_STATION: '09',
	STOPPED_AT_STATION: '10',
	PROCEED: '11'
}

/** -== Xbee code ==- **/
// Path to serialport may be different on other machines
var pathToPort = '/dev/tty.usbserial-DN01J57O';
var databuffer = [];
var stream = '';

// Global, shared variables between functions used for pod scheduling
//var podSchedule = ['free', 'free', 'free', 'free'];
var podSchedule = ['free'];
var overflowQueue = []; // .push() to enqueue, .shift() to dequeue

var numFreePods = 0;
var userIdShared = ''; // 
var startingStationShared = 0;
var closestToStationArr = [];
var closestToStationFlag = false;

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
		processStream(stringData);
		
//		queueBuffer(stringData);
	});
});

function processStream(data) {
	// 'stream' is expected to be a complete 8-char string here
	console.log('Stream: ' + data);
	
	var podNum = getPodNumber(data);
	var actionId = getActionId(data);
	var actionInfo = getActionInfo(data);
	stream = ''; // Clear stream (if using buffering to stream data received from pods)
	
	console.log('Pod #: ' + podNum);
	console.log('Action Id: ' + actionId);
	console.log('Action Info: ' + actionInfo);
	
	// Update speaker port #
	var portNumber = 8000 + parseInt(podNum);
	var speaker = messenger.createSpeaker(portNumber);
	console.log('Speaker port #: ' + portNumber);
	
	switch (actionId) {
		case podAction.GET_CLOSEST_STATION:
			console.log('GET_CLOSEST_STATION');
			// Error checking
			if (!closestToStationFlag) {
				console.log('Error: Server received action Id ' + actionId + ' but flag was cleared.');
				console.log('Did you clear your flag too early?')
			}
			
			// Store return data in our array
			closestToStationArr.push(data);
			if (closestToStationArr.length == podSchedule.length) {
				console.log(closestToStationArr);
				// Should have all return values from pods for getClosestStation() received here
				for (var y = 0; y < podSchedule.length; y++) {
					for (var x = 0; x < podSchedule.length; x++) {
						
						console.log('x: ' + x + ', y: ' + y);
						// 0X - 0Y - ABCD  ->  format of data
						var podNumber = getPodNumber(closestToStationArr[y])
						var closestStations = getActionInfo(closestToStationArr[y]);
						var closestStation = closestStations.charAt(x);
						if (closestStation == startingStationShared) {
							
							var podNumInteger = parseInt(podNumber);
							assignUserToPodNumber(userIdShared, podNumInteger);
							closestToStationArr = []; // clear array
							return;
							// FIXME: Need to address how to find next pod if closest pod is unavailable
							//		This solution only works if all pods are available
							// 		-- filter out list of pod return values [a,b,c,d] before
							//		   doing the double for-loop 
							if (podSchedule[podNumber] != 'free') {
							}
						}
					}
				}
			}
			break;
		case podAction.STOPPED_AT_STATION:
			console.log('STOPPED_AT_STATION');
			var podStatus = actionInfo.substring(0, 3);
			console.log('podStatus: ' + podStatus);
			setTimeout(function() {
				console.log('SPEAKER.REQUEST');
				speaker.request('messageFromPod', { podStatus: podStatus }, onReplyCallback);	
			}, 1200);
			break;
		default:
			console.log('Error: Unrecognized Action ID');
			break;
	}
	
//	var podMessage = podNum + actionId + actionInfo
//	speaker.request('messageFromPod', {podMessage: podMessage}, onReplyCallback);
}

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
			processStream(stream);
		}
	}
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

/** -== Node.js messenger API ==- **/
//speaker = messenger.createSpeaker(8006);
var onReplyCallback = function(replyData) {
	console.log('onReplyCallback: ' + replyData.message);
}


/** -== Server/Client code ==- **/

// This callback gets executed when the client sends the server a reply 
// letting it know that it's task is completed
var clientCallback = function (data) {
	console.log("Worker replied!")
	console.log("xbeeCommand: " + data.xbeeCommand);
	
	// Update pod schedule
	podSchedule[data.podNumber] = data.podStatus;
};

firebase.setListenerForAllCurrentTickets(function(snapshot) {
		// snapshot = snapshot of ticket
		var userId = snapshot.key;
		var firstName = snapshot.child('firstName').val();
		var lastName = snapshot.child('lastName').val();

		// Only assign ticket to client-thread if 'isTicketAlive' == true
		// Worker threads will fire off listeners as they make changes to the ticket data,
		// we use isTicketAlive to negate the server handling the same ticket over and over
		var ticketSnapshot = snapshot.child('currentTicket');
		var isNewTicket = ticketSnapshot.child('isNewTicket').val();
		if (isNewTicket == true) {
			console.log('New ticket detected!');
			console.log('userId: %s, firstName: %s, lastName: %s', userId, firstName, lastName);
			
			// Handle new ticket
			var userId = snapshot.key;
			var ticketRef = firebase.getUserTicketRef(userId);
			ticketRef.child('isNewTicket').set(false);
			
			startingStationShared = ticketSnapshot.child('from').val();			
			userIdShared = userId;
			closestToStationFlag = true;
			collectClosestStations();
			spawnClientThread(6, userId, 8006)
			
			// Queue pod 
//			fillIfAvailable(userId);
		}
});

function collectClosestStations() {
	// Get number of free pods by filtering the pod schedule
	numFreePods = podSchedule.filter(function(element, i) {
		return element == 'free';
	}).length;
	
		console.log(numFreePods);
	// Ask all pods that are available ('free') for closest stations
	var delay = 400;
	podSchedule.forEach(function(element, index) {
		if (element == 'free') {
			// (index + 1)  -> pod number (since index starts at 0)
			// 09 -> action id for getClosestStation()
			var podNumber = (index + 1);
			var podCommand = '0' + '3' + '09' + '0000';
			console.log(podCommand);
			setTimeout(function() {
				port.write(podCommand);
			}, delay);
			delay += 400;
		}
	});
}

function spawnClientThread(podNum, userId, portNum) {
	console.log('spawning client\n');
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
				var podCommand = message.podCommand;
				console.log('client for podNum ' + podNum + ' has sent command: ' + podCommand);

				port.write(podCommand);
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

// Searches for an available pod and add a user to the schedule
function assignUserToPodNumber(userId, podNumber) {
	console.log('assignUserToPodNumber()');
	podSchedule[podNumber] = userId;
	spawnClientThread(podNumber, userId, 8001 + podNumber);
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