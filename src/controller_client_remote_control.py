import sys
sys.path.append('/home/carre/Desktop/CarrE/lib/python3.8/site-packages')

import socketio 
import serial
import time
import csv

f_obj = open("inputs.csv", "w", newline="\n")
csv_writer = csv.writer(f_obj)
# Create a Socket.IO client
sio = socketio.Client()
# DEBUG = True
DEBUG = False

prev_input = ""

if not DEBUG:
    # Establish serial connection
    #ser = serial.Serial("/dev/ttyUSB0", 115200, timeout=1)
    ser = serial.Serial("/dev/ttyACM0", 115200, timeout=1)
    # Reset the Arduino's line to ensure communication starts correctly
    ser.setDTR(False)
    time.sleep(0.1)
    ser.setDTR(True)
    time.sleep(1)

# print(ser.get_settings())
# ser.reset_input_buffer()
# ser.reset_output_buffer()

#learn and repeat trial
recorded_data = []
is_recording = False
playback_mode = False
playback_index = 0

def send_command(command):
    ack = b''
    ser.write(command.encode())
    # for i in range(7):
    # ack = ser.readline()
    
    # # ser.flushInput()
        
    #print('Arduino sent back %s' % ack)

# Define the event handlers
@sio.event
def connect():
    print('Connected to server')
    # # Emit a 'robotCmd' event to the server
    # sio.emit('robotCmd', {'cmd': 'move_forward'})

@sio.event
def disconnect():
    print('Disconnected from server')

@sio.on('cmdStatus')
def robotCmd(data):
    # print(data)
    global prev_input
    data = data[0]
    file_write = [data['dir'], data['lpwm'], data['rpwm']]
    csv_writer.writerow(file_write)
    command = str(data['dir'].decode())+ " " + str(data['lpwm']).rjust(3, '0') + " " + str(data['rpwm']).rjust(3, '0') + " \n"  
    
    #data_temp = str(len(command))+"cmd:" + command


    
    #learn_list.append(data_temp)
    if not DEBUG and command != prev_input:
        send_command(command)
        print(command)
        prev_input = command


# Connect to the Socket.IO server
sio.connect('http://192.168.237.240:8080')
# sio.connect('http://'+subprocess.check_output("arp | grep d0:39:57", shell = True, text = True).split()[0]+':8080')

# Wait for events
# sio.wait()
try:
    sio.wait()
except KeyboardInterrupt:
    sio.shutdown()
    if not DEBUG:
        ser.setDTR(False)
        time.sleep(0.1)
        ser.setDTR(True)
        # time.sleep(1)
        ser.close()
    f_obj.close()
    print("exiting")
