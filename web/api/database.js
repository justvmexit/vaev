const mysql = require('mysql');

exports.connect = function(){
	const connection = mysql.createConnection({
		host: "localhost",
		user: "root",
		password: "",
		database: "væv",
		});

	connection.connect(function(err) {
		if(err) throw err;
		console.log('[væv] database connected');
	});

	return connection;
}
