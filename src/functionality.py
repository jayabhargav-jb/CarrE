import sys
import serial
import time
import cv2
import numpy as np

# Define current_mode globally (import it from your webpage module if needed)
current_mode = "idle"  # Start in idle mode

joy_data = (0, 0)
# Frame and movement settings
frame_center_x = 320
frame_center_y = 240
MAX_SPEED = 40
STOP_THRESHOLD_AREA = 250000
MIN_AREA = 15000

# PWM settings
MAX_PWM = 50
DEADZONE = 0.3
left_pwm = 0
right_pwm = 0

# DEBUG mode and previous input
DEBUG = True
# DEBUG = False
prev_input = ""

if not DEBUG:
    # Establish serial connection
    ser = serial.Serial("/dev/ttyACM0", 115200, timeout=1)
    # Reset the Arduino's line to ensure communication starts correctly
    ser.setDTR(False)
    time.sleep(0.1)
    ser.setDTR(True)
    time.sleep(1)

# Learn and repeat trial
learnt_arr = []
is_recording = False
playback_mode = False
playback_index = 0

def send_command(command):
    ack = b''
    ser.write(command.encode())
    # Optionally, read acknowledgment from the Arduino, if needed
    # ack = ser.readline()
    # print('Arduino sent back %s' % ack)

def remote_control(joy_data):
    global prev_input
    print(f"Remote Control - Joystick data: X={joy_data[0]}, Y={joy_data[1]}")  # Debug print

    if current_mode != "remote_control" and current_mode != "idle":
        return  # Exit if we're not in remote_control or idle mode

    x, y = joy_data
    left_pwm = 0
    dir = b'0'
    right_pwm = 0
    
    # DEADZONE implementation
    if (abs(x) < DEADZONE) and (abs(y) < DEADZONE):
        x = 0
        y = 0
    elif (abs(x) < DEADZONE):
        x = 0
    elif (abs(y) < DEADZONE):
        y = 0

    # PWM implementation
    if x <= 0:
        left_pwm = MAX_PWM * abs(y)
        right_pwm = MAX_PWM * (abs(y) + abs(x))
    elif x >= 0:
        left_pwm = MAX_PWM * (abs(y) + abs(x))
        right_pwm = MAX_PWM * abs(y)
    
    # Forward and Backward logic
    if (x > DEADZONE) or (x < -DEADZONE):
        dir = b'2'  # TURNING 

    if (y > DEADZONE):
        dir = b'2'  # FORWARD
    elif (y < -DEADZONE):
        dir = b'1'  # BACKWARD
    elif ((abs(x) + abs(y)) < 0.05):
        dir = b'0'  # STOP
    left_pwm, right_pwm = int(left_pwm), int(right_pwm)
    command = str(dir.decode()) + " " + str(left_pwm).rjust(3, '0') + " " + str(right_pwm).rjust(3, '0') + " \n"  
    print(command)
    if not DEBUG and command != prev_input:
        send_command(command)
        prev_input = command

def learn(joy_data):
    global learnt_arr
    if current_mode != "learn" and current_mode != "idle":
        return  # Exit if we're not in learn mode
    print(f"Learning - Joystick data: X={joy_data[0]}, Y={joy_data[1]}")  # Debug print
    x, y = joy_data
    left_pwm = 0
    dir = b'0'
    right_pwm = 0
    
    # DEADZONE implementation
    if (abs(x) < DEADZONE) and (abs(y) < DEADZONE):
        x = 0
        y = 0
    elif (abs(x) < DEADZONE):
        x = 0
    elif (abs(y) < DEADZONE):
        y = 0

    # PWM implementation
    if x <= 0:
        left_pwm = MAX_PWM * abs(y)
        right_pwm = MAX_PWM * (abs(y) + abs(x))
    elif x >= 0:
        left_pwm = MAX_PWM * (abs(y) + abs(x))
        right_pwm = MAX_PWM * abs(y)
    
    # Forward and Backward logic    
    if (x > DEADZONE) or (x < -DEADZONE):
        dir = b'2'  # TURNING 
    if (y > DEADZONE):
        dir = b'2'  # FORWARD
    elif (y < -DEADZONE):
        dir = b'1'  # BACKWARD
    elif ((abs(x) + abs(y)) < 0.05):
        dir = b'0'  # STOP

    # Learn Mode
    left_pwm, right_pwm = int(left_pwm), int(right_pwm)
    data = [dir, left_pwm, right_pwm]
    learnt_arr.append(data)
    command = str(dir.decode()) + " " + str(left_pwm).rjust(3, '0') + " " + str(right_pwm).rjust(3, '0') + " \n"
    print("learning:", command)

def repeat():
    global prev_input
    global learnt_arr

    command = "0 000 000"
    if len(learnt_arr) > 0:    
        data = learnt_arr.pop(0)
        command = str(data[0].decode()) + " " + str(data[1]).rjust(3, '0') + " " + str(data[2]).rjust(3, '0') + " \n"  

        print("repeating:", command)
    else:
        print("data exhausted.") 
    
    if not DEBUG and command != prev_input:
        send_command(command)
        prev_input = command

def follow_me(frame):
    # if current_mode != "follow_me":
    #     return  # Exit the function if we're not in follow_me mode
    print(f"inside follow me")
    lower_bound = np.array([35, 50, 50])  # Green color lower HSV bound
    upper_bound = np.array([85, 255, 255])  # Green color upper HSV bound

    frame_center_x = 320
    frame_center_y = 240
    MAX_SPEED = 40
    STOP_THRESHOLD_AREA = 250000
    MIN_AREA = 15000
    prev_input = ""

    if not DEBUG:
        ser = serial.Serial("/dev/ttyACM0", 115200, timeout=1)
        ser.setDTR(False)
        time.sleep(0.1)
        ser.setDTR(True)
        time.sleep(1)

    hsv_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # Green object detection
    mask = cv2.inRange(hsv_frame, lower_bound, upper_bound)
    mask = cv2.erode(mask, None, iterations=2)
    mask = cv2.dilate(mask, None, iterations=2)

    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    if contours:
        largest_contour = max(contours, key=cv2.contourArea)
        x, y, w, h = cv2.boundingRect(largest_contour)
        area = w * h

        if area < MIN_AREA:
            print("Green target too small, ignoring.")
            if not DEBUG:
                send_command("0 000 000")
            return

        center_x = x + w // 2
        error_x = center_x - frame_center_x

        forward_speed = int(MAX_SPEED * (1 - area / STOP_THRESHOLD_AREA)) if area <= STOP_THRESHOLD_AREA else 0

        speed_factor = 8
        error_x /= 100
        if abs(error_x) < 0.2:
            error_x = 0

        left_motor_speed = max(0, min(MAX_SPEED, forward_speed + error_x * speed_factor))
        right_motor_speed = max(0, min(MAX_SPEED, forward_speed - error_x * speed_factor))
        left_motor_speed, right_motor_speed = int(left_motor_speed), int(right_motor_speed)
        command = f"2 {str(left_motor_speed).rjust(3, '0')} {str(right_motor_speed).rjust(3, '0')}\n"
        print(command)

        if command != prev_input and not DEBUG:
            send_command(command)
            prev_input = command
    else:
        if not DEBUG:
            send_command("0 000 000")
