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

class TButton {
	public:
		enum TPressed {Open, Close, Nothing};
		
		TButton();
		void doWork();
		bool isPressed();
		TPressed getPressedButton();
		
	private:
		bool Pressed;
		TPressed pressedButton;
};

TButton::TButton() {
	DDRC &= ~((1 << PC2) | (1 << PC3));
	PORTC |= (1 << PC2) | (1 << PC3);
	
	this->Pressed = false;
	this->pressedButton = TButton::Nothing;
}

void TButton::doWork() {
	static TButton::TPressed prsd = TButton::Nothing;
	static uint8_t counter = 0;
	
	if (counter == 0) {
		if (!(PINC & (1 << PC2))) {
			prsd = TButton::Open;
			counter = 1;
		}
		if (!(PINC & (1 << PC3))) {
			prsd = TButton::Close;
			counter = 1;
		}
	} else if (counter == 5) {	
		if ((!(PINC & (1 << PC2))) && (prsd == TButton::Open)) {
			counter = 1;
		}
		if ((!(PINC & (1 << PC3))) && (prsd == TButton::Close)) {
			counter = 1;
		}
	} else if (counter == 10) {
		this->pressedButton = prsd;
		this->Pressed = true;
		prsd = TButton::Nothing;
		counter = 0;
	}
	if (counter != 0) counter++;
}

bool TButton::isPressed() {
	return Pressed;
}

TButton::TPressed TButton::getPressedButton() {
	TButton::TPressed tmp = this->pressedButton;
	this->pressedButton = TButton::Nothing;
	this->Pressed = false;
	return tmp;
}
