#include <Wire.h>
#include <Servo.h>

Servo baseServo;
Servo clawServo;

int stepDelay = 35;
int clawDelay = 30;

int clawOpen = 100;
int clawClosed = 150;

volatile bool startArm = false;

void receiveEvent(int howMany) {
  while (Wire.available()) {
    char cmd = Wire.read();
    if (cmd == 'A') {
      startArm = true;
    }
  }
}

void setup() {
  Wire.begin(0x09);              // SLAVE
  Wire.onReceive(receiveEvent);

  baseServo.attach(9);
  clawServo.attach(6);

  baseServo.write(90);
  clawServo.write(clawOpen);
}

void loop() {
  if (!startArm) return;

  // ---- ARM SEQUENCE ----
  for (int p = clawOpen; p <= clawClosed; p++) {
    clawServo.write(p);
    delay(clawDelay);
  }

  for (int pos = 90; pos >= 0; pos--) {
    baseServo.write(pos);
    delay(stepDelay);
  }

  for (int p = clawClosed; p >= clawOpen; p--) {
    clawServo.write(p);
    delay(clawDelay);
  }

  delay(5000);

  for (int pos = 0; pos <= 90; pos++) {
    baseServo.write(pos);
    delay(stepDelay);
  }

  startArm = false;   // ✅ IMPORTANT (run only once)
}
