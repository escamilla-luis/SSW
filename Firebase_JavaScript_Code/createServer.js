var messenger = require('messenger');
var mysql = require('mysql');
var firebase = require('firebase');
var mysqlEvents = require('mysql-events');
var threads = require('threads');
var config = threads.config;
var spawn = threads.spawn;

// Initialize Firebase with our Spartan Superway database information
var config = {
	apiKey: "AIzaSyA4tiWKhfsGzygbRWgwfRb76LN4fg7gA5Q",
	authDomain: "spartan-superway.firebaseapp.com",
	databaseURL: "https://spartan-superway.firebaseio.com",
	storageBucket: "spartan-superway.appspot.com",
};

firebase.initializeApp(config);

// Setting up MySQL database for number of rows needed.
var con = mysql.createConnection({
	host:'localhost',
	user:'timmahwork',
	password:'',
	database:'SSW'
})

//createMySQLDatabase();


// To check if the correct number of pods were inserted into the database
// con.query('SELECT * FROM pods', function (error, results, fields) {
// 	if (error) throw error;
// 	console.log('before select');
// 	for (var i = 0; i < results.length; i++) {
// 		console.log(results[i]);
// 	};
// });

var i = 1;

setInterval(function() {
	
	var thread = spawn('client.js');

	i = Math.floor(Math.random() * 10);

	console.log(i);

	thread
		.send({ do : i})
		.on('message', function(message) {
			//console.log('replied: ', message);
			thread.kill();
		});
	
}, 1000);

//creates the Database based on the input from command line
function createMySQLDatabase() {

	// Drop old Table
	con.query('DROP TABLE pods', function (error, results, fields){
		if (error) throw error;
	});

	// Create new Table
	con.query('CREATE TABLE pods (pod_num INT, station_from INT, station_to INT, status INT, time_left_station INT)', function (error, results, fields) {
		if (error) throw error;
	});

	// Insert values to represent the correct number of pods
	for (var i = 0; i < numberOfRows(); i++) {
		var podnumreal = i + 1;
		con.query('INSERT INTO pods (pod_num) VALUES (' + podnumreal + ')', function (error, results, fields) {
			if (error) throw error;
		});
	};


}

//fills rows in MySQL table based on argument in command line
function numberOfRows() {

	//get argument from command line and make it an int
	var argumentValue = parseInt(process.argv[2]);

	//check if the argument is a number
	if (!isNaN(argumentValue)) {
		return argumentValue;
	}
	else {
		return 0;
	}
}

con.end();