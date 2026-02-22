# Arduino-Drone-Project
This drone was build with arduino uno and mpu 5060 as a project.

## ESP32 Robotic Arm Sketch
A ready-to-upload sketch for controlling a 5-DOF robotic arm with ESP32 has been added:

- `esp32_robotic_arm.ino`

### Required library
Install from Arduino Library Manager:
- `ESP32Servo`

### Serial commands (115200 baud)
- `HOME`
- `OPEN`
- `CLOSE`
- `BASE <angle>`
- `SHOULDER <angle>`
- `ELBOW <angle>`
- `WRIST <angle>`
- `GRIPPER <angle>`
