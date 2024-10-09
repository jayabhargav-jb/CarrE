#import "header_for_two_motor_control.h"
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
    // Serial.print("RPM: ");
    //Serial.println(rpm1);
    return rpm1,rpm2;

}


int PID_calculations(float rpm1, float rpm2){
   error1 = (setpoint1) - (rpm1);
   error2 = (setpoint2) - (rpm2);
   // Integral term
   integral1 += error1;
   integral2 += error2;
    // Derivative term
   derivative1 = error1 - prevError1;
   derivative2 = error2 - prevError2;

    // PID output
   output1 = Kp1 * error1 + Ki1 * integral1 + Kd1 * derivative1;
   output2 = Kp2 * error2 + Ki2 * integral2 + Kd2 * derivative2;
   out1=(int)(output1);
   out2=(int)(output2);
   if((out1>255)||(out2>255)){
     out1=255;
     out2=255;
   }
   if((out1<0)||(out2<0)){
     out1=0;
     out2=0;
   }
   //if(integral1>30.0){
    //integral1=27.94;
   //}
   prevError1 = error1;
   prevError2 = error2;
   return out1,out2;
}

void setup() {
  Serial.begin(9600);  // Start serial communication at 9600 baud rate
  pinMode(enable_1r,OUTPUT);
  pinMode(enable_1l,OUTPUT);
  pinMode(enable_2r,OUTPUT);
  pinMode(enable_2l,OUTPUT);
  //pinMode(gpio1,OUTPUT);
  // Set encoder pins as inputs
  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);
  pinMode(pwm_r1, OUTPUT);
  pinMode(pwm_l2, OUTPUT);
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
  
  

  // Check if 1 second (1000 ms) has passed
  // checks the speed and also calculates the error and finds the pwm required
  
  if ((currentMillis - previousMillis) >= interval) {
    //digitalWrite(gpio1,HIGH);
    rpm1,rpm2=speed_calculation(pulseCount1,pulseCount2);
    out1,out2=PID_calculations(rpm1,rpm2);

    if ((out1>50)||(out2>50)){
      out1=50;
      out2=50;

    }
    analogWrite(pwm_r1,out1);
    analogWrite(pwm_l2,out2);
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
  //digitalWrite(gpio1,LOW);
  
}
