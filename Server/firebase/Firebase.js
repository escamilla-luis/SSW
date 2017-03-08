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
var email = "test@email.com";
var password = "TestEmail12345";
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

var userId = "example";
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