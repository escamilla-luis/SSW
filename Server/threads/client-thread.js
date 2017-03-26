//var messenger = require('messenger');
var messenger = require('messenger');
var mysql = require('../interfaces/mysql-interface.js');
var firebase = require('../interfaces/firebase-interface.js');
var xbee = require('../interfaces/xbee-interface.js');

module.exports = function(input, done) {
//	var podNum = input.podNum;
//	var userId = input.userId;
//	var port = input.portNum;
//	var status =- 1;
//	
//	
//	var listener = messenger.createListener(portNum);
//	
//	var ticket = null;
//	//Thread spawns and receives ticket
//	//Get ticket information (Firebase)
//	firebase.getCurrentTIcketJson(userId. function(data) {
//		ticket = data;
//	});
//	
//	
//	
//	listener.on('messageFromPod', function(message,data) {
//		var stationTo = ticket.to;
//		console.log('Message From Server');
//		
//		switch() {
//			case 1: 
//				break;
//			case 2:
//				break;
//			case 3:	
//				break;
//			case 4: 
//				break;
//			default:
//			
//		}
//		
//		message.reply('Pod ' + podNum + 'receieved message');
//	});
//	
//	
//	function userDeparted() {
//		done({podNum: podNum, killThread: true});
//	}
//	var ticket = null;
//
//		//Tell Pod to go to pickup destination
//
//		xbee.writeToXbee('Go to pickup', function(data) {
//			//Thread receives signal that pod arrived
//			//Update status to waiting for user to board (200) (Firebase, MySQL)
//			firebase.setStatus(user_id, 200);
//			mysql.setStatus(200, podNum);
//			
//			//Tell Pod to go to drop off destination
//			//Update status to on the way to destination (300) (Firebase, MySQL)
//			firebase.setStatus(user_id, 300);
//			mysql.setStatus(300, pod_num);
//	// ---> Listen for changed in firebase to tell when user enters pod
//	
//			xbee.writeToXbee('Go to destination', function(data) {
//				//Thread receives signal pod arrived at destination
//				//Update status to waiting for user to disembark (400) (Firebase, MySQL)
//				firebase.setStatus(user_id, 400);
//				mysql.setStatus(400, pod_num);
//	// -------> Listen for changes in firebase to tell when user exits pod. 
//
//				//Send complete signal to Server
//				done('Pod: ' + pod_num + 'Completed Route!');
//				//Kill the thread
//			});
//		})
//
//	
//		
//	});
//	firebase.getCurrentTicketJSON(user_id, function(ticketdata) {
//		status = ticketdata.status;
//		console.log(status);
//	});
//	
//	
	setTimeout(function() {
		console.log(input.pod_num + " " + input.uid);
		done({podNum: podNUm, killThread: true});
	}, 10000);

};

