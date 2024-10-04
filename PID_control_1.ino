#import "header_forpid.h"
// Interrupt service routine (ISR) for Motor 1
void pulseCounter1() {
  pulseCount1++;
}

// Interrupt service routine (ISR) for Motor 2
void pulseCounter2() {
  pulseCount2++;
}


float speed_calculation(int pulseCounter1, int pulseCounter2){
  // Calculate RPM for Motor 1
    rpm1 = (pulseCount1*60) / (pulsesPerRevolution*19.1);

    // Calculate RPM for Motor 2
    rpm2 =  (pulseCount2*60) / (pulsesPerRevolution*19.1);

    return rpm1,rpm2;

}


int PID_calculations(float rpm1, float rpm2){
   error1 = setpoint1 - rpm1;
   error2 = setpoint2 - rpm2;

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

   if(out1>255 || out2>255){
     out1,out2=255;
   }
   if(out1<0 | out2<0){
     out1,out2=0;
   }
   prevError1 = error1;
   prevError2 = error2;

   return out1,out2;
}

void setup() {
  Serial.begin(9600);  // Start serial communication at 9600 baud rate

  // Set encoder pins as inputs
  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);
  pinMode(pwm_r1, OUTPUT);
  pinMode(pwm_l1, OUTPUT);

  // Attach interrupts to the encoder pins
  attachInterrupt(digitalPinToInterrupt(encoderPin1), pulseCounter1, RISING);  // Motor 1 encoder
  attachInterrupt(digitalPinToInterrupt(encoderPin2), pulseCounter2, RISING);  // Motor 2 encoder
}

void loop() {
  unsigned long currentMillis = millis();
  //int pwmr = 30;
  //int pwml = 30;

  
  // Check if 1 second (1000 ms) has passed
  // checks the speed and also calculates the error and finds the pwm required
  if (currentMillis - previousMillis >= interval) {
    rpm1,rpm2=speed_calculation(pulseCounter1,pulseCounter2);
    out1,out2=PID_calculations(rpm1,rpm2);

    analogWrite(pwm_r1,out1);
    analogWrite(pwm_l1,out2);

    // Print Motor 1 RPM to the Serial Monitor
    //Serial.print("Motor 1 Speed: ");
    Serial.print("RPM's: ");
    Serial.print(rpm1);
    //Serial.println(" RPM");
    Serial.print(",");
    // Print Motor 2 RPM to the Serial Monitor
    //Serial.print("Motor 2 Speed: ");
    Serial.println(rpm2);
    Serial.println(integral1);

    previousMillis = currentMillis;  //assigning previous time to the current time 
    
    pulseCount1 = 0;   // making the count of the pulse count1 and pulse 2 count =0
    pulseCount2 = 0;

  }
  
}