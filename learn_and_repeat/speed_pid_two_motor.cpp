#include "header_for_two_motor_control.h"
#include  "Arduino.h"

float Kp1 = 0.2, Ki1 = 0.01, Kd1 = 0.002;  // PID constants
float Kp2 = 0.2, Ki2 = 0.01, Kd2 = 0.002;  // PID constants

float error1=0.0,prevError1=0.0;
float integral1=0.0, derivative1=0.0;

float error2=0.0,prevError2=0.0;
float integral2=0.0, derivative2=0.0;


// Integral anti-windup
float integralMin = -30000.0, integralMax = 30000.00;

const unsigned int pulsesPerRevolution = 7;  // Encoder gives 7 pulses per revolution
// Variables to handle timing
const unsigned long interval = 50;  
unsigned long int multiplier_constant=(60000/interval);
float  rpm_m1=0.0;
float output1=0.0;

//float actualSpeed1, actualSpeed2;  // Feedback from encoders
float  rpm_m2=0.0;
float output2=0.0;

unsigned int out1=0;
unsigned int out2=0;

unsigned long int lastpulseCount1 = 0;
unsigned long int pulseCountdiff_1;
float prevrpm1=0.0;

unsigned long int lastpulseCount2 = 0;
unsigned long int pulseCountdiff_2;
float prevrpm2=0.0;
unsigned long previousTime=0;
int key1=-1;
int key2=-1;
int key3=-1;

float* pid_control::speed_calculation(unsigned long int pulse_Counter1, unsigned long int pulse_Counter2) {
  pulseCountdiff_1 = (pulse_Counter1 - lastpulseCount1);
  pulseCountdiff_2 = (pulse_Counter2 - lastpulseCount2);
  
  rpm_m1 = (pulseCountdiff_1 * multiplier_constant) / (pulsesPerRevolution * 19.1);
  rpm_m2 = (pulseCountdiff_2 * multiplier_constant) / (pulsesPerRevolution * 19.1);
  // Serial.print(rpm_m1);
  // Serial.print(",");
  // Serial.println(rpm_m2);
  if (lastpulseCount1 > pulse_Counter1) {
    rpm_m1 = prevrpm1;
  }
  if (lastpulseCount2 > pulse_Counter2) {
    rpm_m2 = prevrpm2;
  }

  lastpulseCount1 = pulse_Counter1;
  prevrpm1 = rpm_m1;

  lastpulseCount2 = pulse_Counter2;
  prevrpm2 = rpm_m2;
  //Serial.print(rpm1);
  //Serial.println(rpm2);
  rpms_arr[0]=rpm_m1;
  rpms_arr[1]=rpm_m2; 

  return rpms_arr;
}

//#ifdef PID_INCLUDED
int* pid_control::PID_calculations(float rpm_1, float rpm_2,float setpoint_1,float setpoint_2) {
  
  
  unsigned long currentTime = millis();
  unsigned long deltaTime = (currentTime - previousTime);
  
  error1 = (setpoint_1) - (rpm_1);
  error2 = (setpoint_2) - (rpm_2);

  integral1 += (error1 * deltaTime);
  integral2 += (error2 * deltaTime);

  if (integral1 > integralMax) integral1 = integralMax;
  if (integral1 < integralMin) integral1 = integralMin;

  if (integral2 > integralMax) integral2 = integralMax;
  if (integral2 < integralMin) integral2 = integralMin;

  derivative1 = ((error1 - prevError1) / (deltaTime));
  derivative2 = ((error2 - prevError2) / (deltaTime));

  output1 = (Kp1 * error1) + (Ki1 * integral1) + (Kd1 * derivative1);
  output2 = (Kp2 * error2) + (Ki2 * integral2) + (Kd2 * derivative2);

  out1 = (int)constrain(output1, 0, 100);
  out2 = (int)constrain(output2, 0, 100);

  prevError1 = error1;
  prevError2 = error2;
  previousTime = currentTime;

  outputs_arr[0]=out1;
  outputs_arr[1]=out2;
  return outputs_arr;
}

void pid_control::learn_phase(float rpm_1, float rpm_2, int dir) {
    if (key1 < 500) {  // Ensure key stays within bounds
        //Serial.print("Hi");
        arr.rpm_1[++key1] = rpm_1;
        arr.rpm_2[++key2] = rpm_2;
        arr.dir[++key3] = dir;
        
        Serial.println(key1);
        
    } else {
        Serial.println("Array is full");
    }
}

float* pid_control::repeat_phase() {
    if (key1 > 0) {  // Ensure key stays within bounds
        //key--;  // Decrement first before accessing
        //Serial.print("the values is:");
        //Serial.println(key1);
        

        repeat_arr[0] = arr.rpm_1[--key1];
        repeat_arr[1] = arr.rpm_2[--key2];
        repeat_arr[2] = arr.dir[--key3];
        Serial.println(key1);
        // Serial.print(arr.rpm_1[--key1]);
        // Serial.print(",");
        // Serial.println(arr.rpm_1[--key2]);
        // Serial.print(",");
        // Serial.println(arr.rpm_1[--key3]);
    } else {
        Serial.println("Array is empty");
    }
    return repeat_arr;
}
//#endif