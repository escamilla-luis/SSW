var messenger = require('messenger');


var listener = messenger.createListener(8006);
console.log('Ready...')
module.exports = function(input, done) {
    listener.on('messageFromPod', function(message, data) {        

        console.log('Message From Server');
        console.log('podStatus: ' + data.podStatus);
            
            // Process message to figure out what XBEE sent
        switch(data.podStatus) {
            case 200:	 //Pod arrived at Pickup
                console.log('switch - arrivedAtPickup');
                    //Update status code: user must enter pod
    //                updateStatusInDatabases(userId, podNum, 200);
                break;
            case 400:  //Pod arrived at Dropoff
                console.log('switch - arrivedAtDestination');
            //Update status code: user must disembark
    //            updateStatusInDatabases(userId, podNum, 400);
                break;
            default:
    //            message.reply("Unrecognized message");
        }
        message.reply({message: 'pod 06: Received Message'});
    });
}