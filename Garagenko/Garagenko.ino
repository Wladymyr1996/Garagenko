//////////////////////////////////////////////////////////////////////////
//						Project / Проект: Garagenko						//
//																	  	//
//		  Author / Автор: Stanuk Volodymyr / Стадник Володимир		  	//
//		  	Date / Дата: 3 october 2018 / 3 жовтня 2018				  	//
//																	  	//
////////////////////////////////////////////////////////////////////////// 

/*	This file is part of Garagenko.
 * 
 *
 *	Garagenko is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation.
 *
 *	Garagenko is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 *	(Цей файл — частина Garagenko.
 *
 *	Garagenko - вільна програма: ви можете перерозповсюджувати її і/або
 *	міняи її на умовах Загальної громадської ліцензії GNU у тому вигляді,
 *	в якому вона була опублікована Фондом вільного програмного забезпечення;
 *
 *	Garagenko розповюджується в нажії, що вона буде корисною, але
 *	БЕЗ ВСІЛЯКИХ ГАРАНТІЙ; навіть без неявної гарантії ТОВАРНОГО ВИДУ
 *	або ПРИДАТНОСТІ ДЛЯ ВИЗНАЧЕНИХ ЦІЛЕЙ. Детальніше див. в Загальній
 *	громадській лицензії GNU.

 *	Ви повинні були отримати копію Загальної суспільної ліцензії GNU
 *	разом із цією програмою. Якщо це не так, див.
 *	<https://www.gnu.org/licenses/>.)
*/

//Мова встановлюється за її кодом. Наразі доступні такі мови:
//Language can installed by code. Available languages:
//Language/Мова		|	Code/Код
//Українська		|		0
//English			|		1
//Русский			|		2

#define LANG 0

#define OpenCodeDTMF 1
#define CloseCodeDTMF 2
#define GetStatusDTMF 10

#include <TimerOne.h>
#include "headers/limit_switches.h"
#include "headers/phone.h"
#include "headers/DTMF.h"
#include "headers/door.h"
#include "headers/buttons.h"
#include "headers/audio.h"

TLimitSwitches LimitSwitches;
TPhone Phone;
TDTMF DTMF;
TDoor Door;
TButton Button;
TAudio Audio;

bool CallPhone = false;
bool WaitCallEnd = false;

void sayStatus() {
	if (Door.getStatus() == TDoor::isOpening) {
		Audio.play(TAudio::soundOpening);
		Audio.play(TAudio::soundProcess);
		return;
	}
	if (Door.getStatus() == TDoor::isClosing) {
		Audio.play(TAudio::soundClosing);
		Audio.play(TAudio::soundProcess);
		return;
	}
	if (LimitSwitches.getStatus() == TLimitSwitches::Opened) {
		Audio.play(TAudio::soundOpened);
		return;
	}
	if (LimitSwitches.getStatus() == TLimitSwitches::Closed) {
		Audio.play(TAudio::soundClosed);
		return;
	}
	Audio.play(TAudio::soundStopped);
}

void Open() {
	if ((LimitSwitches.getStatus() != TLimitSwitches::Opened) || 
		(Door.getStatus() == TDoor::isClosing)) Door.doOpen();
	if (CallPhone) {
		if (Door.getStatus() == TDoor::isStoped) sayStatus();
		if (Door.getStatus() == TDoor::isOpening) {
			Audio.play(TAudio::soundOpening);
			Audio.play(TAudio::soundProcess);
		}
	}
}

void Close() {
	if ((LimitSwitches.getStatus() != TLimitSwitches::Closed) || 
		(Door.getStatus() == TDoor::isOpening)) Door.doClose();
	if (CallPhone) {
		if (Door.getStatus() == TDoor::isStoped) sayStatus();
		if (Door.getStatus() == TDoor::isClosing) {
			Audio.play(TAudio::soundClosing);
			Audio.play(TAudio::soundProcess);
		}
	}
}

void Stop() {
	Door.doStop();
	if (CallPhone) {
		sayStatus();
		WaitCallEnd = true;
	}
}

void NewDTMF() {
	uint8_t code = DTMF.getDTMFcode();
	switch (code) {
		case OpenCodeDTMF: CallPhone = true; Open(); break;
		case CloseCodeDTMF: CallPhone = true; Close(); break;
		case GetStatusDTMF: sayStatus(); break;
	}
}

void AudioTimer() {
	Audio.doWork();
}

void setup() {
	Timer1.initialize(220);
	Timer1.attachInterrupt(AudioTimer);
	attachInterrupt(digitalPinToInterrupt(2), NewDTMF, FALLING);
	Serial.begin(9600);
}

void loop() {
	LimitSwitches.doWork();
	Phone.doWork();
	Door.doWork();
	Button.doWork();
	
	if (LimitSwitches.isNewStatus()) {
		if ((LimitSwitches.getStatus() == TLimitSwitches::Opened) && (Door.getStatus() == TDoor::isOpening)) Stop();
		if ((LimitSwitches.getStatus() == TLimitSwitches::Closed) && (Door.getStatus() == TDoor::isClosing)) Stop();
	}
	if (Button.isPressed()) {
		TButton::TPressed tmp = Button.getPressedButton();
		switch (tmp) {
			case TButton::Open: Open(); break;
			case TButton::Close: Close(); break;
		}
	}
	
	if ((WaitCallEnd) && (!Audio.isBusy())) {
		CallPhone = false;
		WaitCallEnd = false;
		Phone.endCall();
	}
	delay(10);
}
