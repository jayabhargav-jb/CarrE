// Pin definitions for the two encoder signals
#define pwm_r1 11
#define pwm_l1 10


const int encoderPin1 = 2;  // Encoder 1 signal pin (must be an interrupt pin)
const int encoderPin2 = 3;  // Encoder 2 signal pin (must be an interrupt pin)

// Volatile variables to store the pulse counts for each motor
volatile int pulseCount1 = 0;  // Pulse count for Motor 1
volatile int pulseCount2 = 0;  // Pulse count for Motor 2

const int pulsesPerRevolution = 7;  // Encoder gives 7 pulses per revolution

// Variables to handle timing
unsigned long previousMillis = 0;  // To track time
const unsigned long interval = 1000;  // Interval to calculate RPM (1 second)

// Interrupt service routine (ISR) for Motor 1
void pulseCounter1() {
  pulseCount1++;
}

// Interrupt service routine (ISR) for Motor 2
void pulseCounter2() {
  pulseCount2++;
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
  int pwmr = 30;
  int pwml = 30;

  analogWrite(pwm_r1,pwmr);
  analogWrite(pwm_l1,pwml);
  // Check if 1 second (1000 ms) has passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Calculate RPM for Motor 1
    float rpm1 = (pulseCount1*60) / (pulsesPerRevolution*19.1);

    // Calculate RPM for Motor 2
    float rpm2 =  (pulseCount2*60) / (pulsesPerRevolution*19.1);

    // Print Motor 1 RPM to the Serial Monitor
    //Serial.print("Motor 1 Speed: ");
    Serial.print(rpm1);
    //Serial.println(" RPM");
    Serial.print(",");

    // Print Motor 2 RPM to the Serial Monitor
    //Serial.print("Motor 2 Speed: ");
    Serial.println(rpm2);
    //Serial.println(" RPM");

    

    // Reset the pulse counts for the next interval
    pulseCount1 = 0;
    pulseCount2 = 0;
    //Serial.print("Pulse Count 1: ");
    //Serial.println(pulseCount1);
    //Serial.print("Pulse Count 2: ");
    //Serial.println(pulseCount2);

  }
}
