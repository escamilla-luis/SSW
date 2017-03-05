module.exports = function(input, done) {

	for (var i = 1000000000 - 1; i >= 0; i--) {
		if(i == 200000000) {
			console.log('just started');
		}
		if(i == 400000000) {
			console.log('almost halfway');
		}
		if(i == 600000000) {
			console.log('after halfway');
		}
		if(i == 800000000) {
			console.log('almost there!');
		}
	};

	done('yay!');

	// setTimeout(function() { 
 //   		console.log(input.do + " is done!")
 //  	 }, 5000);

	//done(input.do + 'hey its in thethread!');
};