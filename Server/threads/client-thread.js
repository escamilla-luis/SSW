//var messenger = require('messenger');
var messenger = require('messenger');
var mysql = require('../interfaces/mysql-interface.js');
var firebase = require('../interfaces/firebase-interface.js');
var xbee = require('../interfaces/xbee-interface.js');

module.exports = function(input, done) {
	var podNum = input.podNum;
	var userId = input.userId;
	var port = input.portNum;
	var status =- 1;
	
	podAction = {
		GET_LAST_STATION: '01',
		SET_STATE: '02',
		GET_STATE: '03',
		SET_SPEED: '04',
		GET_SPEED: '05',
		SET_DESTINATION: '06',
		GET_LOCATION: '07',
		SET_NEXT_STATION: '08'
	}
	
	//Creates Listener for server communication
	var listener = messenger.createListener(portNum);
	
	//Variables will need throughout the thread.
	var ticket = null;
	var podMessage = null;
	
	//Thread spawns and receives ticket
	//Get ticket information (Firebase)
	firebase.getCurrentTIcketJson(userId, function(data) {
		ticket = data;

		//Tell pod to go to pickup user
		podMessage = messageFormatter(podNum, podAction.SET_DESTINATION, ticket.from, ticket.to);
		done({podNum: podNUm, podMessage: podMessage});	//Send server message to relay to pod
		updateStatusInDatabases(100);
	});
	
	//Sets a listener on the ticket for status changing
	firebase.setListenerForTicket(userId, function(ticketSnapshot) {
		var status = ticketSnapshot.child('status').val();

		switch (status) {
			case 300: //User just entered the pod (switched from 200)
				//Tell pod to go to destination
				podMessage = messageFormatter(podNum, podAction.SET_DESTINATION, ticket.from, ticket.to);
				done({podNum: podNUm, podMessage: podMessage});	///Send server message to relay to pod
				updateStatusInDatabases(400);
			
			case 900: //User just exited the pod; ride over; (switched from 500)
				//Tell pod to finish
				updateStatusInDatabases(900); 
				done({podNum: podNum, killThread: true});	//Thread tells server to kill it. 
			
			default:
				break;
		}
		
	});
	
	//Listen for Communication from Pod routed through server speaker
	listener.on('messageFromPod', function(message,data) {
		var stationTo = ticket.to;
		console.log('Message From Server');
		var message = data.message;
		//Process message to figure out what XBEE sent
		
//-----> I dont know how the xbee is sending data back right now.
//			Will have to process based on format it is sent.
		
		switch(message) {
			case "arrivedAtPickup":	 //Pod arrived at Pickup
				//Update status code: user must enter pod
				updateStatusInDatabases(200);
				break;
			case "arrivedAtDestination":  //Pod arrived at Dropoff
				//Update status code: user must disembark
				updateStatusInDatabases(400);
				break;
			default:
				message.reply("Communication halted");
		}
		
		message.reply(podNum + 'Received Message');
	});
	
	
	function userDeparted() {
		done({podNum: podNum, killThread: true});
	}

//	setTimeout(function() {
//		console.log(input.pod_num + " " + input.uid);
//		done({podNum: podNUm, killThread: true});
//	}, 10000);

};

//Updates both firebase and mySQL status columns when called
function updateStatusInDatabases(status) {
	firebase.setStatus(user_id, status);
	mysql.setStatus(status, podNum);
}

//Formats data for message to communication according to protocol in google drive doc
function messageFormatter(podNum, action, input1, input2) {
	//Format data for communication.
	var sPodNum = input.podNum < 10 ? "0" + input.podNum : input.podNum.toString();
	var sInput1 = input1 < 10 ? '0' + input1 : input1;
	var sInput2 = input2 < 10 ? '0' +input1 : input2;
	return sPodNum + action + input1 + input2;
}