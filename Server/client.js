//var messenger = require('messenger');
var messenger = require('messenger');
var mysql = require('./interfaces/mysql-interface.js');
var firebase = require('./interfaces/firebase-interface.js');

// Constants that define the actions to send to the pod
var podAction = {
    GET_LAST_STATION: '01',
    SET_STATE: '02',
    GET_STATE: '03',
    SET_SPEED: '04',
    GET_SPEED: '05',
    SET_DESTINATION: '06',
    GET_LOCATION: '07',
    SET_NEXT_STATION: '08'
}

var ledState = {
    RED: '0001',
    BLUE: '0002',
    GREEN: '0003',
    RED_FLASHING: '0004',
    BLUE_FLASHING: '0005',
    GREEN_FLASHING: '0006'
}


module.exports = function(input, done) {
    var podNum = input.podNum;
    var userId = input.userId;
    var portNum = input.portNum;
    var status =- 1;
    
        
    //Creates Listener for server communication
    var listener = messenger.createListener(portNum);
    
    //Thread spawns and receives ticket
    //Get ticket information (Firebase)
    firebase.getCurrentTicketJson(userId, function(data) {
        // Variables will need throughout the thread.
        var ticket = data;
        var podMessage = messageFormatter(podNum, podAction.SET_DESTINATION, ticket.from, ticket.to);
        console.log('podMessage: ' + podMessage);

        // Tell pod to go to pickup user
        done({podNum: podNum, podMessage: podMessage});	// Send server message to relay to pod
        updateStatusInDatabases(userId, podNum, 100);
    
        // Sets a listener on the ticket for status changing
        firebase.setListenerForTicket(userId, function(ticketSnapshot) {
            var status = ticketSnapshot.child('status').val();
            var from = ticketSnapshot.child('from').val()
            console.log('from: ' + from);

            switch (status) {
                case 100:
                    console.log('status: ' + 100);
                    // FIXME: This assumes the pod is at station 1 when user orders a ticket
                    // Tells pod to go to user's starting location
                    podMessage = messageFormatter(podNum, podAction.SET_DESTINATION, 1, ticket.from);
                    done({podNum: podNum, podMessage: podMessage});
                    updateStatusInDatabases(userId, podNum, 200);
                    break;
                case 200:
                    console.log('status: ' + 200);
                    // TODO: Sync LED color of pod with color displayed on mobile app.
                    // Pod arrived at user's starting location, waiting for user to get inside
                    podMessage = messageFormatter(podNum, podAction.SET_STATE, ledState.GREEN_FLASHING);
                    done({podNum: podNum, podMessage: podMessage});
//                    updateStatusInDatabases(userId, podNum, 200); // We should not update Firebase for this status from server
                    break;
                case 300: 
                    console.log('status: ' + 300);
                    // User just entered the pod (switched from 200)
                    // Tell pod to go to destination
                    podMessage = messageFormatter(podNum, podAction.SET_DESTINATION, ticket.from, ticket.to);
                    done({podNum: podNum, podMessage: podMessage});	// Send server message to relay to pod
                    updateStatusInDatabases(userId, podNum, 400);
                    break;
                case 400:
                    console.log('status: ' + 400);
                    // TODO: Sync LED color of pod with color displayed on mobile app.
                    // Pod arrived at user's destination, waiting for user to exit
                    podMessage = messageFormatter(podNum, podAction.SET_STATE, ledState.RED_FLASHING);
                    done({podNum: podNum, podMessage: podMessage});
//                    updateStatusInDatabases(userId, podNum, 400); // We should not update Firebase for this status from server
                    break;
                case 900: 
                    console.log('status: ' + 900);
                    // User just exited the pod; ride over; (switched from 500)
                    // Tell pod to finish
                    podMessage = messageFormatter(podNum, podAction.SET_STATE, ledState.RED);
                    done({podNum: podNum, podMessage: podMessage});
                    updateStatusInDatabases(userId, podNum, 900); 
                    done({podNum: podNum, killThread: true});	// Thread tells server to kill it. 
                    break;
                default:
                    break;
            }
            
        });
        
        // Listen for Communication from Pod routed through Server's speaker
        listener.on('messageFromPod', function(message, data) {
            var stationTo = ticket.to;
            console.log('Message From Server');
            var message = data.message;
            //Process message to figure out what XBEE sent
            
    //-----> I dont know how the xbee is sending data back right now.
    //			Will have to process based on format it is sent.
            
            switch(message) {
                case 'arrivedAtPickup':	 //Pod arrived at Pickup
                    //Update status code: user must enter pod
//                    updateStatusInDatabases(userId, podNum, 200);
                    break;
                case 'arrivedAtDestination':  //Pod arrived at Dropoff
                    //Update status code: user must disembark
//                    updateStatusInDatabases(userId, podNum, 400);
                    break;
                default:
                    message.reply("Communication halted");
            }
            message.reply(podNum + 'Received Message');
        });    
    });
};

function userDeparted() {
    done({podNum: podNum, killThread: true});
}

//Updates both firebase and mySQL status columns when called
function updateStatusInDatabases(userId, podNum, status) {
    firebase.setStatus(userId, status);
//    mysql.setStatus(status, podNum); // FIXME: MySQL interface needs to be tested
}

// Formats data for message to communication according to protocol in google drive doc
// The format should follow a 8-digit number
function messageFormatter(podNum, action, input1, input2) {
    //Format data for communication.
    var sPodNum = podNum < 10 ? "0" + podNum : podNum.toString();
    var sInput1 = input1 < 10 ? '0' + input1 : input1.toString();
    var sInput2 = input2 < 10 ? '0' +input1 : input2.toString();
    return sPodNum + action + sInput1 + sInput2;
}

// Overload function to support single inputs - ie. setAction for LED
function messageFormatter(podNum, action, input) {
    var sPodNum = podNum < 10 ? "0" + podNum : podNum.toString();
    return sPodNum + action + input.toString();
}