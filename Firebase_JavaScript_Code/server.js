var messenger = require('messenger');
var mysql = require('mysql');
var firebase = require('firebase');
var mysqlEvents = require('mysql-events');

// Initialize Firebase with our Spartan Superway database information
var config = {
	apiKey: "AIzaSyA4tiWKhfsGzygbRWgwfRb76LN4fg7gA5Q",
	authDomain: "spartan-superway.firebaseapp.com",
	databaseURL: "https://spartan-superway.firebaseio.com",
	storageBucket: "spartan-superway.appspot.com",
};
firebase.initializeApp(config);

//Setup mySQL connection info
var con = mysql.createConnection({
	host:'localhost',
	user:'root',
	password:'password',
	database:'SSW'
})

var dsn = {
	host:     'localhost',
	user:     'root',
	password: 'password'
};

//Connect to database
con.connect(function(err){
	if(err) {
		console.log('Error connecting to Database')
		return;
	}
	console.log('Connection established...')
})

//Indexed array to hold ticket ID for pod based on pod_num 
var podSchedule  = [1234,-1,-1,-1, -1, -1];
console.log(podSchedule);
//Create server listening on port 8001
server1 = messenger.createListener(8001);

server1.on("assignTicket" , function(message, data) {
	var pod_num = data.pod_num - 1;
		console.log(pod_num);
	if (podSchedule[pod_num] != -1) {
		message.reply(podSchedule[pod_num]);
	}
	var podText = "";
	for (var i = 0; i < podSchedule.length; i++) {
	 podText += podSchedule[i] + ",";
	}
	console.log(podSchedule);
});

server1.on("updatePodSchedule", function(message, data){
	var pod_num = data.pod_num - 1;
	var busy = data.busy;
	if (!busy) {
		podSchedule[pod_num] = -1;
		message.reply("Updated to Inactive");
	}
});


var currentTicketRef = firebase.database().ref('users/Qvn71YOfXzMdmASoievQBboMEvI3/currentTicket');

currentTicketRef.on('value', function(snapshot) {
	var from = snapshot.val().from;
	var to = snapshot.val().to;
	console.log(from + "  " + to);
	con.query('UPDATE pods SET station_from='+from+', station_to=' +to+ ' WHERE pod_num=1');
});

