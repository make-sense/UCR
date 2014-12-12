/*--------------------------------------------------------------------------------
 * Author	   : makesense.cc (info@makesense.cc)
 * Date		   : 2014-10-13
 * Copyright       : 2013-2014 makesense.cc
 *
 * Description     : This is a basic example for makesense base shield for using SiCi.
 * Arduino Model   : Arduino Uno R3
 *
 *--------------------------------------------------------------------------------
 */

#include <Ucr.h>
#include <Servo.h>

Ucr ucr;

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

void sendToSiCi() {
  int value;
  value = digitalRead(2);
  ucr.sendIrSensor(11, value);
  value = digitalRead(3);
  ucr.sendIrSensor(12, value);
  value = analogRead(A0);
  ucr.sendIrSensor(13, value);
  value = analogRead(A1);
  ucr.sendIrSensor(14, value);
  value = analogRead(A2);
  ucr.sendIrSensor(15, value);
  value = analogRead(A3);
  ucr.sendIrSensor(16, value);
  value = analogRead(A4);
  ucr.sendIrSensor(17, value);
  value = analogRead(A5);
  ucr.sendIrSensor(18, value);
}

void setup() {
  Serial.begin(57600);

  // Servo motors
  servo1.attach(4);
  servo2.attach(7);
  servo3.attach(8);
  servo4.attach(12);
  
  // Button
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  // LED
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);

  ucr.setReportFunction(sendToSiCi);
}

void loop() {
  ucr.runTimer();
  while (ucr.count() > 0) {
    sProtocol protocol = ucr.dequeue();
    switch (protocol.cmd)
    {
      case MS_DEVICE_DC:  // DC Motor
      {
        int velocity = (int)protocol.value;
        switch (protocol.id) {
          case 1:
            servo1.write(velocity);
            break;
          case 2:
            servo2.write(velocity);
            break;
        }
        break;
      }
      case MS_DEVICE_SERVO:  // Servo Motor
      {
        int angle = (int)protocol.value;
        switch (protocol.id) {
          case 3:
            servo3.write(angle);
            break;
          case 4:
            servo4.write(angle);
            break;
        }
        break;
      }
      case MS_DIGITAL_OUT:
      {
        int value = (int)protocol.value;
        switch(protocol.id) {
          case 5:
          {
            (value & 0x02)? digitalWrite( 5, HIGH): digitalWrite( 5, LOW);
            (value & 0x01)? digitalWrite(10, HIGH): digitalWrite(10, LOW);
            break;
          }
          case 6:
          {
            (value & 0x02)? digitalWrite( 6, HIGH): digitalWrite( 6, LOW);
            (value & 0x01)? digitalWrite(11, HIGH): digitalWrite(11, LOW);
            break;
          }
          case 7:
          {
            (value & 0x02)? digitalWrite( 9, HIGH): digitalWrite( 9, LOW);
            (value & 0x01)? digitalWrite(13, HIGH): digitalWrite(13, LOW);
            break;
          }
        }
        break;
      }
      case MS_DEVICE_INFO:
      {
        // output devices
        ucr.sendDeviceInfo(1, MS_DEVICE_SERVO); // Servo Motor - D4
        ucr.sendDeviceInfo(2, MS_DEVICE_SERVO); // Servo Motor - D7
        ucr.sendDeviceInfo(3, MS_DEVICE_SERVO); // Servo Motor - D8
        ucr.sendDeviceInfo(4, MS_DEVICE_SERVO); // Servo Motor - D12
        ucr.sendDeviceInfo(5, MS_DIGITAL_OUT); // LED - R
        ucr.sendDeviceInfo(6, MS_DIGITAL_OUT); // LED - G
        ucr.sendDeviceInfo(7, MS_DIGITAL_OUT); // LED - B

        // input devices
        ucr.sendDeviceInfo(11, MS_SENSOR_IR); // Switch D2
        ucr.sendDeviceInfo(12, MS_SENSOR_IR); // Switch D3
        ucr.sendDeviceInfo(13, MS_SENSOR_IR);  // Potentiometer
        ucr.sendDeviceInfo(14, MS_SENSOR_IR);  // Potentiometer
        ucr.sendDeviceInfo(15, MS_SENSOR_IR);  // Light Sensor
        ucr.sendDeviceInfo(16, MS_SENSOR_IR);  // Analog Input
        ucr.sendDeviceInfo(17, MS_SENSOR_IR);  // Analog Input
        ucr.sendDeviceInfo(18, MS_SENSOR_IR);  // Analog Input
       
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
