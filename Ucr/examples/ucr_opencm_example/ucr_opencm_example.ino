#include "Ucr4OpenCM.h"

Ucr4OpenCM ucr;

#define DXL_BUS_SERIAL1 1  //Dynamixel on Serial1(USART1)  <-OpenCM9.04
#define DXL_BUS_SERIAL2 2  //Dynamixel on Serial2(USART2)  <-LN101,BT210
#define DXL_BUS_SERIAL3 3  //Dynamixel on Serial3(USART3)  <-OpenCM 485EXP

Dynamixel Dxl(DXL_BUS_SERIAL1);

HardwareTimer Timer(1);

void SetMotorAngle(byte id, byte angle) {
  unsigned int position = angle * 1024 / 360;
  Dxl.goalPosition(id, position);
}
 
void reportToSiCi() {
}
 
void setup() {
  Dxl.begin(3);
  
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
 
void loop() {
  while (ucr.count() > 0) {
    sProtocol protocol;// = ucr.dequeue();
    switch (protocol.cmd)
    {
      case MS_DEVICE_SERVO: // DC Motor
      {
        SetMotorAngle(protocol.id, protocol.value);
        break;
      }
      case MS_DEVICE_INFO:
      {
        // Input devices
        UcrSend( ucr.buffDeviceInfo(1, MS_SENSOR_ANGLE) );

        UcrSend( ucr.buffDeviceInfo(1, MS_DEVICE_SERVO) );
        
        UcrSend( ucr.buffDeviceInfo(0, 0) );
        break;
      }
    }
  }
}

void UcrSend (unsigned char *buff)
{
  SerialUSB.write(buff, buff[1]+2);
}

void serialEvent() {
  while (SerialUSB.available()) {
    ucr.pushByte (SerialUSB.read());
  }
}
