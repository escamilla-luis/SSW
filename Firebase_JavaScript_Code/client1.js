var messenger = require('messenger');
var firebase = require('firebase');

var client = messenger.createListener(8001);

client.on('assignTicket', function(message, data) {
	
	var firebaseUserId = data.firebaseuserId;
	console.log("user id passed from server is: " + firebaseUserId);
	
	// Reply to server letting it know that task is complete
	var podNumber = 1;
	var podStatus = -1;
	message.reply({podNumber: podNumber, podStatus: podStatus});
});
