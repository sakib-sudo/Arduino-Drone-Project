#include <Arduino.h>
#include <ESP32Servo.h>

// ----- Pin mapping (change to match your wiring) -----
static const int BASE_SERVO_PIN = 13;
static const int SHOULDER_SERVO_PIN = 12;
static const int ELBOW_SERVO_PIN = 14;
static const int WRIST_SERVO_PIN = 27;
static const int GRIPPER_SERVO_PIN = 26;

// ----- Servo limits (degrees) -----
struct ServoConfig {
  int minAngle;
  int maxAngle;
  int homeAngle;
};

ServoConfig baseCfg = {0, 180, 90};
ServoConfig shoulderCfg = {15, 165, 90};
ServoConfig elbowCfg = {10, 170, 90};
ServoConfig wristCfg = {0, 180, 90};
ServoConfig gripperCfg = {20, 120, 60};

Servo baseServo;
Servo shoulderServo;
Servo elbowServo;
Servo wristServo;
Servo gripperServo;

int baseAngle = baseCfg.homeAngle;
int shoulderAngle = shoulderCfg.homeAngle;
int elbowAngle = elbowCfg.homeAngle;
int wristAngle = wristCfg.homeAngle;
int gripperAngle = gripperCfg.homeAngle;

// Smooth movement speed: smaller delay = faster motion.
static const int STEP_DELAY_MS = 12;

int clampAngle(int value, const ServoConfig &cfg) {
  if (value < cfg.minAngle) return cfg.minAngle;
  if (value > cfg.maxAngle) return cfg.maxAngle;
  return value;
}

void moveServoSmooth(Servo &servo, int &current, int target, const ServoConfig &cfg) {
  target = clampAngle(target, cfg);

  while (current != target) {
    current += (current < target) ? 1 : -1;
    servo.write(current);
    delay(STEP_DELAY_MS);
  }
}

void printState() {
  Serial.print("Base:");
  Serial.print(baseAngle);
  Serial.print(" Shoulder:");
  Serial.print(shoulderAngle);
  Serial.print(" Elbow:");
  Serial.print(elbowAngle);
  Serial.print(" Wrist:");
  Serial.print(wristAngle);
  Serial.print(" Gripper:");
  Serial.println(gripperAngle);
}

void goHome() {
  moveServoSmooth(baseServo, baseAngle, baseCfg.homeAngle, baseCfg);
  moveServoSmooth(shoulderServo, shoulderAngle, shoulderCfg.homeAngle, shoulderCfg);
  moveServoSmooth(elbowServo, elbowAngle, elbowCfg.homeAngle, elbowCfg);
  moveServoSmooth(wristServo, wristAngle, wristCfg.homeAngle, wristCfg);
  moveServoSmooth(gripperServo, gripperAngle, gripperCfg.homeAngle, gripperCfg);
}

void handleCommand(const String &cmdRaw) {
  String cmd = cmdRaw;
  cmd.trim();
  cmd.toUpperCase();

  if (cmd == "HOME") {
    goHome();
    Serial.println("OK HOME");
    printState();
    return;
  }

  if (cmd == "OPEN") {
    moveServoSmooth(gripperServo, gripperAngle, gripperCfg.maxAngle, gripperCfg);
    Serial.println("OK OPEN");
    printState();
    return;
  }

  if (cmd == "CLOSE") {
    moveServoSmooth(gripperServo, gripperAngle, gripperCfg.minAngle, gripperCfg);
    Serial.println("OK CLOSE");
    printState();
    return;
  }

  // Format: <JOINT> <ANGLE>
  // Joints: BASE, SHOULDER, ELBOW, WRIST, GRIPPER
  int separator = cmd.indexOf(' ');
  if (separator < 0) {
    Serial.println("ERR Format: <JOINT> <ANGLE>");
    return;
  }

  String joint = cmd.substring(0, separator);
  int target = cmd.substring(separator + 1).toInt();

  if (joint == "BASE") {
    moveServoSmooth(baseServo, baseAngle, target, baseCfg);
  } else if (joint == "SHOULDER") {
    moveServoSmooth(shoulderServo, shoulderAngle, target, shoulderCfg);
  } else if (joint == "ELBOW") {
    moveServoSmooth(elbowServo, elbowAngle, target, elbowCfg);
  } else if (joint == "WRIST") {
    moveServoSmooth(wristServo, wristAngle, target, wristCfg);
  } else if (joint == "GRIPPER") {
    moveServoSmooth(gripperServo, gripperAngle, target, gripperCfg);
  } else {
    Serial.println("ERR Unknown joint");
    return;
  }

  Serial.println("OK");
  printState();
}

void printHelp() {
  Serial.println("ESP32 Robotic Arm Ready");
  Serial.println("Commands:");
  Serial.println("  HOME");
  Serial.println("  OPEN");
  Serial.println("  CLOSE");
  Serial.println("  BASE <0-180>");
  Serial.println("  SHOULDER <15-165>");
  Serial.println("  ELBOW <10-170>");
  Serial.println("  WRIST <0-180>");
  Serial.println("  GRIPPER <20-120>");
}

void setup() {
  Serial.begin(115200);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  baseServo.setPeriodHertz(50);
  shoulderServo.setPeriodHertz(50);
  elbowServo.setPeriodHertz(50);
  wristServo.setPeriodHertz(50);
  gripperServo.setPeriodHertz(50);

  baseServo.attach(BASE_SERVO_PIN, 500, 2500);
  shoulderServo.attach(SHOULDER_SERVO_PIN, 500, 2500);
  elbowServo.attach(ELBOW_SERVO_PIN, 500, 2500);
  wristServo.attach(WRIST_SERVO_PIN, 500, 2500);
  gripperServo.attach(GRIPPER_SERVO_PIN, 500, 2500);

  goHome();
  printHelp();
  printState();
}

void loop() {
  static String input;

  while (Serial.available()) {
    char c = static_cast<char>(Serial.read());

    if (c == '\n' || c == '\r') {
      if (input.length() > 0) {
        handleCommand(input);
        input = "";
      }
    } else {
      input += c;
    }
  }
}
