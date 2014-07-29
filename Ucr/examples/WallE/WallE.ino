/*--------------------------------------------------------------------------------
 * Author	   : makesense.cc (info@makesense.cc)
 * Date		   : 2014-07-29
 * Copyright       : 2013-2014 makesense.cc
 *
 * Description     : This is a basic example for WallE robot for using SiCi.
 * Arduino Model   : Rokit-ArduBox128
 *
 *--------------------------------------------------------------------------------
 */

#include <Herkulex.h>
#include <Ucr.h>

#define HKX_WHEEL1 8
#define HKX_WHEEL2 9
#define HKX_WHEEL3 10

#define HKX_PAN 11
#define HKX_TILT 12

#define HKX_ARM_L1 4
#define HKX_ARM_L2 5
#define HKX_ARM_L3 6
#define HKX_ARM_L4 7

#define HKX_ARM_R1 0
#define HKX_ARM_R2 1
#define HKX_ARM_R3 2
#define HKX_ARM_R4 3

#define WHEEL_TIME 500 //msec
#define JOINT_TIME 300 //msec

Ucr ucr;

void reportSensorToSiCi() {
  int angle;
  angle = (int)Herkulex.getAngle(HKX_PAN);
  ucr.sendMotorAngle(21, angle+180);
  angle = (int)Herkulex.getAngle(HKX_TILT);
  ucr.sendMotorAngle(22, angle+180);
  angle = (int)Herkulex.getAngle(HKX_ARM_L1);
  ucr.sendMotorAngle(23, angle+180);
  angle = (int)Herkulex.getAngle(HKX_ARM_L2);
  ucr.sendMotorAngle(24, angle+180);
  angle = (int)Herkulex.getAngle(HKX_ARM_L3);
  ucr.sendMotorAngle(25, angle+180);
  angle = (int)Herkulex.getAngle(HKX_ARM_L4);
  ucr.sendMotorAngle(26, angle+180);
  angle = (int)Herkulex.getAngle(HKX_ARM_R1);
  ucr.sendMotorAngle(27, angle+180);
  angle = (int)Herkulex.getAngle(HKX_ARM_R2);
  ucr.sendMotorAngle(28, angle+180);
  angle = (int)Herkulex.getAngle(HKX_ARM_R3);
  ucr.sendMotorAngle(29, angle+180);
  angle = (int)Herkulex.getAngle(HKX_ARM_R4);
  ucr.sendMotorAngle(30, angle+180);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(32, OUTPUT);
  digitalWrite(32, HIGH);

  Herkulex.beginSerial1(115200); //open serial
  Herkulex.reboot(HKX_PAN);
  Herkulex.reboot(HKX_TILT);
  Herkulex.reboot(HKX_WHEEL1);
  Herkulex.reboot(HKX_WHEEL2);
  Herkulex.reboot(HKX_WHEEL3);
  delay(500); 
  Herkulex.initialize(); //initialize motors
  delay(200);  
  Herkulex.torqueON(HKX_WHEEL1);
  Herkulex.torqueON(HKX_WHEEL2);
  Herkulex.torqueON(HKX_WHEEL3);
  
  ucr.setReportFunction(reportSensorToSiCi);
}

void loop() {
  ucr.runTimer();
  while (ucr.count() > 0) {
    sProtocol protocol = ucr.dequeue();
    switch (protocol.cmd)
    {
      case MS_DEVICE_SERVO:  // Servo Motor
      {
        int angle = (int)protocol.value;
        switch (protocol.id) {
          case 1: // pan
            Herkulex.moveOneAngle(HKX_PAN, angle - 180, JOINT_TIME, LED_BLUE);
            break;
          case 2: // tilt
            Herkulex.moveOneAngle(HKX_TILT, angle - 180, JOINT_TIME, LED_BLUE);
            break;
          case 3: // Arm Left1
            Herkulex.moveOneAngle(HKX_ARM_L1, angle - 180, JOINT_TIME, LED_BLUE);
            break;
          case 4: // Arm Left2
            Herkulex.moveOneAngle(HKX_ARM_L2, angle - 180, JOINT_TIME, LED_BLUE);
            break;
          case 5: // Arm Left3
            Herkulex.moveOneAngle(HKX_ARM_L3, angle - 180, JOINT_TIME, LED_BLUE);
            break;
          case 6: // Arm Left4
            Herkulex.moveOneAngle(HKX_ARM_L4, angle - 180, JOINT_TIME, LED_BLUE);
            break;
          case 7: // Arm Right1
            Herkulex.moveOneAngle(HKX_ARM_R1, angle - 180, JOINT_TIME, LED_BLUE);
            break;
          case 8: // Arm Right2
            Herkulex.moveOneAngle(HKX_ARM_R2, angle - 180, JOINT_TIME, LED_BLUE);
            break;
          case 9: // Arm Right3
            Herkulex.moveOneAngle(HKX_ARM_R3, angle - 180, JOINT_TIME, LED_BLUE);
            break;
          case 10: // Arm Right4
            Herkulex.moveOneAngle(HKX_ARM_R4, angle - 180, JOINT_TIME, LED_BLUE);
            break;
        }
        break;
      }
      case MS_DEVICE_DC:  // DC Motor
      {
        int velocity = (int)protocol.value;
        switch (protocol.id) {
          case 11: // front
            Herkulex.moveSpeedOne(HKX_WHEEL1, velocity * 10, WHEEL_TIME, LED_BLUE);
            break;
          case 12: // right back
            Herkulex.moveSpeedOne(HKX_WHEEL2, velocity * 10, WHEEL_TIME, LED_BLUE);
            break;
          case 13: // left back
            Herkulex.moveSpeedOne(HKX_WHEEL3, velocity * 10, WHEEL_TIME, LED_BLUE);
            break;
        }
        break;
      }
      case MS_DEVICE_INFO:
      {
        ucr.sendDeviceInfo(1, MS_DEVICE_SERVO);  // Servo Motor - Pan
        ucr.sendDeviceInfo(2, MS_DEVICE_SERVO);  // Servo Motor - Tilt
        ucr.sendDeviceInfo(3, MS_DEVICE_SERVO);  // Servo Motor - Arm Left1
        ucr.sendDeviceInfo(4, MS_DEVICE_SERVO);  // Servo Motor - Arm Left2
        ucr.sendDeviceInfo(5, MS_DEVICE_SERVO);  // Servo Motor - Arm Left3
        ucr.sendDeviceInfo(6, MS_DEVICE_SERVO);  // Servo Motor - Arm Left4
        ucr.sendDeviceInfo(7, MS_DEVICE_SERVO);  // Servo Motor - Arm Right1
        ucr.sendDeviceInfo(8, MS_DEVICE_SERVO);  // Servo Motor - Arm Right2
        ucr.sendDeviceInfo(9, MS_DEVICE_SERVO);  // Servo Motor - Arm Right3
        ucr.sendDeviceInfo(10, MS_DEVICE_SERVO);  // Servo Motor - Arm Right4
       
        ucr.sendDeviceInfo(11, MS_DEVICE_DC); // DC Motor - Front
        ucr.sendDeviceInfo(12, MS_DEVICE_DC); // DC Motor - Right Back
        ucr.sendDeviceInfo(13, MS_DEVICE_DC); // DC Motor - Left Back

        ucr.sendDeviceInfo(21, MS_SENSOR_ANGLE);  // Servo Motor - Pan
        ucr.sendDeviceInfo(22, MS_SENSOR_ANGLE);  // Servo Motor - Tilt        
        ucr.sendDeviceInfo(23, MS_SENSOR_ANGLE);  // Servo Motor - Arm Left1
        ucr.sendDeviceInfo(24, MS_SENSOR_ANGLE);  // Servo Motor - Arm Left2
        ucr.sendDeviceInfo(25, MS_SENSOR_ANGLE);  // Servo Motor - Arm Left3
        ucr.sendDeviceInfo(26, MS_SENSOR_ANGLE);  // Servo Motor - Arm Left4
        ucr.sendDeviceInfo(27, MS_SENSOR_ANGLE);  // Servo Motor - Arm Right1
        ucr.sendDeviceInfo(28, MS_SENSOR_ANGLE);  // Servo Motor - Arm Right2
        ucr.sendDeviceInfo(29, MS_SENSOR_ANGLE);  // Servo Motor - Arm Right3
        ucr.sendDeviceInfo(30, MS_SENSOR_ANGLE);  // Servo Motor - Arm Right4

        ucr.sendDeviceInfo(0, 0);    // End of device info
        break;
      }
    }
  }
}

void serialEvent() {
  while (Serial.available()) {
    ucr.pushByte (Serial.read());
  }
}
