#import "header_forpid.h"
// Interrupt service routine (ISR) for Motor 1
void pulseCounter1() {
  pulseCount1++;
}

unsigned long int lastpulseCount = 0;
unsigned long int pulseCount;
float speed_calculation(unsigned long int pulse_Counter1){

    // Serial.print(lastpulseCount);
    // Serial.print(" ");
    // Serial.print(pulseCounter1);
    // Serial.print(" ");
    pulseCount = (pulse_Counter1-lastpulseCount);

    // Serial.print(pulseCount);
    // Serial.print(" ");
  // Calculate RPM for Motor 1
    rpm1 = (pulseCount*multiplier_constant) / (pulsesPerRevolution*19.1);

    lastpulseCount = pulseCount1;
    // Serial.print("RPM: ");
    //Serial.println(rpm1);
    return rpm1;

}


int PID_calculations(float rpm1){
   error1 = (setpoint1) - (rpm1);
   // Integral term
   integral1 += error1;

    // Derivative term
   derivative1 = error1 - prevError1;

    // PID output
   output1 = Kp1 * error1 + Ki1 * integral1 + Kd1 * derivative1;
   out1=(int)(output1);

   if(out1>255){
     out1=255;
   }
   if(out1<0){
     out1=0;
   }
   //if(integral1>30.0){
    //integral1=27.94;
   //}
   prevError1 = error1;
   return out1;
}

void setup() {
  Serial.begin(9600);  // Start serial communication at 9600 baud rate

  // Set encoder pins as inputs
  pinMode(encoderPin1, INPUT);
  pinMode(pwm_r1, OUTPUT);
  // Attach interrupts to the encoder pins
  attachInterrupt(digitalPinToInterrupt(encoderPin1), pulseCounter1, RISING);  // Motor 1 encoder
  //analogWrite(pwm_r1, 50);
  analogWrite(pwm_r1, 10);
  //delay(2000);

}

void loop() {
  unsigned long currentMillis = millis();

  // Check if 1 second (1000 ms) has passed
  // checks the speed and also calculates the error and finds the pwm required
  
  if ((currentMillis - previousMillis) >= interval) {
    rpm1=speed_calculation(pulseCount1);
    out1=PID_calculations(rpm1);

    analogWrite(pwm_r1,out1);
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