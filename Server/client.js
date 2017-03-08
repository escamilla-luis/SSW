//var messenger = require('messenger');
var mysql = require('mysql');
var firebase = require('firebase');
var mysqlEvents = require('mysql-events');

module.exports = function(input, done) {

  setTimeout(function() { 
  	console.log(input.do + " is done!")
  }, 5000);
  
  //done('Awesome thread script may run in browser and node.js!');
};

// var POD_NUM = 1;

// client.on('assignTicket', function(message, data) {
	
// 	var firebaseUserId = data.firebaseUserId;
// 	console.log("user id passed from server is: " + firebaseUserId);
// 	console.log("Doing relevant work now...");
	
// 	// Reply to server letting it know that task is complete 
// 	var podNumber = POD_NUM;
// 	var podStatus = -1;
	
// 	console.log("Work done! Replying to server w/ updated status");
// 	message.reply({podNumber: podNumber, podStatus: podStatus});
// });
