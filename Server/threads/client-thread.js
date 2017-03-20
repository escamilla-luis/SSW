//var messenger = require('messenger');
var mysql = require('../interfaces/mysql-interface.js');
var firebase = require('../interfaces/firebase-interface.js');
var xbee = require('../interfaces/xbee-interface.js');

module.exports = function(input, done) {
	var pod_num = input.pod_num;
	var user_id = input.uid;
	var status =- 1;
	
	firebase.getCurrentTicketJSON(user_id, function(ticketdata) {
		status = ticketdata.status;
		console.log(status);
	});
	
	
	setTimeout(function() {
		console.log(input.pod_num + " " + input.uid);
		done('DOne! ');
	}, 10000);

};

