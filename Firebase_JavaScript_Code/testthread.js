var threads = require('threads');
var config = threads.config;
var spawn = threads.spawn;

// config.set({
// 	basepath : {
// 		node: __dirname
// 	}
// });

var thread = spawn('thethread.js');

thread
	.send({ do : 'Something'})
	.on('message', function(message) {
		console.log('thethread.js replied: ', message);
	});