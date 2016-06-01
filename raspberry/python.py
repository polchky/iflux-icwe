
import serial

ser = serial.Serial('/dev/ttyACM0', 9600, timeout=0)
rings = [



while 1:
	try:
		line = ser.readline()
		time.sleep(1)
	except ser.SerialTimeoutException:
		time.sleep(1)
	time.sleep(1)
