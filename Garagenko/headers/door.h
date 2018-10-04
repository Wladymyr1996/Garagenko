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

//Двері. Клас керування двигуном
class TDoor {
	public:
		enum TDoorStatus {isOpening, isClosing, isStoped};
		enum TPowerSupply {isOn, isOff};
		
		TDoor();
		void doOpen(); //Відкрити
		void doClose(); //Закрити
		void doStop(); //Зупинити
		void doWork(); //Опрацювання
		TDoorStatus getStatus(); //Повенрнути статус
	private: 
		TDoorStatus Status; //Статус дверей
		TPowerSupply PowerStatus; //Статус живлення
		int16_t counter; //Лічильник
		void PowerOn(); //Увімкнути живлення
		void PowerOff(); //Вимкнути живлення
};

TDoor::TDoor() {
	PORTD &= ~(1 << PD7);
	PORTB &= ~(1 << PB0);
	PORTC &= ~(1 << PC5);
	DDRD |= 1 << PD7;
	DDRB |= 1 << PB0;
	DDRC |= 1 << PC5;
	
	counter = 0;
	this->Status = TDoor::isStoped;
	this->PowerStatus = TDoor::isOff;
}

void TDoor::doOpen() {
	Serial.write("Відкриваю");
	if (this->PowerStatus == isOff) {
		this->Status = TDoor::isOpening;
		this->PowerOn();
		this->counter = 100;
	} else {
		switch (this->Status) {
			case TDoor::isClosing:
				this->doStop();
				break;
			case TDoor::isStoped:
				this->Status = TDoor::isOpening;
				counter = 1;
				break;
		}
	}
}

void TDoor::doClose() {
	if (this->PowerStatus == isOff) {
		this->Status = TDoor::isClosing;
		this->PowerOn();
		this->counter = 100;
	} else {
		switch (this->Status) {
			case TDoor::isOpening:
				this->doStop();
				break;
			case TDoor::isStoped:
				this->Status = TDoor::isClosing;
				counter = 1;
				break;
		}
	}
	Serial.write("Закриваю");
}

void TDoor::doWork() {
	if (this->counter == 0) return;
	counter--;
	if (counter == 0) {
		switch (this->Status) {
			case TDoor::isOpening: 
				PORTB &= ~(1 << PB0);
				PORTD |= (1 << PD7);
				break;
			case TDoor::isClosing:
				PORTD &= ~(1 << PD7);
				PORTB |= (1 << PB0);
				break;
			case TDoor::isStoped:
				PowerOff();
		}
	}
}

void TDoor::doStop() {
	PORTD &= ~(1 << PD7);
	PORTB &= ~(1 << PB0);
	this->Status = TDoor::isStoped;
	counter = 1000;
}

void TDoor::PowerOn() {
	PORTC |= 1 << PC5;
	this->PowerStatus = TDoor::isOn;
}

void TDoor::PowerOff() {
	PORTC &= ~(1 << PC5);
	this->PowerStatus = TDoor::isOff;
}

TDoor::TDoorStatus TDoor::getStatus() {
	return this->Status;
}
