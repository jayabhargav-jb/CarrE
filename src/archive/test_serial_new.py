import serial
import time
ser_obj = serial.Serial("/dev/ttyACM0", 115200, timeout=2)
num = 0
# constant characters
start_byte = 255
end_byte = 254

a = bytearray()

# print(ser_obj.readline())
time.sleep(1)
count = 0
command = "2 20 20"
dir = 2
try:
    while True:
        num += 1
        # use sum(a) for bytearray checksum without  start and end bits
        if (count%10 == 0):
            if dir==1:
                command = "2 20 0"
                dir = 2
            elif dir==2:
                command = "1 0 20"
                dir = 1
        
        count += 1
        for number in command.split(" "):
            a += bytearray([int(number)])
            
        # print(a)
        a += bytearray([sum(a)%256])
        a += bytearray([end_byte])
        a.insert(0, start_byte)
        print(f"iteration: {num}")
        ser_obj.write(a)
        # print("after write") 
        # print(a)
        a = bytearray()
        # time.sleep(0.05)
        print(ser_obj.readline())
        # print(ser_obj.readline())
        time.sleep(0.05)
except KeyboardInterrupt or serial.SerialException:
    ser_obj.close()
    print("done")