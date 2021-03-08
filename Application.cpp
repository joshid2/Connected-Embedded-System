/*
 * Application.cpp
 *
 *  Created on: 06-Mar-2021
 *      Author: Dhruv Joshi
 */

#include <iostream>
#include <stdlib.h>

#include "DS3231.h"

using namespace std;

int main(int argc, char *argv[]) {

	TempSensor DS3231 = TempSensor(1, 0x68);

	if (argc == 1) {

		cout
				<< "This application process helps to takes command-line input for various operations. "
				<< endl;
		cout << "Below listed points are the valid inputs -->" << endl;
		cout << "Enter 1: to read and display time" << endl;
		cout << "Enter 2: get the temperature reading" << endl;
		cout << "Enter 3: for generating square waveform to desired frequency"
				<< endl;
		cout << "Enter 4: activate per second alarm" << endl;
		cout << "Enter 5: set minute alarm" << endl;
		cout << "Enter 6: to deactivate alarm system" << endl;
	} else {
		for (int i = 1; i < argc; ++i) {
			cout << "You have entered " << &(*argv[i]) << " arguments:" << "\n";
			char* entry = argv[i];
			int x = atoi(entry);
			cout << x << endl;

			switch (x) {
			case 1:
				printf(
						"This following function sets current NTP time into DS3231 and displays the same \n");
				DS3231.setTime();
				DS3231.displayTime();
				break;
			case 2:
				printf(
						"Here the function provides temperature reading from DS3231 \n");
				DS3231.tempReadings();
				break;
			case 3:
				printf(
						"This functionality generates a square waveform of below frequencies \n");
				cout << "Enter 1 for 1.0.KHz frequency waveform" << endl;
				cout << "Enter 2 for 1.024 KHz frequency waveform" << endl;
				cout << "Enter 3 for 4.096 KHz frequency waveform" << endl;
				cout << "Enter 4 for 8.192 KHz frequency waveform" << endl;

				int choice;
				cin >> choice;
				if (choice == 1)
					DS3231.BBSQWEnabler(1.0);
				if (choice == 2)
					DS3231.BBSQWEnabler(1.024);
				if (choice == 3)
					DS3231.BBSQWEnabler(4.096);
				if (choice == 4)
					DS3231.BBSQWEnabler(8.192);

				cout << "output can be checked in oscilloscope " << endl;
				break;
			case 4:
				printf(
						"This activates alarm. The alarm system sends a SQW interrupt every second \n");
				DS3231.alarm_1();
				cout << "Before setting alarm 2, deactivate this alarm."
						<< endl;
				break;
			case 5:
				printf(
						"This activates alarm. The alarm system sends a SQW interrupt depending on the minutes and seconds entry \n");
				int seconds, minutes;
				cout << "Enter Seconds entry 0 -59" << endl;
				cin >> seconds;
				cout << "Enter minutes entry 0-59" << endl;
				cin >> minutes;
				DS3231.alarm_2(seconds, minutes);
				cout << "Before setting alarm 1, deactivate this alarm."
						<< endl;
				break;
			case 6:
				printf("This operation deactivates all alarms \n");
				DS3231.deactivateAlarm();
				break;
			default:
				printf("Choice other than 1, 2, 3, 4, 5 and 6 \n");
				break;
			}
		}
	}

	return 0;
}

