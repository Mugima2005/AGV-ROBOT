#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ------------------- ULTRASONIC -------------------
#define TRIG_PIN A0
#define ECHO_PIN A1
#define OBSTACLE_DIST 15   // cm

bool obstacleDetected = false;
bool resumePending = false;

// ------------------- KEYPAD SETUP -------------------
const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {6, 7, 8, 10};
byte colPins[COLS] = {2, 11, 12};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String input = "";
const int maxDigits = 3;
int selectedStation = 0;

// ------------------- ULTRASONIC READ -------------------
long readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  if (duration == 0) return 100;
  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  lcd.setCursor(0, 0);
  lcd.print("Enter Station:");
}

void loop() {

  // ================= ULTRASONIC CHECK =================
  long distance = readUltrasonic();

  // ---- OBSTACLE DETECTED ----
  if (distance < OBSTACLE_DIST && !obstacleDetected) {

    Serial.println("C");   // STOP ROBOT
    obstacleDetected = true;
    resumePending = true;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Obstacle Ahead");
    lcd.setCursor(0, 1);
    lcd.print("Robot Stopped");

    return;
  }

  // ---- OBSTACLE CLEARED → AUTO RESUME ----
  if (distance >= OBSTACLE_DIST && obstacleDetected && resumePending) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Resuming...");
    delay(800);

    Serial.print("S");
    Serial.println(selectedStation);   // RESUME LAST STATION

    obstacleDetected = false;
    resumePending = false;

    lcd.clear();
    lcd.print("Running...");
  }

  // ================= RECEIVE MESSAGE FROM BOT =================
  if (Serial.available()) {
    char msg = Serial.read();

    if (msg == 'R') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Station Reached");
    }
    else if (msg == 'B') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Back to Counter");
    }
    else if (msg == 'H') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("At Counter");
    }
  }

  // ================= KEYPAD =================
  char key = keypad.getKey();

  if (key) {

    if (key >= '0' && key <= '9') {
      if (input.length() < maxDigits) {
        input += key;
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Input:");
      lcd.setCursor(0, 1);
      lcd.print(input);
    }

    else if (key == '*') {
      input = "";
      selectedStation = 0;

      Serial.println("C");

      lcd.clear();
      lcd.print("Cleared");
      delay(600);

      lcd.clear();
      lcd.print("Enter Station:");
    }

    else if (key == '#') {
      if (input.length() > 0) {
        selectedStation = input.toInt();
      }

      Serial.print("S");
      Serial.println(selectedStation);

      lcd.clear();
      lcd.print("STARTED:");
      lcd.setCursor(0, 1);
      lcd.print(selectedStation);

      delay(1000);
      lcd.clear();
      lcd.print("Running...");
      input = "";
    }
  }
}
