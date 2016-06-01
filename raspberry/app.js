var serialport = require('serialport');
var SerialPort = serialport.SerialPort;
var arduinoSerial;
var express = require('express');
var app = express();

var httpPort = 3030;
var arduinoSerialPort;

var order;
var response;

var developers = [


];

function arduinoSerialOpened() {
	console.log("Arduino serial communication opened on port " + arduinoSerialPort);
}

function sendData(data){
	order = "l" + (data.length) + "/" + data + "/";
	response == -1;
	arduinoSerial.write(order);
	console.log("data sent");
}

function arduinoSerialData(data) {
	data = data.substr(0, data.length - 1);
	switch(data){
		case "rings":
		sendData("r/3/000000000000000000000011");
			break;
		case "ok":
			response == 1;
			break;
		case "resend":
			console.log("resend");
			arduinoSerial.write(order);
			break;
		case "commits":
			console.log("commit, lol");
		}
}

function arduinoSerialClosed() {}

function arduinoSerialError(error) {}



serialport.list(function (err, ports) {
	if(ports.length < 1) {
		console.log("No port found");
		process.exit();
	}
	arduinoSerialPort = ports[0].comName;
	arduinoSerial = new SerialPort( arduinoSerialPort, {
		baudRate: 9600,
		parser: serialport.parsers.readline("\n")
	});
	arduinoSerial.on('open', arduinoSerialOpened);
	arduinoSerial.on('data', arduinoSerialData);
	arduinoSerial.on('close', arduinoSerialClosed);
	arduinoSerial.on('error', arduinoSerialError);
	
	app.listen(httpPort, function () {});
});

app.get('/', function (req, res) {
	res.send('Hello World!');
});

