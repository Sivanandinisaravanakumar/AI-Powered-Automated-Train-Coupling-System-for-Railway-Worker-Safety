#include <Servo.h>

// Pin definitions
#define TRIG_PIN 13
#define ECHO_PIN 12
#define SERVO_PIN 9
#define LED_PIN 7
#define BUZZER_PIN 8
#define EMERGENCY_BUTTON_PIN 2

// Servo and distance variables
Servo couplingServo;
long duration;
int distance;

void setup() {
  // Initialize pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(EMERGENCY_BUTTON_PIN, INPUT_PULLUP); // Button uses internal pull-up

  // Attach servo
  couplingServo.attach(SERVO_PIN);
  couplingServo.write(0); // initial position

  // Serial communication for AI or monitoring
  Serial.begin(9600);
  Serial.println("AI-Powered Train Coupling System Initialized...");
}

void loop() {
  // Check emergency stop button
  if (digitalRead(EMERGENCY_BUTTON_PIN) == LOW) {
    stopSystem();
    return;
  }

  // Measure distance using ultrasonic sensor
  distance = readDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Safety check: only operate if no human is detected nearby
  if (distance > 30) { // Safe distance threshold in cm
    digitalWrite(LED_PIN, LOW);   // indicate system ready
    digitalWrite(BUZZER_PIN, LOW); // no alert

    // Operate servo for coupling
    operateServo();
  } else {
    // Human detected or too close
    Serial.println("Unsafe: Human detected or object too close!");
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH); // alert
    couplingServo.write(0);          // reset servo
  }

  delay(500); // small loop delay
}

// Function to read ultrasonic distance
int readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  int distanceCm = duration * 0.034 / 2; // cm
  return distanceCm;
}

// Function to move servo for coupling
void operateServo() {
  Serial.println("Coupling in progress...");
  couplingServo.write(90); // rotate to couple
  delay(2000);             // hold position
  couplingServo.write(0);  // reset
  Serial.println("Coupling complete.");
}

// Emergency stop function
void stopSystem() {
  Serial.println("EMERGENCY STOP ACTIVATED!");
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, HIGH);
  couplingServo.write(0);  // reset servo
  while (digitalRead(EMERGENCY_BUTTON_PIN) == LOW) {
    delay(100); // wait until button released
  }
  digitalWrite(BUZZER_PIN, LOW);
  Serial.println("System resumed.");
}
