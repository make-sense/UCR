#include "Ucr4OpenCM.h"

Ucr4OpenCM ucr;

#define DXL_ID 1

#define DXL_BUS_SERIAL1 1  //Dynamixel on Serial1(USART1)  <-OpenCM9.04
#define DXL_BUS_SERIAL2 2  //Dynamixel on Serial2(USART2)  <-LN101,BT210
#define DXL_BUS_SERIAL3 3  //Dynamixel on Serial3(USART3)  <-OpenCM 485EXP

Dynamixel Dxl(DXL_BUS_SERIAL1);

HardwareTimer Timer(1);

void reportToSiCi() {
  int pos = Dxl.readWord(DXL_ID, 36);  // PRESENT_POSSITION
  int angle = (float)pos * 0.35f;
  if (pos < 1024)
  {
    sendUcr( ucr.buffMotorAngle(DXL_ID, angle) );
  }
//    SerialUSB.write("position:");
//    SerialUSB.print(pos);
//    SerialUSB.write(", angle:");
//    SerialUSB.println(angle);
}

void initTimer () {
  // Pause the timer while we're configuring it
  Timer.pause();

  // Set up period
  Timer.setPeriod(100000); // in microseconds

  // Set up an interrupt on channel 1
  Timer.setMode(TIMER_CH1, TIMER_OUTPUT_COMPARE);
  Timer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  Timer.attachInterrupt(TIMER_CH1, reportToSiCi);

  // Refresh the timer's count, prescale, and overflow
  Timer.refresh();

  // Start the timer counting
  Timer.resume();
}

void setup() {
  Serial2.begin(57600);
  Serial2.attachInterrupt(serialInterrupt);

  Dxl.begin(3);                  // DXL communication speed to 1M
  Dxl.jointMode(DXL_ID);         // DXL Joint Mode
  Dxl.writeByte(DXL_ID, 24, 1);  // DXL Torque On

  initTimer ();
}
 
void loop() {
  while (ucr.count() > 0) {
    sProtocol protocol = ucr.dequeue();
    switch (protocol.cmd)
    {
      case MS_DEVICE_SERVO: // DC Motor
      {
        SetMotorAngle(protocol.id, protocol.value);
        break;
      }
      case MS_DEVICE_INFO:
      {
        sendUcr( ucr.buffDeviceInfo(DXL_ID, MS_SENSOR_ANGLE) );

        sendUcr( ucr.buffDeviceInfo(DXL_ID, MS_DEVICE_SERVO) );
        
        sendUcr( ucr.buffDeviceInfo(0, 0) );
        break;
      }
    }
  }
}

void serialInterrupt(byte buffer){
    ucr.pushByte (buffer);
}

void sendUcr (unsigned char *buff)
{
  Serial2.write(buff, buff[1]+2);
}

void SetMotorAngle(byte id, unsigned int angle) {
  unsigned int position = (float)angle * 2.84f;
  Dxl.goalPosition(id, position);
}


