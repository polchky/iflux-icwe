var serialport = require('serialport');
var SerialPort = serialport.SerialPort;
var arduinoSerial;
var express = require('express');
var app = express();

var httpPort = 3030;
var arduinoSerialPort;

var order;
var response = 1;

var nRingPixels = [
	[0,3,2,6,3,2,8],
	[4,6,2,0,0,6,6],
	[7,0,0,3,7,3,4],
	[5,2,3,5,3,3,3]
];

function getPixelsStart(module, dev){
	var tot = 0;
	for(i=0; i<dev; i++){
		tot += nRingPixels[module][i];
	}
	return tot;
}

function setRings(){
	var ring = "3/";
	var currentDev = 0;
	for(i=0; i<24; i++){
		if(currentDev < 6 && getPixelsStart(3,currentDev + 1) <= i){
			currentDev++;
		}
		ring += currentDev;
	}
	sendData(ring);
}

function arduinoSerialOpened() {
	console.log("Arduino serial communication opened on port " + arduinoSerialPort);
}

function sendData(data){
	order = "l" + (data.length) + "/" + data + "/";
	response == -1;
	console.log(order);
	arduinoSerial.write(order);
}

function arduinoSerialData(data) {
	data = data.substr(0, data.length - 1);
	console.log(data);
	switch(data){
		case "rings":
		setRings();
			break;
		case "ok":
			response == 1;
			break;
		case "resend":
			console.log("resend");
			arduinoSerial.write(order);
			break;
		case "commits":
			break;
		default:
			break;
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
	console.log(arduinoSerialPort);
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

