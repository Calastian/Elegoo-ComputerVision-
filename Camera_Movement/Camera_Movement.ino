
int Trig = 13; // sends ultrasonic pulse (pin 13 of the ultrasonic sensor)
int Echo = 12; //Recieves ultrasonic pulse (pin 12)
long pingT; //stores ping distance (in microseconds)
float distance_cm;
int SPEED = 100;
int R_SENSOR_PIN = A0;
int C_SENSOR_PIN = A1;
int L_SENSOR_PIN = A2;
int THRESHOLD    = 700;   // ADC cutoff (0–1023)

// TB6612FNG motor driver pins (Elegoo V4)
int STBY    = 3;
int PWM_A   = 6;
int PHASE_A = 8;
int PWM_B   = 5;
int PHASE_B = 7;

int MOTOR_SPEED = 50;    // 0–255


void setup() {

  // Configure UltraSound inputs/outputs
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  Serial.begin(9600);

  // Configure line‐tracker inputs
  pinMode(R_SENSOR_PIN, INPUT);
  pinMode(C_SENSOR_PIN, INPUT);
  pinMode(L_SENSOR_PIN, INPUT);

  // Configure motor driver outputs
  pinMode(STBY,    OUTPUT);
  pinMode(PWM_A,    OUTPUT);
  pinMode(PHASE_A, OUTPUT);
  pinMode(PWM_B,    OUTPUT);
  pinMode(PHASE_B, OUTPUT);

  // Start with motors in standby (disabled)
  digitalWrite(STBY, LOW);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
}


void loop() {


  espMovement();







  // AvoidLines();
  // LineTracking();
  // CalDist();
  // ObjectRun(distance_cm);
}

void espMovement()
{
    if (Serial.available()) {
    char command = Serial.read();

    switch (command) {
      case 'F':
        driveForward(200);
        break;
      case 'B':
        driveBackward(200);
        break;
      case 'L':
        pivotLeft(100);
        break;
      case 'R':
        pivotRight(100);
        break;
      case 'S':
        stopMotors();
        break;
    }
  }
}

int pingTime() {
  int pingTravelTime;
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(20);
  digitalWrite(Trig, LOW);
  pingTravelTime = pulseIn(Echo, HIGH);
  return pingTravelTime;
}


void ObjectRun(int distance) {

  while (distance_cm < 30 && distance_cm > 0) {

    RandTurn(); // this one too!
    driveForward(200);
    // BackUp(); // just these two for perfect object detection
    CalDist();

  }
}

void AvoidLines()
{

  // Read each sensor (0 = white, ~1023 = black)
  int rightVal  = analogRead(R_SENSOR_PIN);
  int centerVal = analogRead(C_SENSOR_PIN);
  int leftVal   = analogRead(L_SENSOR_PIN);

  // Determine if each is over the black line
  bool overRight  = (rightVal  > THRESHOLD);
  bool overCenter = (centerVal > THRESHOLD);
  bool overLeft   = (leftVal   > THRESHOLD);

  // Print raw values for debugging
  Serial.print("R="); Serial.print(rightVal);
  Serial.print("  C="); Serial.print(centerVal);
  Serial.print("  L="); Serial.println(leftVal);

  // Avoid the lines
  if (!overCenter && !(overLeft || overRight)) {
    driveForward(MOTOR_SPEED);
    Serial.println("  → Forward");
  }
  else if (overLeft && !(overCenter || overRight)) {
    pivotRight(MOTOR_SPEED);
    TurnRight();
    Serial.println("  → Pivot Right");
  }
  else if (overRight && !(overLeft || overCenter)) {
    pivotLeft(MOTOR_SPEED);
    TurnLeft();
    Serial.println("  → Pivot Left");
  }
  else if (overRight && overCenter && overLeft) {
    stopMotors();
    // findLine(MOTOR_SPEED);
    // Serial.println("  → Line Lost: Stopping");
  }
  else
  {
    if (rand() % 10 % 2 == 0)
    {
      pivotRight(MOTOR_SPEED);
    }
    else
    {
      pivotLeft(MOTOR_SPEED);
    }

  }

  delay(50);  // short pause (motor commands are non-blocking)
}
void LineTracking()
{

  // Read each sensor (0 = white, ~1023 = black)
  int rightVal  = analogRead(R_SENSOR_PIN);
  int centerVal = analogRead(C_SENSOR_PIN);
  int leftVal   = analogRead(L_SENSOR_PIN);

  // Determine if each is over the black line
  bool overRight  = (rightVal  > THRESHOLD);
  bool overCenter = (centerVal > THRESHOLD);
  bool overLeft   = (leftVal   > THRESHOLD);

  // Print raw values for debugging
  Serial.print("R="); Serial.print(rightVal);
  Serial.print("  C="); Serial.print(centerVal);
  Serial.print("  L="); Serial.println(leftVal);

  // Follow the line
  if (overCenter) {
    driveForward(MOTOR_SPEED);
    Serial.println("  → Forward");
  }
  else if (overLeft && (!overCenter || !overRight)) {
    pivotLeft(MOTOR_SPEED);
    Serial.println("  → Pivot Left");
  }
  else if (overRight && (!overCenter || !overLeft)) {
    pivotRight(MOTOR_SPEED);
    Serial.println("  → Pivot Right");
  }
  else {
    stopMotors();
    findLine(MOTOR_SPEED);
    Serial.println("  → Line Lost: Stopping");
  }

  delay(50);  // short pause (motor commands are non-blocking)
}



// ─── MOTOR CONTROL FUNCTIONS ───────────────────────────────────────────
void enableDriver() {
  digitalWrite(STBY, HIGH);
}

void disableDriver() {
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
  digitalWrite(STBY, LOW);
}

void driveForward(int speed) {
  enableDriver();
  digitalWrite(PHASE_A, HIGH);
  digitalWrite(PHASE_B, HIGH);
  analogWrite(PWM_A, speed);
  analogWrite(PWM_B, speed);
}

void driveBackward(int speed) {
  enableDriver();
  digitalWrite(PHASE_A, LOW);
  digitalWrite(PHASE_B, LOW);
  analogWrite(PWM_A, speed);
  analogWrite(PWM_B, speed);
}

void pivotLeft(int speed) {
  enableDriver();
  digitalWrite(PHASE_A, LOW);
  digitalWrite(PHASE_B, HIGH);
  analogWrite(PWM_A, speed);
  analogWrite(PWM_B, speed);
}

void pivotRight(int speed) {
  enableDriver();
  digitalWrite(PHASE_A, HIGH);
  digitalWrite(PHASE_B, LOW);
  analogWrite(PWM_A, speed);
  analogWrite(PWM_B, speed);
}

void findLine(int speed)
{
  enableDriver();
  digitalWrite(PHASE_A, HIGH);
  digitalWrite(PHASE_B, LOW);
  analogWrite(PWM_A, speed);
  analogWrite(PWM_B, speed);

}

void stopMotors() {
  disableDriver();
}

void CalDist()
{
  pingT = pingTime();
  distance_cm = pingT / 58; //convert the microseconds to CM (~0.0343cm/us)
  Serial.println(pingT);
  Serial.println(distance_cm);
}

void BackUp()
{
  if (distance_cm > 0 && distance_cm < 10)
  {
    digitalWrite(PHASE_A, LOW);
    digitalWrite(PHASE_B, LOW);

  }
  else
  {
    delay(200);

    digitalWrite(PHASE_A, HIGH);
    digitalWrite(PHASE_B, HIGH);
  }
}

void RandTurn()
{
  if ((int)distance_cm % 2 == 0)
  {
    analogWrite(PWM_A, SPEED + 30);
    analogWrite(PWM_B, 20);
    delay(500);
  }
  else
  {
    analogWrite(PWM_A, 20);
    analogWrite(PWM_B, SPEED + 30);
    delay(500);
  }
}

void TurnRight()
{
  digitalWrite(PHASE_A, HIGH);
  analogWrite(PWM_A, 80);
  digitalWrite(PHASE_B, LOW);
  analogWrite(PWM_B, 80);
  delay(500);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
  delay(500);
  digitalWrite(PHASE_B, HIGH);
}

void TurnLeft()
{
  digitalWrite(PHASE_A, LOW);
  analogWrite(PWM_A, 80);
  digitalWrite(PHASE_B, HIGH);
  analogWrite(PWM_B, 80);
  delay(500);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
  delay(500);
  digitalWrite(PHASE_A, HIGH);
}

void FF(int Fast, int Slow)
{
  digitalWrite(STBY, HIGH);
  digitalWrite(PHASE_A, HIGH);
  digitalWrite(PHASE_B, HIGH);
  analogWrite(PWM_A, Fast);
  analogWrite(PWM_B, Slow);
}

int Min(int one, int two, int three, int four)
{
  if (one < two && one < three && one < four)
  {
    return one;
  }
  if (two < one && two < three && two < four)
  {
    return two;
  }
  if (three < two && three < one && three < four)
  {
    return three;
  }
  if (four < two && four < three && four < one)
  {
    return four;
  }
}
