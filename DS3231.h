/*
 * DS3231.h
 *
 *  Created on: 06-Mar-2021
 *      Author: Dhruv Joshi
 */

#ifndef DS3231TEMPERATURESENSOR_H_
#define DS3231TEMPERATURESENSOR_H_
#define buffAddressArray 0x19

class TempSensor {
private:
	int I2CBus;
	int I2cAddress;
	char dataBuffer[buffAddressArray];
public:
	TempSensor(int, int);
	virtual ~TempSensor();
	int sensorData();
	int writeI2cDevice(char, char);
	int setTime();
	int alarm_1();
	int alarm_2(int, int);
	int tempReadings();
	int BBSQWEnabler(double);
	int deactivateAlarm();
	void displayTime();
};

#endif /* DS3231_H_ */
