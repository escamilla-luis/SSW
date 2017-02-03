//Messenger package required for communication. 
var messenger = require('messenger');
var firebase = require('firebase');

// Initialize Firebase with our Spartan Superway database information
var config = {
	apiKey: "AIzaSyA4tiWKhfsGzygbRWgwfRb76LN4fg7gA5Q",
	authDomain: "spartan-superway.firebaseapp.com",
	databaseURL: "https://spartan-superway.firebaseio.com",
	storageBucket: "spartan-superway.appspot.com",
};
firebase.initializeApp(config);

// Sign into account
var email = "Stephen.apiazza@gmail.com";
var password = "password";
firebase.auth().signInWithEmailAndPassword(email, password).catch(function(error) {
	// Handle Errors here.
	var errorCode = error.code;
	var errorMessage = error.message;
	console.log("Error code " + errorCode);
	console.log("Error message " + errorMessage);
});

// Get currently logged in user from signed in account
var user = firebase.auth().currentUser;
// If we want to access the user's profile information, we do so here.
if (user != null) {
	console.log('user != null');
	user.providerData.forEach(function (profile) {
		console.log("Sign-in provider: "+profile.providerId);
		console.log("  Provider-specific UID: "+profile.uid);
		console.log("  Name: "+profile.displayName);
		console.log("  Email: "+profile.email);
		console.log("  Photo URL: "+profile.photoURL);
	});
}

var userId = 'Qvn71YOfXzMdmASoievQBboMEvI3';
// Get a reference to the database
var database = firebase.database()
// Here, 'userId' would be the at the root of the data pushed to Firebase
// with two child nodes 'firstName' and 'lastName' having value of
// 'Test' and 'QualityAssurance', respectively.
database.ref(userId).set({
	firstName: "Test",
	lastName: "QualityAssurance"
});
// Another example. We set 12345 as the name of the root node which contains 
// two child nodes, 'test' and 'randomNumbers', with values of 'testtesttesttest'
// and '1295500129385123010238', respectively.
database.ref('12345').set({
	test: "testtesttesttest",
	randomNumbers: "1295500129385123010238"
});

var podNum = 1;

//Set up client to speak on port 8001
client1 = messenger.createSpeaker(8001);

//Sets an interval to request the information from the server every 1 second
setInterval(function(){
	if (loc <=100) { loc++; }
	else { loc = 0; }
	//Request takes 3 parameters
	//Message name - this is how the server recognizes what request to process by name
	//JSON data  - Information for server to use 
	//Function(data) - data is info returned and can process in function call
	client1.request('assignRider', {rider: userId, pod: podNum}, function(data) {
//		console.log('user updated');
	});
}, 1000);

//client1.request('assignRider', {rider: userId, pod: podNum}, function(data) {
//	console.log('user updated');
//});


var loc = 0;

//Request to server using 'setLocation' identifier to update car location on server then send it back and update firebase.
setInterval(function(){
	if (loc <=100) { loc++; }
	else { loc = 0; }
	client1.request('setLocation', {location: loc, pod: podNum}, function(data) {
	var mutableTicketRef = database.ref("users").child(userId).child("currentTicket").child("eta");
	var mutableEtaRef = mutableTicketRef.child("eta");
	mutableEtaRef.set(data.location);
	
	});
}, 1000);

//Request 'info' 
setInterval(function(){
	client1.request('info', {pod: podNum}, function(data){
		console.log(data);
	});
}, 1000);
