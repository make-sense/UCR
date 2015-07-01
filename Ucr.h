/*
 * Ucr.h
 * Copyright (C) 2014 MakeSense. All rights reserved.
 */

#ifndef UCR_H
#define UCR_H

#include "Arduino.h"

#define MS_RESET			0x01
#define MS_COMPANY_ID		0x02
#define MS_PRODUCT_ID		0x03
#define MS_VERSION_MAJOR	0x04
#define MS_VERSION_MINOR	0x05

#define MS_DEVICE_DC		0x31
#define MS_DEVICE_SERVO		0x41
#define MS_DEVICE_SERVO_SET	0x42
#define MS_DIGITAL_OUT		0xa1

#define MS_SENSOR_ANGLE		0x45
#define MS_SENSOR_TOUCH		0x91
#define MS_SENSOR_DISTANCE	0x93
#define MS_SENSOR_IR		0x96
#define MS_SENSOR_BATTERY	0xb1
#define MS_DIGITAL_IN		0xd1
#define MS_ANALOG_IN		0xd2

#define MS_DEVICE_INFO		0x0a

#define UCR_QUEUE_SIZE		64

typedef void (*timer_callback)(void);

typedef enum {
  START,
  LENGTH,
  DATA,  
} PROTOCOL_STATE;

typedef struct {
  byte cmd;
  byte id;
  word value;
} sProtocol;

class Ucr {
public:
	Ucr();

	int setReportFunction(timer_callback f);

	void sendMotorAngle(int id, int angle);
	void sendTouchSensor(int id, int value);
	void sendIrSensor(int id, int value);
	void sendRangeSensor(int id, int centimeter);
	void sendBatteryPercent(int id, int percent);
	void sendDeviceInfo(int id, int type);

	void pushByte(byte data);
	int count();
	sProtocol dequeue();

	void runTimer();

private:
	void _update(byte inChar);
	byte _getChecksum(byte *buff);

	unsigned long _prev_millis;
	timer_callback *callback;


	PROTOCOL_STATE _state;
	byte _buff[16];
	byte _buff_cnt;
	byte _buff_len;

	sProtocol _protocol[UCR_QUEUE_SIZE];
	int _front;
	int _rear;
};

#endif