#ifndef _header_forpid_H
#define _header_forpid_H

#define pwm_r1 11
#define pwm_l1 10
float Kp1 = 0.0016, Ki1 = 0.0, Kd1 = 0.0;  // PID constants
float Kp2 = 0.28, Ki2 = 29.0, Kd2 = 0.00024;
float error1=0.0, error2=0.0, prevError1=0.0, prevError2=0.0;
float integral1=0.0, integral2=0.0, derivative1=0.0, derivative2=0.0;
float setpoint1 = 10, setpoint2 = 0;  // Desired speeds
//float actualSpeed1, actualSpeed2;  // Feedback from encoders
float  rpm1=0,rpm2=0;
float output1, output2;
const int encoderPin1 = 2;  // Encoder 1 signal pin (must be an interrupt pin)
const int encoderPin2 = 3;  // Encoder 2 signal pin (must be an interrupt pin)

// Volatile variables to store the pulse counts for each motor
volatile int pulseCount1 = 0;  // Pulse count for Motor 1
volatile int pulseCount2 = 0;  // Pulse count for Motor 2

const int pulsesPerRevolution = 7;  // Encoder gives 7 pulses per revolution

// Variables to handle timing
unsigned long previousMillis = 0;  // To track time
const unsigned long interval = 1000;  // Interval to calculate RPM (1 second)
unsigned int out1=0;
unsigned int out2=0;



#endif // _header_forpid_H
