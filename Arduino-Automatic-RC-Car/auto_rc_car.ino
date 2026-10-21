#include <Servo.h>

// ---------- Motor driver pins ----------
const int LF = 13;  // Left motor forward
const int LR = 12;  // Left motor backward
const int RF = 11;  // Right motor forward
const int RR = 10;  // Right motor backward

// ---------- Ultrasonic sensor pins ----------
const int TRIG = A0;
const int ECHO = A1;

// ---------- Servo (sensor scanner) ----------
const int SERVO_PIN = 6;
const int ANGLE_CENTER = 90;
const int ANGLE_LEFT   = 150;  // Swap with ANGLE_RIGHT if your left/right is reversed
const int ANGLE_RIGHT  = 30;

// ---------- Settings (adjust to your car) ----------
const int STOP_DISTANCE  = 10;   // Stop if object is this close (cm)
const int CLEAR_DISTANCE = 20;   // A side needs at least this much room (cm)
const int BACK_TIME      = 400;  // How long to reverse after stopping (ms)
const int TURN_TIME      = 500;  // How long to turn about 90 degrees (ms)

Servo scanner;

bool autoMode = true;  // true = drives itself, false = manual commands
char command = 'S';    // Manual command: F, B, L, R or S

// ---------- Movement functions ----------
void stopCar() {
  digitalWrite(LF, LOW);
  digitalWrite(LR, LOW);
  digitalWrite(RF, LOW);
  digitalWrite(RR, LOW);
}

void forward() {
  digitalWrite(LR, LOW);
  digitalWrite(RR, LOW);
  digitalWrite(LF, HIGH);
  digitalWrite(RF, HIGH);
}

void backward() {
  digitalWrite(LF, LOW);
  digitalWrite(RF, LOW);
  digitalWrite(LR, HIGH);
  digitalWrite(RR, HIGH);
}

void turnLeft() {
  // Left motor backward, right motor forward (spin in place)
  digitalWrite(LF, LOW);
  digitalWrite(RR, LOW);
  digitalWrite(LR, HIGH);
  digitalWrite(RF, HIGH);
}

void turnRight() {
  // Left motor forward, right motor backward (spin in place)
  digitalWrite(LR, LOW);
  digitalWrite(RF, LOW);
  digitalWrite(LF, HIGH);
  digitalWrite(RR, HIGH);
}

// ---------- Ultrasonic distance ----------
int getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  unsigned long duration = pulseIn(ECHO, HIGH, 30000UL);

  if (duration == 0) {
    return -1;  // No echo received
  }

  return (int)(duration * 0.0343 / 2.0);
}

// Turn the sensor to an angle, measure, then return the distance.
// A bad reading counts as 0 (blocked) to be safe.
int scanAt(int angle) {
  scanner.write(angle);
  delay(500);  // Give the servo time to move
  int d = getDistance();
  return (d < 0) ? 0 : d;
}

// ---------- Automatic obstacle avoidance ----------
void avoidObstacle() {
  Serial.println("OBSTACLE - ROBOT STOPPED");
  stopCar();
  delay(300);

  // Back up a little to make room
  Serial.println("Backing up...");
  backward();
  delay(BACK_TIME);
  stopCar();
  delay(200);

  // Look left and right
  int leftDist  = scanAt(ANGLE_LEFT);
  int rightDist = scanAt(ANGLE_RIGHT);
  scanner.write(ANGLE_CENTER);  // Face forward again
  delay(400);

  Serial.print("Left: ");
  Serial.print(leftDist);
  Serial.print(" cm, Right: ");
  Serial.print(rightDist);
  Serial.println(" cm");

  // Choose where to go
  if (leftDist < CLEAR_DISTANCE && rightDist < CLEAR_DISTANCE) {
    Serial.println("Both sides blocked - turning around");
    turnRight();
    delay(TURN_TIME * 2);
  }
  else if (leftDist > rightDist) {
    Serial.println("Turning left");
    turnLeft();
    delay(TURN_TIME);
  }
  else {
    Serial.println("Turning right");
    turnRight();
    delay(TURN_TIME);
  }

  stopCar();
  delay(200);
}

void autoDrive(int distance) {
  if (distance < 0) {
    // Invalid reading: stop for safety
    stopCar();
    Serial.println("No echo - stopped for safety");
  }
  else if (distance <= STOP_DISTANCE) {
    avoidObstacle();
  }
  else {
    forward();
  }
}

// ---------- Manual driving ----------
void manualDrive(int distance) {
  bool blocked = (distance < 0 || distance <= STOP_DISTANCE);

  if (command == 'F' && blocked) {
    stopCar();
    command = 'S';  // Cancel forward so it won't restart by itself
    Serial.println("Obstacle detected! Robot stopped.");
  }
  else if (command == 'F') {
    forward();
  }
  else if (command == 'B') {
    backward();
  }
  else if (command == 'L') {
    turnLeft();
  }
  else if (command == 'R') {
    turnRight();
  }
  else {
    stopCar();
  }
}

// ---------- Read serial commands ----------
void readCommand() {
  if (Serial.available() > 0) {
    char c = toupper(Serial.read());

    if (c == 'A') {
      autoMode = true;
      Serial.println("AUTO mode");
    }
    else if (c == 'F' || c == 'B' || c == 'L' || c == 'R' || c == 'S') {
      autoMode = false;
      command = c;
    }
  }
}

void setup() {
  pinMode(LF, OUTPUT);
  pinMode(LR, OUTPUT);
  pinMode(RF, OUTPUT);
  pinMode(RR, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  Serial.begin(9600);

  scanner.attach(SERVO_PIN);
  scanner.write(ANGLE_CENTER);  // Face forward
  delay(500);

  stopCar();

  Serial.println("AUTO mode (default). Commands: A=auto F=forward B=backward L=left R=right S=stop");
}

void loop() {
  readCommand();

  int distance = getDistance();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (autoMode) {
    autoDrive(distance);
  }
  else {
    manualDrive(distance);
  }

  delay(50);
}
