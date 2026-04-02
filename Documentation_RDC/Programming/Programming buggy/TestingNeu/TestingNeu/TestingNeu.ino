#include <Bluepad32.h>
#include <ESP32Servo.h>

// ===== Motor & Servo Pin Setup =====
const int fR_PWM = 13;  // Front Right
const int bR_PWM = 32;  // Back Right
const int fL_PWM = 22;   // Front Left
const int bL_PWM = 16;  // Back Left

const int servoPin1 = 4;   // Base servo
const int servoPin2 = 21;   // Base servo 14
const int servoPin3 = 18;  // Gripper servo

int servo1_pos = 0;
int servo2_pos = 0;
int servo3_pos = 90;   // middle start

const int SERVO_SPEED_Greifer = 5;  // degrees per loop while holding
const int SERVO_SPEED_Base = 3;

Servo servo1;
Servo servo2;
Servo servo3;

// ===== Shift Register Pins (74HC595) =====
const uint8_t dataPin = 25;
const uint8_t clockPin = 27;
const uint8_t latchPin = 26;

uint16_t val_dri = 0;  // Motor direction bits

// ===== Bit Mapping (for direction control) =====
const uint16_t FR_fwd_BIT = (1 << 6);
const uint16_t FR_bwd_BIT = (1 << 7);
const uint16_t BR_fwd_BIT = (1 << 4);
const uint16_t BR_bwd_BIT = (1 << 5);
const uint16_t FL_fwd_BIT = (1 << 8);
const uint16_t FL_bwd_BIT = (1 << 11);
const uint16_t BL_fwd_BIT = (1 << 10);
const uint16_t BL_bwd_BIT = (1 << 9);

// ===== Controller =====
ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// --- Controller connection callbacks ---
void onConnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      myControllers[i] = ctl;
      Serial.printf("Controller connected (index %d)\n", i);
      return;
    }
  }
  Serial.println("No free slot for controller!");
}

void onDisconnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      myControllers[i] = nullptr;
      Serial.printf("Controller disconnected (index %d)\n", i);
      return;
    }
  }
}

// ====== Helper functions ======
void writeShiftRegister(uint16_t val) {
  digitalWrite(latchPin, LOW);
  for (int i = 0; i < 16; i++) {
    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, (val >> i) & 1);
    digitalWrite(clockPin, HIGH);
  }
  digitalWrite(latchPin, HIGH);
}

void setMotorDirection(uint16_t fwd, uint16_t bwd, int dir) {
  val_dri &= ~(fwd | bwd);  // Clear
  if (dir == 1)
    val_dri |= fwd;
  else if (dir == -1)
    val_dri |= bwd;
}

void updateMotors(int leftSpeed, int rightSpeed) {
  leftSpeed = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);
  analogWrite(fL_PWM, leftSpeed);
  analogWrite(bL_PWM, leftSpeed);
  analogWrite(fR_PWM, rightSpeed);
  analogWrite(bR_PWM, rightSpeed);
}

// ====== Main Controller Processing ======
void processGamepad(ControllerPtr ctl) {
  int LX = ctl->axisX();
  int LY = ctl->axisY();

  // Testing
  // forward
  if (ctl->dpad() == 0x01)
  {
    int speed = 200;
    setMotorDirection(FR_fwd_BIT, FR_bwd_BIT, 1);
    setMotorDirection(FL_fwd_BIT, FL_bwd_BIT, 1);
    setMotorDirection(BR_fwd_BIT, BR_bwd_BIT, 1);
    setMotorDirection(BL_fwd_BIT, BL_bwd_BIT, 1);
    updateMotors(speed, speed);
  }
  //backward
  else if (ctl->dpad() == 0x02)
  {
    int speed = 200;
    setMotorDirection(FR_fwd_BIT, FR_bwd_BIT, -1);
    setMotorDirection(FL_fwd_BIT, FL_bwd_BIT, -1);
    setMotorDirection(BR_fwd_BIT, BR_bwd_BIT, -1);
    setMotorDirection(BL_fwd_BIT, BL_bwd_BIT, -1);
    updateMotors(speed, speed);
  }
  // left
  else if (ctl->dpad() == 0x08)
  {
    int speed = 150;
    setMotorDirection(FR_fwd_BIT, FR_bwd_BIT, -1);
    setMotorDirection(FL_fwd_BIT, FL_bwd_BIT, 1);
    setMotorDirection(BR_fwd_BIT, BR_bwd_BIT, -1);
    setMotorDirection(BL_fwd_BIT, BL_bwd_BIT, 1);
    updateMotors(speed, speed);
  }
  //right
  else if (ctl->dpad() == 0x04)
  {
    int speed = 150;
    setMotorDirection(FR_fwd_BIT, FR_bwd_BIT, 1);
    setMotorDirection(FL_fwd_BIT, FL_bwd_BIT, -1);
    setMotorDirection(BR_fwd_BIT, BR_bwd_BIT, 1);
    setMotorDirection(BL_fwd_BIT, BL_bwd_BIT, -1);
    updateMotors(speed, speed);
  }
  else
  {
    updateMotors(0,0);
  }    

  // ================= SERVO CONTROL =================

// ----- Servo 1 (Base) -----
if (ctl->buttons() == 0x0008) {   // traingle (down)
    servo1_pos = constrain(servo1_pos + SERVO_SPEED_Base, 0, 60);
}
if (ctl->buttons() == 0x0001) {   // X (up)
    servo1_pos = constrain(servo1_pos - SERVO_SPEED_Base, 0, 60);
}
servo1.write(servo1_pos);

// ----- Servo 2 (Base) -----
if (ctl->buttons() == 0x0008) {
    servo2_pos = constrain(servo2_pos + SERVO_SPEED_Base, 0, 60);
}
if (ctl->buttons() == 0x0001) {
    servo2_pos = constrain(servo2_pos - SERVO_SPEED_Base, 0, 60);
}
servo2.write(servo2_pos);

// ----- Servo 3 (Greifer) -----
if (ctl->buttons() == 0x0002) {    // Circle (close)
    servo3_pos = constrain(servo3_pos + SERVO_SPEED_Greifer, 0, 180);
}
if (ctl->buttons() == 0x0004) {   // Square (open)
    servo3_pos = constrain(servo3_pos - SERVO_SPEED_Greifer, 0, 180);
}
servo3.write(servo3_pos);

  writeShiftRegister(val_dri);
}

// ====== Controller Loop ======
void processControllers() {
  for (auto ctl : myControllers) {
    if (ctl && ctl->isConnected() && ctl->hasData() && ctl->isGamepad())
      processGamepad(ctl);
  }
}

// ====== Setup ======
void setup() {
  Serial.begin(115200);
  pinMode(fR_PWM, OUTPUT);
  pinMode(bR_PWM, OUTPUT);
  pinMode(fL_PWM, OUTPUT);
  pinMode(bL_PWM, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);

  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);

  Serial.println("Robot ready. Connect your PS4 controller!");
}

// ====== Loop ======
void loop() {
  if (BP32.update())
    processControllers();
  delay(50);
}

