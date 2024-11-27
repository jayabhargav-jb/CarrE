import Jetson.GPIO as GPIO
import time

inp_pin = 16
GPIO.setmode(GPIO.BCM)  # BCM pin-numbering scheme from Raspberry Pi
GPIO.setup(inp_pin, GPIO.IN)  # set pin as an input pin
print("Starting demo now! Press CTRL+C to exit")
prev_value = 0
try:
    while True:
        value = GPIO.input(inp_pin)
        # if value != prev_value:
        if value == GPIO.HIGH:
            value_str = "HIGH"
            print("1")
        elif value == GPIO.LOW:
            value_str = "LOW"
            print("0")
            # print("Value read from pin {} : {}".format(12, value_str))
            # prev_value = value
        time.sleep(0.2)
finally:
    GPIO.cleanup()