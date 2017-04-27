var threads = require('threads');
// Put 'config' and 'spawn' AFTER 'threads' module
var config = threads.config;
var spawn = threads.spawn;
var messenger = require('messenger');

var portNum = 8000 + parseInt('06');
console.log(portNum);
var speaker = messenger.createSpeaker(portNum);
var onReplyCallback = function(replyData) {
    console.log('onReplyCallback: ' + replyData.message);
}

setTimeout(function() {
    speaker.request('messageFromPod', { podStatus: 200 }, onReplyCallback);
}, 1500);

var thread = spawn('listenerTest.js');
    thread
        .send({
            nothing: 1
        })
        .on('done', function(message) {
            if (message.killThread) {
                // Client thread asking to be killed
                console.log('Killing thread');
                thread.kill();
            }
        });



//client = messenger.createSpeaker(8000);
//
//setInterval(function(){
//    client.request('give it to me', {hello: 123456}, function(data){
//        console.log(data);
//    });
//}, 1000);
