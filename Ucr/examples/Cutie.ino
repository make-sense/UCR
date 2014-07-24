/*--------------------------------------------------------------------------------
* Author : makesense.cc (info@makesense.cc)
* Date : 2014-07-23
* Copyright : 2013-2014 makesense.cc
*
* Description : This is a basic example for using SiCi.
*
*--------------------------------------------------------------------------------
*/
 
#include <Ucr.h>

// ---- 2DC
#define DC_MAX 2+1
 
#define MOTOR_A_PWM 5
#define MOTOR_A_DIR 4
#define MOTOR_B_PWM 6
#define MOTOR_B_DIR 7
 
// ---- RGB LED
#define LED_R 9
#define LED_G 10
#define LED_B 11
 
// ---- HC-SR04
#define SR04_ECHO 13
#define SR04_TRIG 12
 
// ---- Line Detect
#define LINE0 A0
#define LINE1 A1
 
// ---- Line LED
#define LINE_LED0 2
#define LINE_LED1 3
 
// ---- Battery Check
#define BATTERY A5
 
Ucr ucr;
int dc_val[DC_MAX];
 
void init2DC() {
  pinMode(MOTOR_A_PWM, OUTPUT);
  pinMode(MOTOR_A_DIR, OUTPUT);
  digitalWrite(MOTOR_A_PWM, LOW);
  digitalWrite(MOTOR_A_DIR, LOW);
   
  pinMode(MOTOR_B_PWM, OUTPUT);
  pinMode(MOTOR_B_DIR, OUTPUT);
  digitalWrite(MOTOR_B_PWM, LOW);
  digitalWrite(MOTOR_B_DIR, LOW);
}
 
void InitSR04() {
  pinMode(SR04_TRIG, OUTPUT);
  pinMode(SR04_ECHO, INPUT);
}
 
int GetRangeSR04() {
  long duration;
  digitalWrite(SR04_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(SR04_TRIG, HIGH);
  delayMicroseconds(5);
  digitalWrite(SR04_TRIG, LOW);
   
  duration = pulseIn(SR04_ECHO, HIGH, 36000);
  return (duration / 29 / 2); // us to cm
}
 
void SetMotorPower(byte id, byte power) {
  char _power = (char)power;
  if (id==1)
  {
    if (1 <= _power && _power <= 100)
    {
      // Forward
      digitalWrite(MOTOR_A_DIR, LOW);
      byte val = map(_power, 0, 100, 0, 255);
      analogWrite(MOTOR_A_PWM, val);
    }
    else if (-100 <= _power && _power <= -1)
    {
      // Backward
      digitalWrite(MOTOR_A_DIR, HIGH);
      byte val = map(-_power, 0, 100, 0, 255);
      analogWrite(MOTOR_A_PWM, val);
    }
    else
    {
      digitalWrite(MOTOR_A_DIR, LOW);
      digitalWrite(MOTOR_A_PWM, LOW);
    }
  }
  else if (id==2)
  {
    if (1 <= _power && _power <= 100)
    {
      // Forward
      digitalWrite(MOTOR_B_DIR, HIGH);
      byte val = map(_power, 0, 100, 0, 255);
      analogWrite(MOTOR_B_PWM, val);
    }
    else if (-100 <= _power && _power <= -1) {
      // Backward
      digitalWrite(MOTOR_B_DIR, LOW);
      byte val = map(-_power, 0, 100, 0, 255);
      analogWrite(MOTOR_B_PWM, val);
    }
    else 
    {
      digitalWrite(MOTOR_B_DIR, LOW);
      digitalWrite(MOTOR_B_PWM, LOW);
    }
  }
}
void initColorLED()
{
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
}
void SetColorLED(int r, int g, int b) {
  digitalWrite(LED_R, r);
  digitalWrite(LED_G, g);
  digitalWrite(LED_B, b);
}
 
void reportSensorToSiCi() {
  int ir = analogRead(A0);
  ir = ir * 0.1;
  if (ir > 100)
    ir = 100;
  ucr.sendIrSensor(1, ir);
  if (ir > 50)
    digitalWrite(LINE_LED0, LOW);
  else
    digitalWrite(LINE_LED0, HIGH);
  ir = analogRead(A1);
  ir = ir * 0.1;
  if (ir > 100)
    ir = 100;
  ucr.sendIrSensor(2, ir);
  if (ir > 50)
    digitalWrite(LINE_LED1, LOW);
  else
    digitalWrite(LINE_LED1, HIGH);
   
  int range = GetRangeSR04();
  ucr.sendRangeSensor(3, range);
}
 
void setup() {
  Serial.begin(57600);
   
  init2DC();
   
  InitSR04();
   
  initColorLED();
   
  pinMode(LINE_LED0, OUTPUT);
  pinMode(LINE_LED1, OUTPUT);
   
  simpleTimer.setInterval(100, timer);
}
 
void loop() {
  simpleTimer.run();
  while (ucr.count() > 0) {
    sProtocol protocol = ucr.dequeue();
    switch (protocol.cmd)
    {
      case MS_DEVICE_DC: // DC Motor
      {
        if (protocol.id < DC_MAX)
        {
          SetMotorPower(protocol.id, protocol.value);
        }
        break;
      }
      case MS_DEVICE_INFO:
      {
        // Input devices
        ucr.sendDeviceInfo(1, MS_SENSOR_IR);
        ucr.sendDeviceInfo(2, MS_SENSOR_IR);
        ucr.sendDeviceInfo(3, MS_SENSOR_DISTANCE);
         
        // Output devices
        ucr.sendDeviceInfo(1, MS_DEVICE_DC);
        ucr.sendDeviceInfo(2, MS_DEVICE_DC);
        ucr.sendDeviceInfo(0, 0);
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
