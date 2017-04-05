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
    RED:          '0001',     // At station/checkpoint
    RED_FLASH:    '0002',    // Kill state
    BLUE:         '0002',             // Read magnet, station, or checkoint
    BLUE_FLASH:   '0005',    // 
    GREEN:        '0003',            // *Idle
    GREEN_FLASH:  '0006'    // 
    PURPLE:       '0007'
    PURPLE_FLASH: '0008'
    YELLOW:       '0009'
    YELLOW_FLASH: '0010'
}


module.exports = function(input, done) {
    var podNum = input.podNum;
    var userId = input.userId;
    var portNum = input.portNum;
    console.log('portNum: ' + portNum);
    var status =- 1;
    
        
    //Creates Listener for server communication
    var listener = messenger.createListener(portNum);
    
    //Thread spawns and receives ticket
    //Get ticket information (Firebase)
    firebase.getCurrentTicketJson(userId, function(data) {
        // Variables will need throughout the thread.
        var ticket = data;
        var input = formatLocationInput(ticket.from, ticket.to);
        var podMessage = messageFormatter(podNum, podAction.SET_DESTINATION, input);
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
                    var input = formatLocationInput(1, ticket.from);
                    podMessage = messageFormatter(podNum, podAction.SET_DESTINATION, input);
                    done({podNum: podNum, podMessage: podMessage});
                    updateStatusInDatabases(userId, podNum, 200);
                    break;
                case 200:
                    console.log('status: ' + 200);
                    // TODO: Sync LED color of pod with color displayed on mobile app.
                    // Pod arrived at user's starting location, waiting for user to get inside
                    podMessage = messageFormatter(podNum, podAction.SET_STATE, ledState.GREEN_FLASHING);
                    done({podNum: podNum, podMessage: podMessage});
                    // We should not have to update Firebase for this status from server
//                    updateStatusInDatabases(userId, podNum, 200); 
                    break;
                case 300: 
                    console.log('status: ' + 300);
                    // User just entered the pod (switched from 200)
                    // Tell pod to go to destination
                    var input = formatLocationInput(ticket.from, ticket.to);
                    podMessage = messageFormatter(podNum, podAction.SET_DESTINATION, input);
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
        // message = the message OBJECT
        // data = the json object passed from server
        listener.on('messageFromPod', function(message, data) {
            
            // TODO: Filter data so that podMessage corresponds to the right client #
            var stationTo = ticket.to;
            console.log('Message From Server');
            console.log('podMessage: ' + data.podMessage);
            
            // Process message to figure out what XBEE sent
            switch(data.podMessage) {
                case 'arrivedAtPickup':	 //Pod arrived at Pickup
                    console.log('switch - arrivedAtPickup');
                    //Update status code: user must enter pod
//                    updateStatusInDatabases(userId, podNum, 200);
                    break;
                case 'arrivedAtDestination':  //Pod arrived at Dropoff
                    console.log('switch - arrivedAtDestination');
                    //Update status code: user must disembark
//                    updateStatusInDatabases(userId, podNum, 400);
                    break;
                default:
                    message.reply("Unrecognized message");
            }
            message.reply({message: podNum + ': Received Message'});
        });    
    });
};

function userDeparted() {
    done({podNum: podNum, killThread: true});
}

//Updates both firebase and mySQL status columns when called
function updateStatusInDatabases(userId, podNum, status) {
    firebase.setStatus(userId, status);
    mysql.setStatus(status, podNum); // FIXME: MySQL interface needs to be tested
}

function formatLocationInput(from, to) {
    var sFrom = from < 10 ? '0' + from : from;
    var sTo = to < 10 ? '0' + to : to;
    return sFrom + '' + sTo;
}

// Formats data for message to communication according to protocol in google drive doc
// The format should follow a 8-digit number
function messageFormatter(podNum, action, input) {
    var sPodNum = podNum < 10 ? "0" + podNum : podNum;
    return sPodNum + action + input;
}