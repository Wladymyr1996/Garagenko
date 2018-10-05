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

#define ST PB3
#define SH PB2
#define DS PB4

#ifndef LANG
	#error [ERROR] LANG is not defined | LANG не налаштовано
#endif

#if LANG == 0
	#include "audio_defines_uk.h"
#elif LANG == 1
	#include "audio_defines_en.h"
#elif LANG == 2
	#include "audio_defines_ru.h"
#elif LANG == 9999
	#include "audio_defines_custom.h"
#else
	#error [ERROR] LANG may be defined to 0-2 | LANG повинен мати значення 0-2
#endif
	
const uint8_t audDataSine[10] PROGMEM = {128, 203, 249, 249, 203, 128, 53, 7, 7, 53};

class TAudio {
	public:
		enum TTypeSound {soundOpened, soundOpening, soundClosed, soundClosing, soundStopped, soundProcess, soundSine};
		TAudio();
		void play(TTypeSound type);
		void doWork();
		bool isBusy();
		
	private:
		struct TSound {
			TTypeSound type;
			TSound *nextSound = NULL;
			uint16_t counter;
			uint16_t max;
		};
		
		TSound *SoundListBegin;
		TSound *SoundListEnd;
		bool busy;
		void sendByte(uint8_t byte);
		void popSoundList();
};

TAudio::TAudio() {
	DDRB |= (1 << ST) | (1 << SH) | (1 << DS);
	PORTB &= ~((1 << ST) | (1 << SH) | (1 << DS));
	
	SoundListBegin = NULL;
	SoundListEnd = NULL;
	busy = false;
}

void TAudio::play(TAudio::TTypeSound type) {
	if ((type != TAudio::soundSine) && (type != TAudio::soundProcess)) this->play(TAudio::soundSine);
	uint16_t bytes;
	switch (type) {
		case TAudio::soundOpened: bytes = OPENED_BYTES; break;
		case TAudio::soundOpening: bytes = OPENING_BYTES; break;
		case TAudio::soundClosed: bytes = CLOSED_BYTES; break;
		case TAudio::soundClosing: bytes = CLOSING_BYTES; break;
		case TAudio::soundStopped: bytes = STOPPED_BYTES; break;
		case TAudio::soundProcess: bytes = PROCESS_BYTES; break;
		case TAudio::soundSine: bytes = 2000; break;
	}
	if (SoundListBegin == NULL) {
		SoundListBegin = new TSound;
		SoundListBegin->nextSound = NULL;
		SoundListBegin->counter = 0;
		SoundListBegin->max = bytes;
		SoundListBegin->type = type;
		SoundListEnd = SoundListBegin;
	} else {
		SoundListEnd->nextSound = new TSound;
		SoundListEnd = SoundListEnd->nextSound;
		SoundListEnd->nextSound = NULL;
		SoundListEnd->counter = 0;
		SoundListEnd->max = bytes;
		SoundListEnd->type = type;
	}
	busy = true;
}

void TAudio::doWork() {
	if (SoundListBegin == NULL) {
		sendByte(128);
		busy = false;
		return;
	} else {
		busy = true;
		if (SoundListBegin->type == TAudio::soundSine) {
			if ((SoundListBegin->counter > 700) && 
				(SoundListBegin->counter < 1000) || (
				(SoundListBegin->counter > 1200) && 
				(SoundListBegin->counter < 1500))) {
				sendByte(pgm_read_byte(&(audDataSine[SoundListBegin->counter % 10])));
			} else sendByte(128);
			SoundListBegin->counter++;
			if (SoundListBegin->counter == SoundListBegin->max) popSoundList();
		} else if (SoundListBegin->type == TAudio::soundProcess) {
			sendByte(pgm_read_byte(&(audDataprocess[SoundListBegin->counter]))); 
			SoundListBegin->counter++;
			if (SoundListBegin->nextSound != NULL) popSoundList();
			if (SoundListBegin->counter == SoundListBegin->max) SoundListBegin->counter = 0;
		} else {
			switch (SoundListBegin->type) {
				case TAudio::soundOpened: sendByte(pgm_read_byte(&(audDataopened[SoundListBegin->counter]))); break;
				case TAudio::soundOpening: sendByte(pgm_read_byte(&(audDataopening[SoundListBegin->counter]))); break;
				case TAudio::soundClosed: sendByte(pgm_read_byte(&(audDataclosed[SoundListBegin->counter]))); break;
				case TAudio::soundClosing: sendByte(pgm_read_byte(&(audDataclosing[SoundListBegin->counter]))); break;
				case TAudio::soundStopped: sendByte(pgm_read_byte(&(audDatastopped[SoundListBegin->counter]))); break;
			}
			SoundListBegin->counter++;
			if (SoundListBegin->counter == SoundListBegin->max) popSoundList();
		}
	}
}

void TAudio::popSoundList() {
	if (SoundListBegin == NULL) return;
	TSound *tmp = SoundListBegin;
	SoundListBegin = SoundListBegin->nextSound;
	delete tmp;
	if (SoundListBegin == NULL) SoundListEnd = NULL;
}

void TAudio::sendByte(uint8_t byte) {
	for (uint8_t i = 0; i < 8; i++) {
		if (byte & 0x01) PORTB |= 1 << DS;
			else PORTB &= ~(1 << DS);
		byte >>= 1;
		PORTB |= (1 << SH);
		PORTB &= ~(1 << SH);
	}
	PORTB |= (1 << ST);
	PORTB &= ~(1 << ST);
}

bool TAudio::isBusy() {
	return busy;
}
