/*
 * Ucr.h
 * Copyright (C) 2014 MakeSense. All rights reserved.
 */

#ifndef UCR_H
#define UCR_H

#include "Arduino.h"

#define MS_DEVICE_DC		0x31
#define MS_DEVICE_SERVO		0x41

#define MS_SENSOR_ANGLE		0x45
#define MS_SENSOR_DISTANCE	0x93
#define MS_SENSOR_IR		0x96

#define MS_DEVICE_INFO		0x0a

typedef void (*timer_callback)(void);

typedef enum {
  START,
  LENGTH,
  DATA,  
} PROTOCOL_STATE;

// ---- Communication
typedef struct {
  byte cmd;
  byte id;
  word value;
} sProtocol;
// ---- End

class Ucr {
public:
	Ucr();

	int setReportFunction(timer_callback f);

	void sendMotorAngle(int id, int value);
	void sendIrSensor(int id, int value);
	void sendRangeSensor(int id, int value);
	void sendDeviceInfo(int id, int type);

	void pushByte(byte data);
	int count();
	sProtocol dequeue();

private:
	void _update(byte inChar);
	byte _getChecksum(byte *buff);

	PROTOCOL_STATE _state;
	byte _buff[256];
	byte _buff_cnt;
	byte _buff_len;

	sProtocol _protocol[256];
	byte _front;
	byte _rear;
};

#endif