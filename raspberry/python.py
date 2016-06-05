import serial
import time
import random


orders = [0 for i in range(20)]
lastOrderOk = 1
lastRandomCommit = time.time()
noRandomCommitBefore = time.time() + 10
busy = 0
connected = 0
ser = 0

def openSerial():
	global ser
	global connected
	while connected == 0:
		try:
			ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
			connected = 1
			print "serial port opened"
		except serial.SerialException:
			time.sleep(2)

nDevs = 5
rings = [
	[3,6,3,8,4],
	[0,4,11,9,0],
	[6,9,9,0,0],
	[5,5,2,8,4]
]

weeks = 48
modules = 4
commits = [[] for i in range(5)]

commits[0] = [
	"t/00800031/00903031/01006031",
	"t/01109011/01210011/01311011",
	"t/01412011/01513011/01614021",
	
	
	
	"t/20000017/20101035/20404025",
	"t/20506017/20607015/20707015",
	"t/20808027/21008015/21109015",
	"t/21210057/21315035/21418045",
	
	
	
	"t/30000017/30101035/30404025",
	"t/30506017/30607015/30707015",
	"t/30808027/31008015/31109015",
	"t/31210057/31315035/31418045"
]

commits[1] = [
	"t/00000017/00101035/00404025",
	"t/00506017/00607015/00707015",
	"t/00808027/01008015/01109015",
	"t/01210057/01315035/01418045",
	
	"t/10000017/10101035/10404025",
	"t/10506017/10607015/10707015",
	"t/10808027/11008015/11109015",
	
	"t/20000017/20101035/20404025",
	"t/20506017/20607015/20707015",
	"t/20808027/21008015/21109015",
	"t/21210057/21315035/21418045",
	"t/21522011/21622011/21822021",
	
	"t/30500021/30702031/30905051",
	"t/31008011/31108011/31208011",
	"t/31309031/31411011/31612021",
	"t/31414011/31515011/31715031"
]

commits[2] = [
	"t/00000017/00101035/00404045",
	"t/01208047/01312065/01418045",
	"t/01512011/01613011/01714021",
	
	"t/10000017/10101035/10404025",
	"t/10506017/10607015/10707015",
	"t/10808027/11008015/11109015",
	"t/11210057/11315035/11418045",
	"t/11522011/11622011/11723011",
	
	"t/20000017/20101035/20404025",
	"t/20506017/20607015/20707015",
	"t/20808027/21008015/21109015",
	"t/21210057/21315035/21418045",
	"t/21522011/21622011/21822021",
	
	
	"t/30100021/30202021/30304011",
	"t/30504011/30705011/30906021"
]

commits[3] = [
	"t/00000017/00101035/00404025",
	"t/00506017/00607015/00707015",
	"t/00808027/01008015/01109015",
	"t/01210057/01315035/01418045",
	"t/01512011/01613011/01714021",
	
	"t/10000017/10101035/10404025",
	"t/10506017/10607015/10707015",
	"t/10808027/11008015/11109015",
	"t/11210057/11315035/11418045",
	"t/11522011/11622011/11723011",
	
	
	
	"t/30100021/30202021/30304011",
	"t/30504011/30705011/30906021",
	"t/31008011/31108011/31208011",
	"t/31309031/31411011/31612021"
]

commits[4] = [
	"t/00000021/00102011/00303011",
	"t/00403011/00503011/00704021",
	"t/00805011/00906011/01007031",
	"t/01109011/01210011/01311011",
	"t/01412011/01513011/01614021",
	
	
	
	
	
	"t/30100021/30202021/30304011",
	"t/30504011/30705011/30906021",
	"t/31008011/31108011/31208011",
	"t/31309031/31411011/31612021"
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
	global busy
	for module in range(4):
		order = "r/" + str(module) + "/"
		for dev in range(nDevs):
			for pos in range(rings[module][dev]):
				order += str(dev)
		addOrder(order)
	busy = 0
		
def addOrder(order):
	global orders
	for i in range(len(orders) - 1, 0, -1):
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
	global noRandomCommitBefore
	if time.time() - lastRandomCommit > 10:
		lastRandomCommit = time.time()
		dev = random.randint(0,4)
		module = random.randint(0,3) 
		strength = random.randint(1,7)
		addOrder("c/" 
			+ str(dev) 
			+ str(module) 
			+ "%02d" % findDevPos(dev, module)
			+ "01"
			+ str(strength))
		noRandomCommitBefore = time.time() + 3 + 4 * (5 + strength * 8 + 30)/ 10
	
def sendCommits(dev):
	global noRandomCommitBefore
	global commits
	for newOrder in commits[int(dev)]:
		addOrder(newOrder)
	addOrder("f")
	noRandomCommitBefore = time.time() + 80
	

def ordersEmpty():
	global orders
	for i in range(len(orders)):
		if not orders[i] == 0:
			return 0
	return 1
	
		
openSerial()
addOrder("d/5")
sendRings()
	
	

	
while True:
	# Read data
	try:
		data = ser.readline()
	except serial.SerialException:
		print "exception"
		connected = 0
		openSerial()
		continue
	except TypeError:
		print "typeError"
		connected = 0
		openSerial()
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
			busy = 1
			sendCommits(data[8:])
			
	# Send next order
	if lastOrderOk == 1:
		sendNextOrder()
	# send random commit
	if ordersEmpty() and time.time() > noRandomCommitBefore and not busy:
		createCommitPerhaps()

		
	
	
