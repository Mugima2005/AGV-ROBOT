#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>          // ✅ ADDED (I2C for Arduino-3)

#define SS_PIN   4
#define RST_PIN  3

MFRC522 rfid(SS_PIN, RST_PIN);

// ---------------- RFID UID LIST ----------------
byte station0_UID[] = {0x91, 0x00, 0x63, 0x0C};   // COUNTER
byte station1_UID[] = {0x33, 0x0D, 0x5C, 0x1F};
byte station2_UID[] = {0xB0, 0xDC, 0x95, 0x0E};
byte station3_UID[] = {0x31, 0x47, 0x10, 0x0C};
byte station4_UID[] = {0x41, 0xB9, 0xEF, 0x1D};
byte station5_UID[] = {0xA0, 0xEF, 0x46, 0x0E};
byte station6_UID[] = {0x31, 0xE4, 0x1C, 0x0C};

int selectedStation = 0;
bool stopFlag = false;
bool startFlag = false;
bool returningToCounter = false;

// ---------------- LINE FOLLOWER ----------------
#define IR_SENSOR_RIGHT A1
#define IR_SENSOR_LEFT  A0

int forwardSpeed = 80;
int turnStrength = 80;
int softTurn     = 70;

int rightTrim = 10;
int leftTrim  = 0;

int wiggleDirection = 0;

// ---------------- MOTOR PINS ----------------
int enableRightMotor = 10;
int rightMotorPin1   = 9;
int rightMotorPin2   = 8;

int enableLeftMotor  = 5;
int leftMotorPin1    = 7;
int leftMotorPin2    = 6;

// ---------------- UID COMPARE ----------------
bool compareUID(byte *tagUID) {
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != tagUID[i]) return false;
  }
  return true;
}

// ---------------- RFID CHECK ----------------
void checkRFID() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  if (!returningToCounter) {
    if (selectedStation == 1 && compareUID(station1_UID)) stopFlag = true;
    if (selectedStation == 2 && compareUID(station2_UID)) stopFlag = true;
    if (selectedStation == 3 && compareUID(station3_UID)) stopFlag = true;
    if (selectedStation == 4 && compareUID(station4_UID)) stopFlag = true;
    if (selectedStation == 5 && compareUID(station5_UID)) stopFlag = true;
    if (selectedStation == 6 && compareUID(station6_UID)) stopFlag = true;
  }

  if (returningToCounter && compareUID(station0_UID)) {
    rotateMotor(0, 0);
    Serial.println("H");
    startFlag = false;
    returningToCounter = false;
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(150);
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  Wire.begin();           // ✅ ADDED (I2C MASTER)
  rfid.PCD_Init();

  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);

  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  pinMode(IR_SENSOR_RIGHT, INPUT);
  pinMode(IR_SENSOR_LEFT, INPUT);

  rotateMotor(0, 0);
}

// ================= MAIN LOOP =================
void loop() {

  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == 'S') {
      selectedStation = Serial.parseInt();
      startFlag = true;
      stopFlag = false;
      returningToCounter = false;
    }
    else if (cmd == 'C') {
      startFlag = false;
      stopFlag = false;
      returningToCounter = false;
      rotateMotor(0, 0);
      return;
    }
  }

  if (!startFlag) {
    rotateMotor(0, 0);
    return;
  }

  checkRFID();

  if (stopFlag) {
    rotateMotor(0, 0);
    Serial.println("R");

    // 🔥 ARM TRIGGER (Arduino-3)
    Wire.beginTransmission(0x09);
    Wire.write('A');
    Wire.endTransmission();

    delay(30000);

    rotateMotor(-90, 90);
    delay(4200);
    rotateMotor(0, 0);

    Serial.println("B");
    returningToCounter = true;
    stopFlag = false;
  }

  int right = digitalRead(IR_SENSOR_RIGHT);
  int left  = digitalRead(IR_SENSOR_LEFT);

  if (right == LOW && left == LOW) {
    rotateMotor(-60, -60);
    delay(60);

    if (wiggleDirection == 0) {
      rotateMotor(90, -90);
      wiggleDirection = 1;
    } else {
      rotateMotor(-90, 90);
      wiggleDirection = 0;
    }

    delay(60);
    return;
  }

  if (left == HIGH && right == LOW) {
    rotateMotor(turnStrength, -softTurn);
    return;
  }

  if (right == HIGH && left == LOW) {
    rotateMotor(-softTurn, turnStrength);
    return;
  }

  if (right == HIGH && left == HIGH) {
    rotateMotor(forwardSpeed, forwardSpeed);
    return;
  }
}

// ---------------- MOTOR CONTROL ----------------
void rotateMotor(int rightSpeed, int leftSpeed) {

  rightSpeed += rightTrim;
  leftSpeed  += leftTrim;

  digitalWrite(rightMotorPin1, rightSpeed > 0);
  digitalWrite(rightMotorPin2, rightSpeed < 0);

  digitalWrite(leftMotorPin1, leftSpeed > 0);
  digitalWrite(leftMotorPin2, leftSpeed < 0);

  analogWrite(enableRightMotor, abs(rightSpeed));
  analogWrite(enableLeftMotor,  abs(leftSpeed));
}
