var messenger = require('messenger');
var firebase = require('firebase');

var PORT = 8001;
var POD_NUM = 1;

var client = messenger.createListener(PORT);

client.on('assignTicket', function(message, data) {
	
	var firebaseUserId = data.firebaseUserId;
	console.log("user id passed from server is: " + firebaseUserId);
	console.log("Doing relevant work now...");
	
	// Reply to server letting it know that task is complete 
	var podNumber = POD_NUM;
	var podStatus = -1;
	
	console.log("Work done! Replying to server w/ updated status");
	message.reply({podNumber: podNumber, podStatus: podStatus});
});
