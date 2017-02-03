var messenger = require('messenger');
var mysql = require('mysql');

//Setup mySQL connection info
var con = mysql.createConnection({
	host:'localhost',
	user:'root',
	password:'password',
	database:'SpartanSuperway'
})

//Connect to database
con.connect(function(err){
	if(err) {
		console.log('Error connecting to Database')
		return;
	}
	console.log('Connection established...')
})
 
//Create server listening on port 8000
server1 = messenger.createListener(8000);

//Create server listening on port 8001
server2 = messenger.createListener(8001);
 
//Set up action to be taking on 'give it to me' request from client
server1.on('give it to me', function(message, data){
	console.log('message recieved');
	message.reply({'you':'got it'});
});

//Set action to be taken on 'info' request
//data is data sent from client 
server2.on('info', function(message, data){
	var pod = data.pod;
	console.log('Request from '+pod+' recieved...')
	con.query('SELECT location FROM SpartanSuperway.location WHERE pod='+ pod +';', function(err, rows) {
		if (err) {
			console.log(err);
		} else {
			//returns information to the client
			message.reply(rows);
		}
	});
});

//Set action to be taken on 'setLocation'
server2.on('setLocation', function(message, data) {
	var pod = data.pod
	var location = data.location
	//Update locationin MySQL
	con.query('UPDATE location SET location ='+ data.location +' WHERE pod='+ data.pod);
	message.reply(location);
});
 
//Set action to be taken for 'assignRier;
server2.on('assignRider', function(message, data) {
	var pod = data.pod
 	console.log(data.rider)
	//Update rider in mySQL for given pod
	con.query('UPDATE podAssignment SET rider = \'' + data.rider + '\' WHERE pod = ' + data.pod);
	//Return success to client
	message.reply('Success');
});
//setInterval(function(){
//	client.request('give it to me', {hello:'world'}, function(data){
//		console.log(data);
//	});
//}, 1000);
