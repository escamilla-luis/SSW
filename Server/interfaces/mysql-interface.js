
var mysql = require('mysql');

var pool = mysql.createPool({
	host     : 'localhost',
	user     : 'root',
	password : 'password',
	database : 'SSW'
});


/*** --== QUERY FUNCTIONS ==-- ***/

exports.getStationTo = function getStationTo(pod_num, callback) {
	pool.getConnection(function(err, connection) {
		connection.query('SELECT station_to FROM pods WHERE pod_num = ?', [pod_num], function(error, results, fields) {
			connection.release();
			if (error) throw error;
			callback(null, results[0].station_to);
		});
	});
}

exports.getStationFrom = function getStationFrom(pod_num, callback) {
	pool.getConnection(function(err, connection) {
		connection.query('SELECT station_from FROM pods WHERE pod_num = ?', [pod_num], function(error, results, fields) {
			connection.release();
			if (error) throw error;
			callback(null, results[0].station_from);
		});
	});
}

exports.getLastCheckpoint = function getLastCheckpoint(pod_num, callback) {
	pool.getConnection(function(err, connection) {
		connection.query('SELECT last_checkpoint FROM pods WHERE pod_num = ?', [pod_num], function(error, results, fields) {
			connection.release();
			if (error) throw error;
			callback(null, results[0].last_checkpoint);
		});
	});
}

exports.getTimeLeftCheckpoint = function getTimeLeftCheckpoint(pod_num, callback) {
	pool.getConnection(function(err, connection) {
		connection.query('SELECT time_left_checkpoint FROM pods WHERE pod_num = ?', function(error, results, fields) {
			connection.release();
			if (error) throw error;
			callback(null, results[0].time_left_checkpoint);
		});
	});
}

exports.getStatus = function getStatus(pod_num, callback) {
	pool.getConnection(function(err, connection) {
		connection.query('SELECT status FROM pods WHERE pod_num = ?', [pod_num], function(error, results,fields) {
			connection.release();
			if (error) throw error;
			callback(err, results[0].status);
		});
	});
}


/*** --== STORE FUNCTIONS ==-- ***/

exports.setStatus = function setStatus(status, pod_num) { 
	pool.getConnection(function(err, connection) {
		connection.query('UPDATE pods SET status = ? WHERE pod_num = ?', [status, pod_num]);
		connection.release();
	});
}

exports.setStationTo = function setStationTo(station, pod_num) { 
	pool.getConnection(function(err, connection) {
		connection.query('UPDATE pods SET station_to = ? WHERE pod_num = ?', [station, pod_num]);
		connection.release();
	});
}

exports.setStationFrom = function setStationFrom(station, pod_num) { 
	pool.getConnection(function(err, connection) {
		connection.query('UPDATE pods SET station_from = ? WHERE pod_num = ?', [station, pod_num]);
		connection.release();
	});
}


exports.setLastCheckpoint = function setLastCheckpoint(last_checkpoint, pod_num) {
	pool.getConnection(function(err, connection) {
		connection.query('UPDATE pods SET location = ? WHERE pod_num = ?',[last_checkpoint, pod_num]);
		connection.release();
	});
}

exports.setTimeLeftStation = function setTimeLeftStation(time_left, pod_num) {
	pool.getConnection(function(err, connection) {
		connection.query('UPDATE pods SET time_left_station = ? WHERE pod_num = ?', [time_left, pod_num]);
		connection.release();
	});
}
