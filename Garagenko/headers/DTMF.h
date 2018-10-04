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

class TDTMF {
	public:
		TDTMF();
		uint8_t getDTMFcode();
};

TDTMF::TDTMF() {
	DDRD &= 0x83;
	PORTD &= 0x83;
}

uint8_t TDTMF::getDTMFcode() {
	uint8_t tmp = PIND;
	return (tmp & 0x08) | ((tmp >> 2) & 0x04) | ((tmp >> 4) & 0x02) | ((tmp >> 6) & 0x01);
}
