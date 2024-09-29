#!/usr/bin/python3.8
import serial
import time
from controller_input.controller import XboxController

# Establish a connection with the Arduino
ser = serial.Serial("COM9", 115200, timeout=1)

joy = XboxController()

# Reset the Arduino's line to ensure communication starts correctly
ser.setDTR(False)
time.sleep(1)
ser.flushInput()
ser.setDTR(True)
time.sleep(2)

def send_command(command):
    ack = b''
    ser.write(command.encode())
    for i in range(7):
        ack += ser.read()
        
    print('Arduino sent back %s' % ack)



while True:
    # Take input from the user   
    try:
        data = joy.read()
        command = str(data['dir'].decode()) + str(data['lpwm']).rjust(3, '0') + str(data['rpwm']).rjust(3,'0')
        send_command(command)
        time.sleep(0.3)
    except KeyboardInterrupt:
        print("Exiting...")
        ser.close()
        break
      