import cv2
import numpy as np
import serial
import time
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

def send_command(command):
    ack = b''
    ser.write(command.encode())
    time.sleep(0.1)
    print("in send command")


# Define the color in HSV for Green
# Adjust the HSV range to capture a wider range of green shades
lower_bound = np.array([35, 50, 50])  # Lower HSV bound for green
upper_bound = np.array([85, 255, 255])  # Upper HSV bound for green

# Initialize OpenCV webcam
cap = cv2.VideoCapture(0)

# Check if the webcam opened successfully
if not cap.isOpened():
    print("Error: Could not open webcam.")
    exit()

# Center of the frame (assumes 640x480 resolution, adjust if necessary)
frame_center_x = 320
frame_center_y = 240

# Maximum speed of the motors (100 is arbitrary, adjust for your robot)
MAX_SPEED = 25

# Set the threshold for stopping the robot if the object is too close
STOP_THRESHOLD_AREA = 250000  # Threshold for object area (larger value = closer object)

# Set the minimum area to consider an object (ignore very small areas)
MIN_AREA = 15000  # Minimum area of the object to be considered valid
try:
    while True:
        print("executing")
        ret, frame = cap.read()
        if not ret:
            print("Error: Failed to capture image.")
            break

        # Show the raw frame to verify that the webcam is working
        # cv2.imshow("Raw Frame", frame)

        # Convert the frame to HSV color space
        hsv_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

        # Create a mask for the target color (green)
        mask = cv2.inRange(hsv_frame, lower_bound, upper_bound)
        
        # Check if the mask has valid values
        if np.sum(mask) == 0:
            print("No green target detected in the frame.")
        
        # Show the mask to verify the color detection
        # cv2.imshow("Mask", mask)

        # Optionally apply some morphological operations to clean up the mask
        mask = cv2.erode(mask, None, iterations=2)
        mask = cv2.dilate(mask, None, iterations=2)

        result = cv2.bitwise_and(frame, frame, mask=mask)

        # Find contours of the target color
        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        if contours:
            # Get the largest contour (we assume this is the target)
            largest_contour = max(contours, key=cv2.contourArea)
            # Calculate the bounding box for the contour
            x, y, w, h = cv2.boundingRect(largest_contour)
            # Calculate the area of the bounding box to estimate distance
            area = w * h
            print("Largest area:", area)

            if (area)<15000:
                print("skipping")
                if not DEBUG:
                    send_command("0 000 000")
                continue

            # Calculate the center of the bounding box
            center_x = x + w // 2
            center_y = y + h // 2

            # Calculate the error between the target's center and the center of the frame
            error_x = center_x - frame_center_x


            # Adjust the forward/backward speed based on the area
            # Larger area = closer, smaller area = farther
            if area < MIN_AREA:
                forward_speed = MAX_SPEED  # Go forward if the object is far
            else:
                # Slow down or stop if the object is too close
                if area > STOP_THRESHOLD_AREA:
                    forward_speed = 0  # Stop if the object is too close
                else:
                    # Otherwise, move forward with speed adjusted based on area
                    forward_speed = int(MAX_SPEED * (1 - area / STOP_THRESHOLD_AREA))

            # Calculate motor speeds
            speed_factor = 10  # Proportional factor for horizontal movement correction
            error_x = error_x/100
            if abs(error_x)<1:
                error_x = 0
            print(f"error x: {error_x}")
            left_motor_speed = forward_speed + error_x * speed_factor
            right_motor_speed = forward_speed - error_x * speed_factor

            print(f"Left Motor Speed: {int(left_motor_speed)}")
            print(f"Right Motor Speed: {int(right_motor_speed)}")

            # Ensure the speeds are clamped to a valid range
            left_motor_speed = max(0, min(MAX_SPEED, left_motor_speed))
            right_motor_speed = max(0, min(MAX_SPEED, right_motor_speed))

            # Print direction and motor speeds
            print(f"Direction: {'Forward' if forward_speed > 0 else 'Stop'}")
            print(f"Forward Speed: {forward_speed}")
            print(f"Left Motor Speed: {int(left_motor_speed)}")
            print(f"Right Motor Speed: {int(right_motor_speed)}")
            command = str(2)+ " " + str(left_motor_speed).rjust(3, '0') + " " + str(right_motor_speed).rjust(3, '0') + " \n"  
            if command != prev_input and not DEBUG:
                send_command(command)
                # time.sleep(0.1)
                prev_input = command
        
        else:
            command = "0 000 000"
            if not DEBUG:
                send_command(command)
        # Show the frame with detected color
        # cv2.imshow("Result", result)

        # Break the loop if 'q' is pressed
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
except KeyboardInterrupt:
    if not DEBUG:
        send_command("0 000 000 \n")
        ser.close()

cap.release()
cv2.destroyAllWindows()
