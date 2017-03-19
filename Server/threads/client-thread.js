//var messenger = require('messenger');
var mysql = require('../database.js');
var firebase = require('../firebase/firebase-interface.js');
var mysqlEvents = require('mysql-events');
var xbee = require('../xbee/xbee-interface.js');

module.exports = function(input, done) {
	var pod_num = input.pod_num;
	var user_id = input.uid;
	
	var ticket = firebase.getCurrentTicketFromUser(user_id);
	console.log(ticket);
	
	setTimeout(function() {
		console.log(input.pod_num + " " + input.uid);
		done('DOne! ');
	}, 10000);

};

