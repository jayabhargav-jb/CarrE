import Jetson.GPIO as GPIO
import time

# GPIO pin setup for the encoders
ENCODER1_A = 17  # Replace with the actual GPIO pin for Motor 1 Encoder A

ENCODER2_A = 22  # Replace with the actual GPIO pin for Motor 2 Encoder A


# Initialize pulse counters
encoder1_pulse_count = 0
encoder2_pulse_count = 0

# Callback functions for the interrupts
def encoder1_callback(channel):
    global encoder1_pulse_count
    encoder1_pulse_count += 1

def encoder2_callback(channel):
    global encoder2_pulse_count
    encoder2_pulse_count += 1

# GPIO setup
GPIO.setmode(GPIO.BCM)
GPIO.setup(ENCODER1_A, GPIO.IN, pull_up_down=GPIO.PUD_UP)

GPIO.setup(ENCODER2_A, GPIO.IN, pull_up_down=GPIO.PUD_UP)


# Set up interrupts
GPIO.add_event_detect(ENCODER1_A, GPIO.RISING, callback=encoder1_callback)
GPIO.add_event_detect(ENCODER2_A, GPIO.RISING, callback=encoder2_callback)

# Main loop
try:
    while True:
        print(f"Encoder 1 Pulse Count: {encoder1_pulse_count}")
        print(f"Encoder 2 Pulse Count: {encoder2_pulse_count}")
        time.sleep(0.5)
except KeyboardInterrupt:
    print("Exiting program.")

# Clean up GPIO
finally:
    GPIO.cleanup()
