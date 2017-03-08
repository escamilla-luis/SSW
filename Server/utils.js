// Gets an array of current tickets from Firebase
function getCurrentTickets() {
    var users = [];
    var tickets = [];
    
    usersStart = firebase.database().ref('users');

    //first start by getting the array of users
    usersStart.on('value', function(snapshot) {

        for (var i = Object.keys(snapshot.val()).length - 1; i >= 0; i--) {
            
            users.push(Object.keys(snapshot.val())[i]);
            //console.log(Object.keys(snapshot.val())[i]);
            
        };

        //then get the array of tickets
        for (var i = users.length - 1; i >= 0; i--) {
            var ticketStart = firebase.database().ref('users/' + users[i]+'/currentTicket');

            ticketStart.on('value', function(snapshot) {
                //console.log(snapshot.val());
                tickets.push(snapshot.val());
            });
        };

        console.log(tickets);
        return tickets;
    });
}

