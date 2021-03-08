/*
 * DS3231.cpp
 *
 *  Created on: 06-Mar-2021
 *      Author: Dhruv Joshi
 */

#include <bits/types/struct_tm.h>
#include <bits/types/time_t.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ctime>
#include <iostream>
#include <vector>
#include "DS3231.h"

#define CLOCK_ADDRESS  0x68
#define ADDR  char(CLOCK_ADDRESS<<1)

#define BUFFER_SIZE 19

using namespace std;

int bcdToDec(char b) {
	return (b / 16) * 10 + (b % 16);
}
char decToBcd(int b) {
	return (((b / 10) << 4) | (b % 10));
}
void decToBinary(int n) {
	// array to store binary number
	int binaryNum[32];

	// counter for binary array
	int i = 0;
	while (n > 0) {

		// storing remainder in binary array
		binaryNum[i] = n % 2;
		n = n / 2;
		i++;
	}

	// printing binary array in reverse order
	for (int j = i - 1; j >= 0; j--)
		cout << binaryNum[j];
}

int setBitOne(int n, int k) {
	//n |= 1 << 3;

	return n | (1 << (k - 1));
}

int setBitZero(int n, int k) {
	//n |= 1 << 3;

	return n & ~(1u << (k - 1));
}

TempSensor::TempSensor(int I2CBus, int I2CAddress) {

	this->I2CBus = I2CBus;
	this->I2cAddress = I2CAddress;
	sensorData();
}

/*
 * This method the reads the value from DS3231 and stores it in dataBuffer array.
 */
int TempSensor::sensorData() {
	int file;
	if ((file = open("/dev/i2c-1", O_RDWR)) < 0) {
		cout << "Failed to open DS3231 Sensor on /dev/i2c-1" << " I2C Bus"
				<< endl;
		return (1);
	}
	if (ioctl(file, I2C_SLAVE, this->I2cAddress) < 0) {
		cout << "I2C_SLAVE address " << this->I2cAddress << " failed..."
				<< endl;
		return (2);
	}

	char writerBuffer[1] = { 0x00 };
	if (write(file, writerBuffer, 1) != 1) {
		cout << "Failed to Reset Address in readValue() " << endl;
		return 3;
	}

	int number = buffAddressArray;
	int bytesRead = read(file, this->dataBuffer, number);
	if (bytesRead == -1) {
		cout << "Failure to read Byte Stream in readSensorState()" << endl;
	}

	close(file);
	return 0;
}

/*
 * This method essentially writes data to a particular address --->
 */
int TempSensor::writeI2cDevice(char address, char value) {
	int file;
	if ((file = open("/dev/i2c-1", O_RDWR)) < 0) {
		cout << "Failed to open DS3231 Sensor on /dev/i2c-1" << " I2C Bus"
				<< endl;
		return (1);
	}
	if (ioctl(file, I2C_SLAVE, this->I2cAddress) < 0) {
		cout << "I2C_SLAVE address " << this->I2cAddress << " failed..."
				<< endl;
		return (2);
	}

	char writerBuffer[2];
	writerBuffer[0] = address;
	writerBuffer[1] = value;
	if (write(file, writerBuffer, 2) != 2) {
		cout << "Failed to write on i2c at  " << address << endl;
		return 3;
	}

	char resetBuffer[1] = { 0x00 };
	if (write(file, resetBuffer, 1) != 1) {
		cout << "Failed to Reset Address in writeValue() " << endl;
		return 4;
	}
	close(file);

	return 0;
}

int TempSensor::setTime() {

	vector<int> calender;

	time_t now = time(0);
	tm *ltm = localtime(&now);
	calender.push_back(1 + ltm->tm_sec);
	calender.push_back(1 + ltm->tm_min);
	calender.push_back(ltm->tm_hour);
	calender.push_back(1 + ltm->tm_wday);
	calender.push_back(ltm->tm_mday);
	calender.push_back(1 + ltm->tm_mon);
	calender.push_back(20);

	this->sensorData();
	for (int i = 0; i < calender.size(); i++) {
		this->writeI2cDevice(0x0 + i, decToBcd(calender[i]));
	}
	return 0;
}

int TempSensor::alarm_1() {

	//setting bit INTCN and A1IE
	this->sensorData();
	int _0e = bcdToDec(dataBuffer[0x0e]);
	_0e = setBitOne(_0e, 3);
	_0e = setBitOne(_0e, 1);
	decToBinary(_0e);
	cout << "0e ->>>" << endl;
	writeI2cDevice( { 0x0e }, decToBcd(_0e));
	// setting control/status A1F
	this->sensorData();

	//this->readSensorState();
	this->writeI2cDevice(0x07, setBitOne(bcdToDec(dataBuffer[0x07]), 8));
	this->writeI2cDevice(0x08, setBitOne(bcdToDec(dataBuffer[0x08]), 8));
	this->writeI2cDevice(0x09, setBitOne(bcdToDec(dataBuffer[0x09]), 8));
	this->writeI2cDevice(0x0a, setBitOne(bcdToDec(dataBuffer[0x0a]), 8));

	return 0;

}

int TempSensor::deactivateAlarm() {

	this->sensorData();
	int _0e = bcdToDec(dataBuffer[0x0e]);
	_0e = setBitZero(_0e, 3);
	_0e = setBitZero(_0e, 1);
	//_0e = setBitZero(_0e, 2);
	//_0e = setBitZero(_0e, 4);
	decToBinary(_0e);
	cout << "0e ->>>" << endl;
	writeI2cDevice( { 0x0e }, decToBcd(_0e));
	// setting control/status A1F
	this->sensorData();

	this->writeI2cDevice(0x07, setBitZero(bcdToDec(dataBuffer[0x07]), 8));
	this->writeI2cDevice(0x08, setBitZero(bcdToDec(dataBuffer[0x08]), 8));
	this->writeI2cDevice(0x09, setBitZero(bcdToDec(dataBuffer[0x09]), 8));
	this->writeI2cDevice(0x0a, setBitZero(bcdToDec(dataBuffer[0x0a]), 8));

	return 0;
}

/**
 * this alarm is intended to work when timekeeping registers matches seconds and minuted
 */
int TempSensor::alarm_2(int seconds, int minutes) {

	this->sensorData();
	int a1m1 = bcdToDec(dataBuffer[0x07]);
	cout << "decimal value of a1m1" << a1m1 << endl;
	a1m1 = setBitZero(a1m1, 8);
	a1m1 = setBitZero(a1m1, 1);
	a1m1 = setBitZero(a1m1, 2);
	a1m1 = setBitZero(a1m1, 3);
	a1m1 = setBitZero(a1m1, 4);
	decToBinary(a1m1);
	cout << "a1m1 ->>>" << endl;

	int a1m2 = bcdToDec(dataBuffer[0x08]);
	cout << "decimal value of a2m2" << a1m2 << endl;
	a1m2 = setBitZero(a1m2, 8);
	a1m2 = setBitZero(a1m2, 1);
	a1m2 = setBitZero(a1m2, 2);
	a1m2 = setBitZero(a1m2, 3);
	a1m2 = setBitZero(a1m2, 4);
	decToBinary(a1m2);
	cout << "a1m2 ->>>" << endl;

	int a1m3 = bcdToDec(dataBuffer[0x09]);
	a1m3 = setBitOne(a1m3, 8);
	decToBinary(a1m3);
	cout << "a1m3 ->>>" << endl;

	int a1m4 = bcdToDec(dataBuffer[0x0a]);
	a1m4 = setBitOne(a1m4, 8);
	decToBinary(a1m4);
	cout << "a1m4 ->>>" << endl;

	int _0f = bcdToDec(dataBuffer[0x0f]);
	_0f = setBitOne(_0f, 1);
	decToBinary(_0f);
	cout << "0f ->>>" << endl;

	int _0e = bcdToDec(dataBuffer[0x0e]);
	_0e = setBitOne(_0e, 4);
	_0e = setBitOne(_0e, 1);
	decToBinary(_0e);
	cout << "0e ->>>" << endl;
	writeI2cDevice( { 0x0e }, decToBcd(_0e));

	writeI2cDevice( { 0x0f }, decToBcd(_0f));
	writeI2cDevice( { 0x07 }, decToBcd(a1m1));
	writeI2cDevice( { 0x08 }, decToBcd(a1m2));
	writeI2cDevice( { 0x09 }, decToBcd(a1m3));
	writeI2cDevice( { 0x0a }, decToBcd(a1m4));

	sensorData();

	writeI2cDevice( { 0x07 }, decToBcd(bcdToDec(dataBuffer[0x07]) + seconds));
	writeI2cDevice( { 0x08 }, decToBcd(bcdToDec(dataBuffer[0x08]) + minutes));
//	writeI2cDevice( { 0x00 }, decToBcd(50));
//	writeI2cDevice( { 0x01 }, decToBcd(3));

	return 0;
}

float getTemperature(int adc_value) {
	cout << adc_value << " val " << endl;
	float cur_voltage = adc_value * (3.30f / 4096.0f);
	float diff_degreesC = (cur_voltage - 0.75f) / 0.01f;
	return (25.0f + diff_degreesC);
}

int TempSensor::tempReadings() {
	int file;
	printf("Starting the DS3231 test application\n");
	if ((file = open("/dev/i2c-1", O_RDWR)) < 0) {
		perror("failed to open the bus\n");
		return 1;
	}
	if (ioctl(file, I2C_SLAVE, 0x68) < 0) {
		perror("Failed to connect to the sensor\n");
		return 1;
	}
	char writeBuffer[1] = { 0x00 };
	if (write(file, writeBuffer, 1) != 1) {
		perror("Failed to reset the read address\n");
		return 1;
	}
	char buf[BUFFER_SIZE];
	if (read(file, buf, BUFFER_SIZE) != BUFFER_SIZE) {
		perror("Failed to read in the buffer\n");
		return 1;
	}
	printf("The RTC time is %02d:%02d:%02d\n", bcdToDec(buf[2]),
			bcdToDec(buf[1]), bcdToDec(buf[0]));
	// note that 0x11 = 17 decimal and 0x12 = 18 decimal
	float temperature = buf[0x11] + ((buf[0x12] >> 6) * 0.25);
	printf("The temperature is %f°C\n", temperature);
	close(file);
	return 0;
}

/**
 * Square wave form of 1.024 KHz frequency -> enable BBSQW register and RS1 for 1 KHz, 1.024 KHz, 4.096 KHZ,  8.192 KHZ
 */
int TempSensor::BBSQWEnabler(double frequency) {

	this->sensorData();
	int _0e = bcdToDec(dataBuffer[0x0e]);
	_0e = setBitZero(_0e, 3);

	if (frequency == 1.0) {
		_0e = setBitOne(_0e, 6);
	}

	if (frequency == 1.024) {
		_0e = setBitOne(_0e, 6);
		_0e = setBitOne(_0e, 3);
	}

	if (frequency == 4.096) {
		_0e = setBitOne(_0e, 6);
		_0e = setBitOne(_0e, 4);
	}

	if (frequency == 8.192) {
		_0e = setBitOne(_0e, 6);
		_0e = setBitOne(_0e, 3);
		_0e = setBitOne(_0e, 4);
	}

//		_0e = setBitOne(_0e, 1);
	decToBinary(_0e);
	cout << "0e ->>>" << endl;
	writeI2cDevice( { 0x0e }, decToBcd(_0e));

	return 0;
}

void TempSensor::displayTime() {

	this->sensorData();
//week = dataBuffer[0x03];
	cout << "Year" << bcdToDec(dataBuffer[0x06]) << endl;
	cout << "Month: " << bcdToDec(dataBuffer[0x05]) << endl;
	cout << "Date: " << bcdToDec(dataBuffer[0x04]) << endl;
	cout << "Time: " << bcdToDec(dataBuffer[0x02]) << ":";
	cout << bcdToDec(dataBuffer[0x01]) << ":";
	cout << bcdToDec(dataBuffer[0x00]) << endl;
//cout << "Day of the week" << week << endl;
}
TempSensor::~TempSensor() {
// TODO Auto-generated destructor stub
}

