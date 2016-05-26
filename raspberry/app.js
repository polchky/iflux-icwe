var serialport = require('serialport');
var SerialPort = serialport.SerialPort;
var arduinoSerial;
var express = require('express');
var app = express();

var httpPort = 3030;
var arduinoSerialPort;

function arduinoSerialOpened() {
	console.log("Arduino serial communication opened on port " + arduinoSerialPort);
}

function arduinoSerialData(data) {}

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

