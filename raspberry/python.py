import serial
import time
import random


orders = [0 for i in range(20)]
lastOrderOk = 1
canSendRandomCommit = 1
lastRandomCommit = time.time()

ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
rings = [
	333344445555444433334444,
	333344445555444433334444,
	333344445555444433334444,
	333344445555444433334444
]

weeks = 48
modules = 4
commits = [[0 for i in range(weeks)] for j in range(modules)]
commits[0][0] = [
	[], [], [], [], [], [],
	[], [], [], [], [], [],
	[], [], [], [], [], [],
	[], [], [], [], [], [],
	[], [], [], [], [], [],
	[], [], [], [], [], [],
	[], [], [], [], [], [],
	[], [], [], [], [], [],
]

def sendNextOrder():
	global orders
	global lastOrderOk
	index = nextOrderIndex()
	if index == -1:
		return
	else:
		lastOrderOk = 0;
		order = "l" + str(len(orders[index])) + "/" + str(orders[index]) + "/"
		print "sending " + order
		ser.write(order)


def sendRings():
	for i in range(4):
		addOrder("r/" + str(i) + "/" + str(rings[i]))
		
def addOrder(order):
	global orders
	for i in range(len(orders), 0, -1):
		if not orders[i - 1] == 0:
			orders[i] = order
			return
	orders[0] = order

def nextOrderIndex():
	global orders
	for i in range(len(orders)):
		if not orders[i] == 0:
			return i
	return -1

def createCommitPerhaps():
	global lastRandomCommit
	if time.time() - lastRandomCommit > 10:
		lastRandomCommit = time.time()
		dev = random.randint(0,4)
		module = random.randint(0,3)
		strength = random.randint(0,6)
		addOrder("c/" + str(dev) + str(module) + "0004" + str(strength))
	

def ordersEmpty():
	global orders
	for i in range(len(orders)):
		if not orders[i] == 0:
			return 0
	return 1
	
addOrder("d/5")
	
while 1:
	# Read data
	data = ser.readline()
	time.sleep(0.05)
	if len(data) > 3:
		data = data[:-2]
		print data
		if data == "ok":
			orders[nextOrderIndex()] = 0
			lastOrderOk = 1
		elif data == "resend":
			sendNextOrder()
		elif data == "rings":
			canSendRandomCommit = 0
			sendRings()
	# Send next order
	if lastOrderOk == 1:
		sendNextOrder()
	# send random commit
	if(ordersEmpty() and canSendRandomCommit == 1):
		createCommitPerhaps()
		
		
	
	
