import serial
import time

ser = serial.Serial("/dev/ttyACM0", 9600, timeout=1)
print(ser.get_settings())
ser.reset_input_buffer()
ser.reset_output_buffer()

# Reset the Arduino's line to ensure communication starts correctly
ser.setDTR(False)
time.sleep(0.5)
ser.flushInput()
ser.setDTR(True)
time.sleep(0.5)

test_arr = ["0 0 0", "1 50 50", "1 25 25", "0 0 0"]

def send_command(command):
    ack = b''
    ser.write(command.encode())

for a in test_arr:
    send_command(a)
    time.sleep(1)