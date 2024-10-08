#ifndef _header_forpid_H
#define _header_forpid_H

#define pwm_r1 11
float Kp1 = 0.7, Ki1 = 0.01, Kd1 = 0.00105;  // PID constants
float error1=0.0,prevError1=0.0;
float integral1=0.0, derivative1=0.0;
float setpoint1 = 25.0;  // Desired speeds  
//float actualSpeed1, actualSpeed2;  // Feedback from encoders
float  rpm1=0.0;
float output1=0.0;
const int encoderPin1 = 2;  // Encoder 1 signal pin (must be an interrupt pin)

// Volatile variables to store the pulse counts for each motor
volatile unsigned int pulseCount1 = 0;  // Pulse count for Motor 1

const unsigned int pulsesPerRevolution = 7;  // Encoder gives 7 pulses per revolution

// Variables to handle timing
unsigned long previousMillis = 0;  // To track time
const unsigned long interval = 50;  // Interval to calculate RPM (1 second)
unsigned long int multiplier_constant=(60000/interval); 

unsigned int out1=0;


#endif // _header_forpid_H
