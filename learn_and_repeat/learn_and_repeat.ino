#include "header_for_two_motor_control.h"
unsigned long int  pwmforconversion_1=0;
unsigned long int  pwmforconversion_2=0;
unsigned long previousMillis = 0;  // To track time
const unsigned long interval_1 = 50;  
volatile unsigned int pulseCount1 = 0;  // Pulse count for Motor 1
volatile unsigned int pulseCount2 = 0;  // Pulse count for Motor 1
float rpm1=0.0;
float rpm2=0.0;
float out_1=0.0;
float out_2=0.0;
float setpoint2 = 0.01;  // Desired speeds  
float setpoint1 = 0.01;  // Desired speeds  
float* rpm_val;
int* out_val;
char* cmd_val = (char*)malloc(10 * sizeof(char)); 
int dir=0;
float* repeat_val;
int repeat_command;
// Interrupt service routine (ISR) for Motor 1
void pulseCounter1() {
  pulseCount1++;
}

void pulseCounter2() {
  pulseCount2++;
}

// Function to calculate checksum
int calculateChecksum(const char* message) {
  int checksum = 0;
  int length = strlen(message);

  // Iterate over each character in the message (excluding the checksum part)
  for (int i = 0; i < length - 4; i++) {
    // Serial.println(message[i]);
    checksum += message[i];  // Add the ASCII value of each character
  }
  return checksum % 256;  // Modulo 256 to keep checksum within byte range
}

// Function to extract the checksum from the command
int extractChecksum(const char* message) {
  char checksumStr[4];  // Buffer to store checksum (3 digits + null terminator)
  strncpy(checksumStr, &message[strlen(message) - 3], 3);  // Extract last 3 characters
  checksumStr[3] = '\0';  // Null-terminate
  return atoi(checksumStr);  // Convert to integer
}

// Function to verify the checksum
bool verifyChecksum(const char* message) {
  int receivedChecksum = extractChecksum(message);
  // Serial.println(receivedChecksum);
  int calculatedChecksum = calculateChecksum(message);
  return receivedChecksum == calculatedChecksum;
}




void setup() {
  Serial.begin(115200);  // Start serial communication at 115200 baud rate
  cmd_val[11] = '\0';
  pinMode(enable_1r, OUTPUT);
  pinMode(enable_1l, OUTPUT);
  pinMode(enable_2r, OUTPUT);
  pinMode(enable_2l, OUTPUT);

  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);
  pinMode(pwm_r1, OUTPUT);
  pinMode(pwm_l2, OUTPUT);
  pinMode(pwm_r2, OUTPUT);
  pinMode(pwm_l1, OUTPUT);

  digitalWrite(enable_1r, HIGH);
  digitalWrite(enable_1l, HIGH);
  digitalWrite(enable_2r, HIGH);
  digitalWrite(enable_2l, HIGH);

  attachInterrupt(digitalPinToInterrupt(encoderPin1), pulseCounter1, RISING);  // Motor 1 encoder
  attachInterrupt(digitalPinToInterrupt(encoderPin2), pulseCounter2, RISING);
}

void loop() {
  
  unsigned long currentMillis = millis();
  pid_control pid;
  // Check if enough time has passed and the checksum is valid
  if (Serial.available() > 11) {
    Serial.readBytesUntil('\n', cmd_val, 12);  // Read until newline
    cmd_val[11] = '\0';  // Ensure null termination

    // Verify checksum before processing
    if (!verifyChecksum(cmd_val)) {
      // Serial.println("Checksum invalid");
      dir = 0;
    }
    else {
      // Parse the message (excluding the checksum)
      char* token = strtok(cmd_val, " ");  // Split by space
      if (token) {
        dir = atoi(token);  // Convert first token to integer
        token = strtok(NULL, " ");  // Move to next token
      }
      if (token) {
        setpoint1 = atoi(token);  // Convert second token to integer
        token = strtok(NULL, " ");  // Move to next token
      }
      if (token) {
        setpoint2 = atoi(token);  // Convert third token to integer
      }
    }
    
  }
  if(dir==3){
    repeat_command=3;
  }
  
  if ((currentMillis - previousMillis) >= interval_1) {
    
    if((repeat_command!=3)){
      
      rpm_val= pid.speed_calculation(pulseCount1, pulseCount2);
      rpm1=*(rpm_val);
      rpm2=*(rpm_val+1);
      Serial.print(rpm1);
      Serial.print(",");
      Serial.println(rpm2);
      //Serial.println();
      pid.learn_phase(rpm1,rpm2,dir);
      
      out_val= pid.PID_calculations(rpm1,rpm2,setpoint1,setpoint2);
      out_1=*(out_val);
      out_2=*(out_val+1);
    }
    if(repeat_command==3){
      repeat_val=pid.repeat_phase();
      setpoint1=*(repeat_val);
      setpoint2=*(repeat_val+1);
      dir=*(repeat_val+2);
      Serial.println("in the loop");
      // Serial.print(setpoint1);
      // Serial.print(",");
      // Serial.print(setpoint2);
      // Serial.print(",");
      // Serial.println(dir);
      rpm_val= pid.speed_calculation(pulseCount1, pulseCount2);
      rpm1=*(rpm_val);
      rpm2=*(rpm_val+1);
      out_val= pid.PID_calculations(rpm1,rpm2,setpoint1,setpoint2);
      out_1=*(out_val);
      out_2=*(out_val+1);
    }
    pwmforconversion_1 = (out_1 / 1.5);
    pwmforconversion_2 = (out_2 / 1.5);
    
    
    if (dir == 1) {
      analogWrite(pwm_l1, pwmforconversion_1);
      analogWrite(pwm_r1, 0);
      analogWrite(pwm_l2, 0);
      analogWrite(pwm_r2, pwmforconversion_2);
    }
    else if (dir == 2) {
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
 
    previousMillis = currentMillis;  // Update time tracking
  }
}
