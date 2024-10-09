#ifndef _header_forpid_H
#define _header_forpid_H

#define pwm_r1 9
#define pwm_l1 10
#define pwm_r2 11
#define pwm_l2 12
#define enable_1r 4
#define enable_1l 5
#define enable_2r 6
#define enable_2l 7
//#define gpio1 8
void pulseCounter1();
void pulseCounter2();
float speed_calculation(unsigned long int pulse_Counter1,unsigned long int pulse_Counter2);
int PID_calculations(float rpm1, float rpm2);

float Kp1 = 0.7, Ki1 = 0.02, Kd1 = 0.0013;  // PID constants
float Kp2 = 0.7, Ki2 = 0.02, Kd2 = 0.0013;  // PID constants

float error1=0.0,prevError1=0.0;
float integral1=0.0, derivative1=0.0;
float setpoint1 = 25.0;  // Desired speeds  

float error2=0.0,prevError2=0.0;
float integral2=0.0, derivative2=0.0;
float setpoint2 = 25.0;  // Desired speeds  

//float actualSpeed1, actualSpeed2;  // Feedback from encoders
float  rpm1=0.0;
float output1=0.0;
const int encoderPin1 = 21;  // Encoder 1 signal pin 

//float actualSpeed1, actualSpeed2;  // Feedback from encoders
float  rpm2=0.0;
float output2=0.0;
const int encoderPin2 = 2;  // Encoder 2 signal pin

// Volatile variables to store the pulse counts for each motor
volatile unsigned int pulseCount1 = 0;  // Pulse count for Motor 1
volatile unsigned int pulseCount2 = 0;  // Pulse count for Motor 1

unsigned int out1=0;
unsigned int out2=0;

unsigned long int lastpulseCount1 = 0;
unsigned long int pulseCountdiff_1;
float prevrpm1=0.0;

unsigned long int lastpulseCount2 = 0;
unsigned long int pulseCountdiff_2;
float prevrpm2=0.0;

const unsigned int pulsesPerRevolution = 7;  // Encoder gives 7 pulses per revolution
// Variables to handle timing
unsigned long previousMillis = 0;  // To track time
const unsigned long interval = 50;  // Interval to calculate RPM (1 second)
unsigned long int multiplier_constant=(60000/interval); 


//char* cmd_val = (char*)malloc(10 * sizeof(char)); 
// variables
//int dir, lpwm, rpwm;
//int pwm;
//int last_pwm_r = 0;
//int last_pwm_l = 0;
#endif // _header_forpid_H
