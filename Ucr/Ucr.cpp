/*
 * Ucr.cpp
 * Copyright (C) 2014 MakeSense. All rights reserved.
*/

#include "Ucr.h"

#define TIMER2_PRELOAD 100

timer_callback *callbacks;

Ucr::Ucr()
{
	_front = 0;
	_rear = 0;

	TCCR2A = 0;
	TCCR2B = 1<<CS22 | 1<<CS21 | 1<<CS20;
	TIMSK2 = 1<<TOIE2; //timer2 Overflow Interrupt
	TCNT2 = TIMER2_PRELOAD; //start timer
}

int Ucr::setReportFunction(timer_callback f) {
	*callbacks = f;
}

void Ucr::sendMotorAngle(int id, int value) {
	byte buff[7];
	buff[0] = 0xaa;
	buff[1] = 0x05;
	buff[2] = MS_SENSOR_ANGLE;
	buff[3] = id;
	buff[4] = (byte)(value&0x00ff);
	buff[5] = (byte)(value>>8 & 0x00ff);
	buff[6] = _getChecksum(buff);
	Serial.write(buff, buff[1]+2);
}

void Ucr::sendIrSensor(int id, int value) {
	byte buff[7];
	buff[0] = 0xaa;
	buff[1] = 0x05;
	buff[2] = MS_SENSOR_IR;
	buff[3] = id;
	buff[4] = (byte)value;
	buff[5] = (byte)(value>>8);
	buff[6] = _getChecksum(buff);
	Serial.write(buff, buff[1]+2);
}

void Ucr::sendRangeSensor(int id, int value) {
	byte buff[7];
	buff[0] = 0xaa;
	buff[1] = 0x05;
	buff[2] = MS_SENSOR_DISTANCE;
	buff[3] = id;
	buff[4] = (byte)value;
	buff[5] = (byte)(value>>8);
	buff[6] = _getChecksum(buff);
	Serial.write(buff, buff[1]+2);
}

void Ucr::sendDeviceInfo(int id, int type) {
	byte buff[7];
	buff[0] = 0xaa;
	buff[1] = 0x05;
	buff[2] = 0x0a;
	buff[3] = 0x01;
	buff[4] = id;
	buff[5] = type;
	buff[6] = _getChecksum(buff);
	Serial.write(buff, buff[1]+2);
}


void Ucr::pushByte(byte data) {
	_update(data);
}

int Ucr::count()
{
	return ((256 + _rear) - _front) % 256;
}

sProtocol Ucr::dequeue()
{
	if (count() > 0) {
		sProtocol protocol;
		protocol = _protocol[_front];
		_front++;
		return protocol;
	}
}


// Private Methods /////////////////////////////////////////

void Ucr::_update(byte inChar) {
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
			_buff_len = inChar;
			_buff_cnt = 0;
			_state = DATA;
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

byte Ucr::_getChecksum(byte *buff) {
	byte checksum = 0;
	for (int i = 2 ; i < buff[1]+1 ; i++)
		checksum += buff[i];
	return (byte) (0-checksum);
}

ISR(TIMER2_OVF_vect) //timer1 overflow interrupt vector handler
{
	//timer2 => 8 bits counter => 256 clock ticks
	//preeescaler = 1024 => this routine is called 61 (16.000.000/256/1024) times per second approximately => interruption period =  1 / 16.000.000/256/1024 = 16,384 ms
	//as we need a 20 ms interruption period but timer2 doesn't have a suitable preescaler for this, we program the timer with a 10 ms interruption period and we consider an interruption every 2 times this routine is called.
	//to have a 10 ms interruption period, timer2 counter must overflow after 156 clock ticks => interruption period = 1 / 16.000.000/156/1024 = 9,984 ms => counter initial value (TCNT) = 100
	TCNT2 = TIMER2_PRELOAD;  //reset timer
	if (callbacks != 0)
		(*callbacks)();
}