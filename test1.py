import serial
import time

ser = serial.Serial('/dev/ttyUSB0', baudrate=9600)
ser.rts = True  # Relay ON
# ser.dsr = True

time.sleep(15)
ser.rts = False  # Relay OFF
# ser.dsr = False

time.sleep(15)
ser.close()
