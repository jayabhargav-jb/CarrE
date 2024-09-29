import socketio
import subprocess 
# import serial
# Create a Socket.IO client
sio = socketio.Client()
max_pwm = 255

# Establish serial connection
# ser = serial.Serial("/dev/ttyUSB0", 115200, timeout=1)

# Define the event handlers
@sio.event
def connect():
    print('Connected to server')
    # Emit a 'robotCmd' event to the server
    # sio.emit('robotCmd', {'cmd': 'move_forward'})

@sio.event
def disconnect():
    print('Disconnected from server')

@sio.on('cmdStatus')
def robotCmd(data):
    # print(data)
    
    command = 'f 0' 
    deadzone = 0.2
    x = data['x']
    y = data['y']
    abs_x = abs(x)
    abs_y = abs(y)
    if abs_x < deadzone:
        x = 0
    if abs_y < deadzone:
        y = 0
    
    print(x, y)
    # if abs(x)>abs(y):
    #     if x>=0:
    #         pwm = x * max_pwm
    #         command = 'r '+str(pwm)
    #     else:
    #         x = abs(x)
    #         pwm = x * max_pwm
    #         commmand = 'l '+str(pwm)
    
    # elif abs(x)<abs(y):
    #     if y>=0:
    #         pwm = y * max_pwm
    #         command = 'f '+str(pwm)
    #     elif y<0:
    #         y = abs(y)
    #         pwm = y * max_pwm
    #         commmand = 'b '+str(pwm)


    if abs(x) > abs(y):
        if x > 0:
            pwm = int(x * max_pwm)
            command = f'r {pwm}'
        elif x < 0:
            pwm = int(abs(x) * max_pwm)
            command = f'l {pwm}'
    elif abs(x) < abs(y):
        if y > 0:
            pwm = int(y * max_pwm)
            command = f'f {pwm}'
        elif y < 0:
            pwm = int(abs(y) * max_pwm)
            command = f'b {pwm}'
    
    # print(command)
    
    # ser.write(command.encode())
    # ack = ser.read()
    # if ack:
    #     return
    # print('Arduino sent back %s' % ack)

# Connect to the Socket.IO server
sio.connect('http://localhost:8080')
# sio.connect('http://'+subprocess.check_output("arp | grep d0:39:57", shell = True, text = True).split()[0]+':8080')

# Wait for events
# sio.wait()
try:
    sio.wait()
except KeyboardInterrupt:
    print("exiting")