/*
 * Ucr4OpenCM.cpp
 * Copyright (C) 2014~2015 MakeSense. All rights reserved.
*/

#include "Ucr4OpenCM.h"

Ucr4OpenCM::Ucr4OpenCM()
{
	_front = 0;
	_rear = 0;
}

unsigned char *Ucr4OpenCM::buffMotorAngle(int id, int angle) {
	_send_buff[0] = 0xaa;
	_send_buff[1] = 0x05;
	_send_buff[2] = MS_SENSOR_ANGLE;
	_send_buff[3] = id;
	_send_buff[4] = (unsigned char)(angle&0x00ff);
	_send_buff[5] = (unsigned char)(angle>>8 & 0x00ff);
	_send_buff[6] = _getChecksum(_send_buff);
	return _send_buff;
}

unsigned char *Ucr4OpenCM::buffTouchSensor(int id, int value) {
	_send_buff[0] = 0xaa;
	_send_buff[1] = 0x05;
	_send_buff[2] = MS_SENSOR_TOUCH;
	_send_buff[3] = id;
	_send_buff[4] = (unsigned char)value;
	_send_buff[5] = (unsigned char)(value>>8);
	_send_buff[6] = _getChecksum(_send_buff);
	return _send_buff;
}

unsigned char *Ucr4OpenCM::buffIrSensor(int id, int value) {
	_send_buff[0] = 0xaa;
	_send_buff[1] = 0x05;
	_send_buff[2] = MS_SENSOR_IR;
	_send_buff[3] = id;
	_send_buff[4] = (unsigned char)value;
	_send_buff[5] = (unsigned char)(value>>8);
	_send_buff[6] = _getChecksum(_send_buff);
	return _send_buff;
}

unsigned char *Ucr4OpenCM::buffRangeSensor(int id, int centimeter) {
	_send_buff[0] = 0xaa;
	_send_buff[1] = 0x05;
	_send_buff[2] = MS_SENSOR_DISTANCE;
	_send_buff[3] = id;
	_send_buff[4] = (unsigned char)centimeter;
	_send_buff[5] = (unsigned char)(centimeter>>8);
	_send_buff[6] = _getChecksum(_send_buff);
	return _send_buff;
}

unsigned char *Ucr4OpenCM::buffBatteryPercent(int id, int percent) {
	_send_buff[0] = 0xaa;
	_send_buff[1] = 0x05;
	_send_buff[2] = MS_SENSOR_BATTERY;
	_send_buff[3] = id;
	_send_buff[4] = (unsigned char)percent;
	_send_buff[5] = (unsigned char)(percent>>8);
	_send_buff[6] = _getChecksum(_send_buff);
	return _send_buff;
}

unsigned char *Ucr4OpenCM::buffDeviceInfo(int id, int type) {
	_send_buff[0] = 0xaa;
	_send_buff[1] = 0x05;
	_send_buff[2] = 0x0a;
	_send_buff[3] = 0x01;
	_send_buff[4] = id;
	_send_buff[5] = type;
	_send_buff[6] = _getChecksum(_send_buff);
	return _send_buff;
}

void Ucr4OpenCM::pushByte(unsigned char data) {
	_update(data);
}

int Ucr4OpenCM::count()
{
	return ((UCR_QUEUE_SIZE + _rear) - _front) % UCR_QUEUE_SIZE;
}

sProtocol Ucr4OpenCM::dequeue()
{
	if (count() > 0) {
		sProtocol protocol;
		protocol = _protocol[_front];
		_front++;
		if (_front >= UCR_QUEUE_SIZE)
			_front = 0;
		return _protocol[_front-1];
	}
}

// Private Methods /////////////////////////////////////////

void Ucr4OpenCM::_update(unsigned char inChar) {
	switch (_state) 
	{
		case START :  // Ready to Start
		{
			if (inChar == 0xAA) 
				_state = LENGTH;
			break;
		}

		case LENGTH :
		{
			if (inChar < 0xAA) 
			{
				_buff_len = inChar;
				_buff_cnt = 0;
				_state = DATA;
			}
			else if (inChar > 0xAA){
				_state = START;
			}
			break;
		}

		case DATA :
		{
			_buff[_buff_cnt] = inChar;
			_buff_cnt++;
			if (_buff_cnt >= _buff_len) 
			{
				// Checksum
				char sum = 0;
				for (int i = 0 ; i < _buff_len ; i++)
					sum += _buff[i];
         
				if (sum == 0) 
				{
					_protocol[_rear].cmd = _buff[0];
					_protocol[_rear].id = _buff[1];
					if (_buff_len==4) 
					{
						_protocol[_rear].value = _buff[2];
					}
					else if (_buff_len==5) 
					{
						_protocol[_rear].value = _buff[3];
						_protocol[_rear].value = _protocol[_rear].value<<8;
						_protocol[_rear].value += _buff[2];
					}
				}
				_rear++;
				if (_rear >= UCR_QUEUE_SIZE)
					_rear = 0;
				_state = START;
			}
			break;
		}

		default :
		{
			_state = START;
			break;
		}
	}
}

unsigned char Ucr4OpenCM::_getChecksum(unsigned char *buff) {
	unsigned char checksum = 0;
	for (int i = 2 ; i < buff[1]+1 ; i++)
		checksum += buff[i];
	return (unsigned char) (0-checksum);
}
