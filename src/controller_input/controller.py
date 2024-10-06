from inputs import get_gamepad
import math
import threading

from .CONSTANTS import *


class XboxController(object):
    MAX_TRIG_VAL = math.pow(2, 8)
    MAX_JOY_VAL = math.pow(2, 15)

    def __init__(self):
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
        right_pwm = 0
        
        pwm = MAX_PWM

        # Forward and Backward logic
        if y > DEADZONE:
            dir = b'2' # FORWARD
        elif y < (-DEADZONE):
            dir = b'1' # BACKWARD
        else:
            dir = b'0' # STOP

        # if (abs(x) + abs(y)) > 1:
        #     pwm = MAX_PWM/2
        # else:
        #     pwm = MAX_PWM

        # # OLD LOGIC
        # # Speed logic
        # if x > DEADZONE: # LEFT
        #     left_pwm = pwm * abs(x+y)
        #     right_pwm = pwm * abs(y)
        # elif x < (-DEADZONE): # RIGHT
        #     left_pwm = pwm * abs(y)
        #     right_pwm = pwm * abs(x+y)
        # else: # FORWARD OR BACKWARD
        #     left_pwm = pwm * abs(y)
        #     right_pwm = pwm * abs(y)
        
        # # NEW LOGIC
        if x < 0:
            left_pwm = pwm * abs(y)
            right_pwm = pwm * (abs(y)+abs(x))
        
        elif x > 0:
            left_pwm = pwm * (abs(y)+abs(x))
            right_pwm = pwm * abs(y)
        # return {
        #     'dir' : dir,
        #     'lpwm' : int(left_pwm),
        #     'rpwm' : int(right_pwm),
        #     'x' : x,
        #     'y' : y
        # }

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
                elif event.code == 'BTN_NORTH':
                    self.Y = event.state
                elif event.code == 'BTN_WEST':
                    self.X = event.state
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
    joy = XboxController()
    try:
        while True:
            print(joy.read())
    except KeyboardInterrupt:
        print("exiting.")