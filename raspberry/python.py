import serial
import time
import random


orders = [0 for i in range(20)]
lastOrderOk = 1
lastRandomCommit = time.time()

ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
nDevs = 5
rings = [
	[3,6,3,8,4],
	[0,4,11,9,0],
	[6,9,9,0,0],
	[5,5,2,8,4]
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

def findDevPos(dev, module):
	if dev == 0:
		return 0
	global rings
	pos = 0
	for i in range(dev):
		pos += rings[module][i]
	return pos

def sendRings():
	for module in range(4):
		order = "r/" + str(module) + "/"
		for dev in range(nDevs):
			for pos in range(rings[module][dev]):
				order += str(dev)
		addOrder(order)
		
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
	return
	global lastRandomCommit
	if time.time() - lastRandomCommit > 10:
		lastRandomCommit = time.time()
		dev = random.randint(0,4)
		module = 3 
		strength = random.randint(0,6)
		addOrder("c/" 
			+ str(dev) 
			+ str(module) 
			+ "%02d" % findDevPos(dev, module)
			+ "01"
			+ str(strength))
	
def sendCommits(dev):
	newOrders = [
		"t/30000037/30103035/30406045"
	]
	for newOrder in newOrders:
		addOrder(newOrder)
	addOrder("f")

def ordersEmpty():
	global orders
	for i in range(len(orders)):
		if not orders[i] == 0:
			return 0
	return 1
	
addOrder("d/5")
sendRings()
	
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
			sendRings()
		elif len(data) == 9 and data[:7] == "commits":
			sendCommits(data[8:])
	# Send next order
	if lastOrderOk == 1:
		sendNextOrder()
	# send random commit
	if ordersEmpty():
		createCommitPerhaps()
		
		
	
	
