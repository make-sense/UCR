/*
 * Ucr4OpenCM.h
 * Copyright (C) 2014~2015 MakeSense. All rights reserved.
 */

#ifndef UCR4OPENCM_H
#define UCR4OPENCM_H

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

typedef enum {
  START,
  LENGTH,
  DATA,  
} PROTOCOL_STATE;

typedef struct {
  unsigned char cmd;
  unsigned char id;
  unsigned short value;
} sProtocol;

class Ucr4OpenCM {
public:
	Ucr4OpenCM();

	unsigned char *buffMotorAngle(int id, int angle);
	unsigned char *buffTouchSensor(int id, int value);
	unsigned char *buffIrSensor(int id, int value);
	unsigned char *buffRangeSensor(int id, int centimeter);
	unsigned char *buffBatteryPercent(int id, int percent);
	unsigned char *buffDeviceInfo(int id, int type);

	void pushByte(unsigned char data);
	int count();
	sProtocol dequeue();

	void runTimer();

private:
	unsigned char _send_buff[7];

	void _update(unsigned char inChar);
	unsigned char _getChecksum(unsigned char *buff);

	PROTOCOL_STATE _state;
	unsigned char _buff[7];
	unsigned char _buff_cnt;
	unsigned char _buff_len;

	sProtocol _protocol[UCR_QUEUE_SIZE];
	unsigned char _front;
	unsigned char _rear;
};

#endif