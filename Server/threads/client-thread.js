//var messenger = require('messenger');
var mysql = require('../database.js');
var firebase = require('firebase');
var mysqlEvents = require('mysql-events');
var xbee = require('../xbee-interface.js');

module.exports = function(input, done) {
	var pod_num = input.pod_num;
	var user_id = input.uid;
	
	var ticket = xbee.getCurentTicketFromUser(user_id);
	console.log(ticket);
	
	setTimeout(function() {
		console.log(input.pod_num + " " + input.uid);
		done('DOne! ');
	}, 10000);

};

