/*--------------------------------------------------------------------------------
 * Author	   : makesense.cc (info@makesense.cc)
 * Date		   : 2014-03-12
 * Copyright       : 2013-2014 makesense.cc
 *
 * Description     : This is a basic example for Rokit SmartServoBox for using SiCi.
 *
 *--------------------------------------------------------------------------------
 */

#include <Ucr.h>

#define DIGITAL_OUT_1 16
#define DIGITAL_OUT_2 17
#define DIGITAL_OUT_3 18
#define DIGITAL_OUT_4 19
#define DIGITAL_OUT_5 20
#define DIGITAL_OUT_6 21
#define DIGITAL_OUT_7 22
#define DIGITAL_OUT_8 23

Ucr ucr;

void reportSensorToSiCi() {
  int analog;
  analog = analogRead(A0);
  ucr.sendIrSensor(1, analog);
  analog = analogRead(A1);
  ucr.sendIrSensor(2, analog);
  analog = analogRead(A2);
  ucr.sendIrSensor(3, analog);
}

void setup() {
  Serial.begin(57600);

  pinMode(DIGITAL_OUT_1, OUTPUT);
  pinMode(DIGITAL_OUT_2, OUTPUT);
  pinMode(DIGITAL_OUT_3, OUTPUT);
  pinMode(DIGITAL_OUT_4, OUTPUT);
  pinMode(DIGITAL_OUT_5, OUTPUT);
  pinMode(DIGITAL_OUT_6, OUTPUT);
  pinMode(DIGITAL_OUT_7, OUTPUT);
  pinMode(DIGITAL_OUT_8, OUTPUT);

  ucr.setReportFunction(reportSensorToSiCi);
}

void loop() {
  ucr.runTimer();
  while (ucr.count() > 0)
  {
    sProtocol protocol = ucr.dequeue();
    switch (protocol.cmd) 
    {
      case 0xa1:  // Digital Output
      {
        switch (protocol.id) {
          case 1:
            digitalWrite(DIGITAL_OUT_1, protocol.value);
            break;
          case 2:
            digitalWrite(DIGITAL_OUT_2, protocol.value);
            break;
          case 3:
            digitalWrite(DIGITAL_OUT_3, protocol.value);
            break;
          case 4:
            digitalWrite(DIGITAL_OUT_4, protocol.value);
            break;
          case 5:
            digitalWrite(DIGITAL_OUT_5, protocol.value);
            break;
          case 6:
            digitalWrite(DIGITAL_OUT_6, protocol.value);
            break;
          case 7:
            digitalWrite(DIGITAL_OUT_7, protocol.value);
            break;
          case 8:
            digitalWrite(DIGITAL_OUT_8, protocol.value);
            break;
        }
        break;
      }
      case MS_DEVICE_INFO:
      {
        ucr.sendDeviceInfo(MS_DIGITAL_OUT, 1);  // Digital Output
        ucr.sendDeviceInfo(MS_DIGITAL_OUT, 2);
        ucr.sendDeviceInfo(MS_DIGITAL_OUT, 3);
        ucr.sendDeviceInfo(MS_DIGITAL_OUT, 4);
        ucr.sendDeviceInfo(MS_DIGITAL_OUT, 5);
        ucr.sendDeviceInfo(MS_DIGITAL_OUT, 6);
        ucr.sendDeviceInfo(MS_DIGITAL_OUT, 7);
        ucr.sendDeviceInfo(MS_DIGITAL_OUT, 8);

        ucr.sendDeviceInfo(MS_SENSOR_IR, 1);  // InfraRed Sensor
        ucr.sendDeviceInfo(MS_SENSOR_IR, 2);
        ucr.sendDeviceInfo(MS_SENSOR_IR, 3);

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
