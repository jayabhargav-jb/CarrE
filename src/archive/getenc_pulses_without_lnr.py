import RPi.GPIO as GPIO
import time

# GPIO pin configuration
ENCODER_PIN = 16 # Replace with the GPIO pin connected to the encoder

# Pulse counter
pulse_count = 0

# Callback function for interrupt
def encoder_callback(channel):
    global pulse_count
    pulse_count += 1  # Increment pulse count on each rising edge

# GPIO setup
GPIO.setmode(GPIO.BCM)  # Use Broadcom SOC channel numbering
GPIO.setup(ENCODER_PIN, GPIO.IN)  # Set pin as input with pull-up resistor

# Configure the pin as an interrupt
GPIO.add_event_detect(ENCODER_PIN, GPIO.RISING, callback=encoder_callback)  # Trigger on rising edge
# count = 0
# Main loop
try:
    # print("Reading encoder pulses. Press Ctrl+C to exit.")
    while True:
        # Print the pulse count to the terminal
        print(f"Pulse Count: {pulse_count}")
        time.sleep(0.1)  # Update every second
        # count += 1
except KeyboardInterrupt:
    print("\nExiting program.")

finally:
    GPIO.cleanup()  # Clean up GPIO settings
