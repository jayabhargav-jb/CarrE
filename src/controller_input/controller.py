from inputs import get_gamepad
import math
import threading

class XboxController(object):
    MAX_TRIG_VAL = math.pow(2, 8)
    MAX_JOY_VAL = math.pow(2, 15)

    def __init__(self,sio_client):
        self.sio = sio_client
        self.LeftJoystickY = 0
        self.LeftJoystickX = 0
        self.RightJoystickY = 0
        self.RightJoystickX = 0
        self.LeftTrigger = 0
        self.RightTrigger = 0
        self.LeftBumper = 0
        self.RightBumper = 0
        self.A = 0
        self.X = 0
        self.Y = 0
        self.B = 0
        self.LeftThumb = 0
        self.RightThumb = 0
        self.Back = 0
        self.Start = 0
        self.LeftDPad = 0
        self.RightDPad = 0
        self.UpDPad = 0
        self.DownDPad = 0

        self._monitor_thread = threading.Thread(target=self._monitor_controller)
        self._monitor_thread.daemon = True
        self._monitor_thread.start()

    def read(self):
        x = self.LeftJoystickX
        y = self.LeftJoystickY

        left_pwm = 0
        dir = b'0'
        right_pwm = 0
        
        # # NEW LOGIC
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
            right_pwm = MAX_PWM * (abs(y)+abs(x))
        elif x >= 0:
            left_pwm = MAX_PWM * (abs(y)+abs(x))
            right_pwm = MAX_PWM * abs(y)
        
        # Forward and Backward logic
        if (y > DEADZONE) or (x > DEADZONE):
            dir = b'2' # FORWARD
        elif (y < -DEADZONE) or (x < -DEADZONE):
            dir = b'1' # BACKWARD
        elif((abs(x)+abs(y))<0.05):
            dir = b'0' # STOP

        return {
            'dir' : dir,
            'lpwm' : int(left_pwm),
            'rpwm' : int(right_pwm)
        }

    def _monitor_controller(self):
        while True:
            events = get_gamepad()
            for event in events:
                if event.code == 'ABS_Y':
                    self.LeftJoystickY = event.state / XboxController.MAX_JOY_VAL
                elif event.code == 'ABS_X':
                    self.LeftJoystickX = event.state / XboxController.MAX_JOY_VAL
                elif event.code == 'ABS_RY':
                    self.RightJoystickY = event.state / XboxController.MAX_JOY_VAL
                elif event.code == 'ABS_RX':
                    self.RightJoystickX = event.state / XboxController.MAX_JOY_VAL
                elif event.code == 'ABS_Z':
                    self.LeftTrigger = event.state / XboxController.MAX_TRIG_VAL
                elif event.code == 'ABS_RZ':
                    self.RightTrigger = event.state / XboxController.MAX_TRIG_VAL
                elif event.code == 'BTN_TL':
                    self.LeftBumper = event.state
                elif event.code == 'BTN_TR':
                    self.RightBumper = event.state
                elif event.code == 'BTN_SOUTH':
                    self.A = event.state
                    #learn and repeat trial 
                    if self.A:
                        self.sio.emit('button_press', {'button': 'A'})
                elif event.code == 'BTN_NORTH':
                    self.Y = event.state
                elif event.code == 'BTN_WEST':
                    self.X = event.state
                    #learn and repeat trial
                    if self.X:
                        self.sio.emit('button_press', {'button': 'X'})

                elif event.code == 'BTN_EAST':
                    self.B = event.state
                elif event.code == 'BTN_THUMBL':
                    self.LeftThumb = event.state
                elif event.code == 'BTN_THUMBR':
                    self.RightThumb = event.state
                elif event.code == 'BTN_SELECT':
                    self.Back = event.state
                elif event.code == 'BTN_START':
                    self.Start = event.state
                elif event.code == 'BTN_TRIGGER_HAPPY1':
                    self.LeftDPad = event.state
                elif event.code == 'BTN_TRIGGER_HAPPY2':
                    self.RightDPad = event.state
                elif event.code == 'BTN_TRIGGER_HAPPY3':
                    self.UpDPad = event.state
                elif event.code == 'BTN_TRIGGER_HAPPY4':
                    self.DownDPad = event.state

if __name__ == "__main__":

    #learn and repeat trial
   # import socketio
    #sio = socketio.Client()
    #sio.connect('http://192.168.237.42:8080')
    #controller = XboxController(sio)  # Pass sio as an argument

    #from CONSTANTS import *
    joy = XboxController()
    try:
        while True:
            print(joy.read())
    except KeyboardInterrupt:
        #sio.disconnect()
        print("exiting.")
        

else:
    from .CONSTANTS import *