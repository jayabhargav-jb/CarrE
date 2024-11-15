#include "header_for_two_motor_control.h"

#define PID_INCLUDED 1

// Interrupt service routine (ISR) for Motor 1
void pulseCounter1() {
  pulseCount1++;
}
void pulseCounter2() {
  pulseCount2++;
}


float speed_calculation(unsigned long int pulse_Counter1,unsigned long int pulse_Counter2){

    // Serial.print(lastpulseCount);
    // Serial.print(" ");
    // Serial.print(pulseCounter1);
    // Serial.print(" ");
    pulseCountdiff_1 = (pulse_Counter1-lastpulseCount1);
    pulseCountdiff_2 = (pulse_Counter2-lastpulseCount2);
    // Serial.print(pulseCount);
    // Serial.print(" ");
  // Calculate RPM for Motor 1
    rpm1 = (pulseCountdiff_1*multiplier_constant) / (pulsesPerRevolution*19.1);
    rpm2 = (pulseCountdiff_2*multiplier_constant) / (pulsesPerRevolution*19.1);

    if(lastpulseCount1>pulseCount1){
      rpm1=prevrpm1;
    }
    if(lastpulseCount2>pulseCount2){
      rpm2=prevrpm2;
    }

    lastpulseCount1 = pulseCount1;
    prevrpm1=rpm1;

    lastpulseCount2 = pulseCount2;
    prevrpm2=rpm2;
    //Serial.print("RPM: ");
    
    // Serial.print(rpm1);
    // Serial.print(",");
    // Serial.println(rpm2);
    return rpm1,rpm2;

}

#ifdef PID_INCLUDED

int PID_calculations(float rpm1, float rpm2){
  unsigned long currentTime=millis();
  
  unsigned long deltaTime=(currentTime-previousTime);
  error1 = (setpoint1) - (rpm1);
  error2 = (setpoint2) - (rpm2);
  // Integral term
  integral1 += (error1*deltaTime);
  integral2 += (error2*deltaTime);

  
  if (integral1 > integralMax) integral1 = integralMax;
  if (integral1 < integralMin) integral1 = integralMin;

  if (integral2 > integralMax) integral2 = integralMax;
  if (integral2 < integralMin) integral2 = integralMin;

  // Derivative term
  derivative1 = ((error1 - prevError1)/(deltaTime));
  derivative2 = ((error2 - prevError2)/(deltaTime));

  // PID output
  output1 = (Kp1 * error1) + (Ki1 * integral1) + (Kd1 * derivative1);
  output2 = (Kp2 * error2) + (Ki2 * integral2) + (Kd2 * derivative2);
  out1=(int)(output1);
  out2=(int)(output2);

  // if((out1>255)){
  //   out1=255;
  // }
  // if((out2>255)){
  //   out2=255;
  // }
  // if((out1<0)){
  //   out1=0;
  // }
  // if((out2<0)){
  //   out2=0;
  // }

  // Clamp outputs to PWM limits
  out1 = (int)constrain(output1, 0, 100);
  out2 = (int)constrain(output2, 0, 100);

  //if(integral1>30.0){
  //integral1=27.94;
  //}
  prevError1 = error1;
  prevError2 = error2;
  previousTime=currentTime;

  return out1,out2;
}
#endif

void setup() {
  Serial.begin(115200);  // Start serial communication at 9600 baud rate
  cmd_val[9] = '\0';
  pinMode(enable_1r,OUTPUT);
  pinMode(enable_1l,OUTPUT);
  pinMode(enable_2r,OUTPUT);
  pinMode(enable_2l,OUTPUT);
  
  // Set encoder pins as inputs
  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);
  pinMode(pwm_r1, OUTPUT);
  pinMode(pwm_l2, OUTPUT);
  pinMode(pwm_r2, OUTPUT);
  pinMode(pwm_l1, OUTPUT);
  // setting the pins high
  digitalWrite(enable_1r,HIGH);
  digitalWrite(enable_1l,HIGH);
  digitalWrite(enable_2r,HIGH);
  digitalWrite(enable_2l,HIGH);

  
  // Attach interrupts to the encoder pins
  attachInterrupt(digitalPinToInterrupt(encoderPin1), pulseCounter1, RISING);  // Motor 1 encoder
  attachInterrupt(digitalPinToInterrupt(encoderPin2), pulseCounter2, RISING); 
  //analogWrite(pwm_r1, 50);
  //analogWrite(pwm_r1, 10);
  //delay(2000);

}

void loop() {
  
  unsigned long currentMillis = millis();
  //while (Serial.available() == 0) {

  //setpoint1 = Serial.parseInt();
  
  //Serial.println(setpoint1);
//}
  // Check if 1 second (1000 ms) has passed
  // checks the speed and also calculates the error and finds the pwm required
  
  if (Serial.available() > 8) {
    Serial.readBytesUntil('\n', cmd_val, 9); // Read until newline
    cmd_val[9] = '\0'; // Ensure null termination
    // Serial.print("Received: ");
    // Serial.println(cmd_val);
    // Parsing the values from the string
   char* token = strtok(cmd_val, " "); // Split by space
   if (token) {
      dir = atoi(token); // Convert first token to integer
      token = strtok(NULL, " "); // Move to next token
     }
   if (token) {
      setpoint1 = atoi(token); // Convert second token to integer
      token = strtok(NULL, " "); // Move to next token
     }
   if (token) {
      setpoint2 = atoi(token); // Convert third token to integer
     }
     //Serial.flush();
  }
    


  if ((currentMillis-previousMillis) >= interval) {
    //Serial.print("1");
    rpm1,rpm2=speed_calculation(pulseCount1,pulseCount2);
#ifdef PID_INCLUDED
    out1,out2=PID_calculations(rpm1,rpm2);
#else
    out1=30*1.5;
    out2=30*1.5;
#endif
    //Serial.print(out1);
    //Serial.print(",");
    //Serial.println(out2);
    pwmforconversion_1=(out1/1.5);
    pwmforconversion_2=(out2/1.5);
    // if ((out1>50)||(out2>50)){       //look into this for the end
    //   out1=50;
    //   out2=50;

    // }
    
    //analogWrite(pwm_r1,pwmforconversion_1);
    //analogWrite(pwm_l2,pwmforconversion_2);
    if (dir==1) {
       analogWrite(pwm_l1, pwmforconversion_1);
       analogWrite(pwm_r1, 0);
       analogWrite(pwm_l2, 0);
       analogWrite(pwm_r2, pwmforconversion_2);
     }
     else if (dir==2) {
       analogWrite(pwm_l1, 0);
       analogWrite(pwm_r1, pwmforconversion_1);
       analogWrite(pwm_l2, pwmforconversion_2);
       analogWrite(pwm_r2, 0);
     }
     else {
       analogWrite(pwm_l1, 0);
       analogWrite(pwm_r1, 0);
       analogWrite(pwm_l2, 0);
       analogWrite(pwm_r2, 0);
     }


    
    // Print Motor 1 RPM to the Serial Monitor
    //Serial.print("Motor 1 Speed: ");
    
    // Serial.print(",");
    // Serial.print("integral ");
    // Serial.print(integral1);
    // Serial.print("error ");
    // Serial.print(error1);
    // Serial.println(",");
    
    

     previousMillis = currentMillis;  //assigning previous time to the current time 
    
    // pulseCount1 = 0;   // making the count of the pulse count1 and pulse 2 count =0
  
    
  }
  
}

