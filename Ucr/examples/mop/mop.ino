#include "Ucr4OpenCM.h"

#define MOT_ENA  10
#define MOT_IN1  18
#define MOT_IN2  19
#define MOT_IN3  20
#define MOT_IN4  21
#define MOT_ENB  13

Ucr4OpenCM ucr;

#define DXL_MAX 8
unsigned char dxl_ids [DXL_MAX] = {1, 2, 11, 12, 13, 21, 22, 23};

#define DXL_BUS_SERIAL1 1  //Dynamixel on Serial1(USART1)  <-OpenCM9.04
#define DXL_BUS_SERIAL2 2  //Dynamixel on Serial2(USART2)  <-LN101,BT210
#define DXL_BUS_SERIAL3 3  //Dynamixel on Serial3(USART3)  <-OpenCM 485EXP

Dynamixel Dxl(DXL_BUS_SERIAL1);

void setup () {
  Serial2.begin (57600);
  Serial2.attachInterrupt (serialInterrupt);

  InitMobile ();
  InitMotion ();
}

void DisplayUcrProtocol (sProtocol protocol) {
  SerialUSB.print("[protocol]cmd:");
  SerialUSB.print(protocol.cmd);
  SerialUSB.print(", id:");
  SerialUSB.print(protocol.id);
  SerialUSB.print(", value:");
  SerialUSB.println((short)protocol.value);
}

long updateCount;
int count;
void loop () {
  updateCount++;
  if (updateCount > 60000)
  {
    updateCount = 0;
    toggleLED();
//    SerialUSB.print("update:");
//    SerialUSB.println(count++);
    for (int i = 0 ; i < DXL_MAX ; i++) 
    {
      int angle = GetMotorAngle (dxl_ids[i]);
      if (angle < 300)
        sendUcr (ucr.buffMotorAngle (dxl_ids[i], angle));
    }
  }
  
  while (ucr.count () > 0)
  {
//    SerialUSB.print("[protocol] get:");
//    SerialUSB.println(ucr.count ());
    sProtocol protocol = ucr.dequeue ();
//    DisplayUcrProtocol (protocol);
    switch (protocol.cmd)
    {
      case MS_DEVICE_DC:
      {
//        SerialUSB.println("MS_DEVICE_DC");
        short power = (short)protocol.value;
        switch (protocol.id)
        {
          case 51: // Left Wheel
            MotorPower(1, power);
            break;
          case 52: // Right Wheel
            MotorPower(2, power);
            break;
        }
        break;
      }
      case MS_DEVICE_SERVO:
      {
//        SerialUSB.println("MS_DEVICE_SERVO");
        switch (protocol.id)
        {
          case 1:
          case 2:
          case 11:
          case 12:
          case 13:
          case 21:
          case 22:
          case 23:
            SetMotorAngle (protocol.id, protocol.value);
            break;
        }
        break;
      }
      case MS_DEVICE_INFO:
      {
//        SerialUSB.println("MS_DEVICE_INFO");
        for (int i = 0 ; i < DXL_MAX ; i++)
        {
          sendUcr (ucr.buffDeviceInfo (dxl_ids[i], MS_SENSOR_ANGLE));
          sendUcr (ucr.buffDeviceInfo (dxl_ids[i], MS_DEVICE_SERVO));
        }
        sendUcr (ucr.buffDeviceInfo (51, MS_DEVICE_DC));
        sendUcr (ucr.buffDeviceInfo (52, MS_DEVICE_DC));
        sendUcr (ucr.buffDeviceInfo (0, 0));
        break;
      }
    }
  }
}

void serialInterrupt (byte buffer) {
//  Serial2.write (buffer);
  ucr.pushByte(buffer);
}

void sendUcr (unsigned char *buff) {
  Serial2.write (buff, buff[1]+2);
}

int GetMotorAngle (byte id) {
  int position = Dxl.readWord (id, 36);
  return position * 0.293f; // (300 / 1024)
}

void SetMotorAngle (byte id, unsigned int angle) {
  unsigned int position = (float)angle * 3.41f;
  Dxl.goalPosition (id, position);
}

void InitMotion () {
  Dxl.begin(3);                  // DXL communication speed to 1M
  for (int i = 0 ; i < DXL_MAX ; i++) 
  {
    Dxl.jointMode (dxl_ids[i]);         // DXL Joint Mode
    Dxl.writeByte (dxl_ids[i], 24, 1);  // DXL Torque On
  }
}

void InitMobile () {
  pinMode (MOT_ENA, PWM);
  analogWrite (MOT_ENA, 0);
  pinMode (MOT_IN1, OUTPUT);
  pinMode (MOT_IN2, OUTPUT);
  pinMode (MOT_IN3, OUTPUT);
  pinMode (MOT_IN4, OUTPUT);
  pinMode (MOT_ENB, PWM);
  analogWrite (MOT_ENB, 0);
}

void MotorPower(byte id, short power) {
  short _power = (short)power;
  if (id==1)
  {
    if (0 <= _power && _power <= 100)
    {
      // Forward
      digitalWrite (MOT_IN1, LOW);
      digitalWrite (MOT_IN2, HIGH);
      unsigned int val = _power * 655;
      analogWrite (MOT_ENA, val);
      SerialUSB.print("[DC1]:");
      SerialUSB.println(val);
    }
    else if (-100 <= _power && _power <= -1)
    {
      // Backward
      digitalWrite (MOT_IN1, HIGH);
      digitalWrite (MOT_IN2, LOW);
      unsigned int val = -_power * 655;
      analogWrite (MOT_ENA, val);
      SerialUSB.print("[DC2]:");
      SerialUSB.println(val);
    }
  }
  else if (id==2)
  {
    if (0 <= _power && _power <= 100)
    {
      // Forward
      digitalWrite (MOT_IN3, LOW);
      digitalWrite (MOT_IN4, HIGH);
      unsigned int val = _power * 655;
      analogWrite (MOT_ENB, val);
      SerialUSB.print("[DC3]:");
      SerialUSB.println(val);
    }
    else if (-100 <= _power && _power <= -1) {
      // Backward
      digitalWrite (MOT_IN3, HIGH);
      digitalWrite (MOT_IN4, LOW);
      unsigned int val = -_power * 655;
      analogWrite (MOT_ENB, val);
      SerialUSB.print("[DC4]:");
      SerialUSB.println(val);
    }
  }
}

