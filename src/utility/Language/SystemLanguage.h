// This file is ISO-8859-1 encoded, or anything compatible, such as Windows-1252

/*
This file is part of the Gamebuino-Meta library,
Copyright (c) Aadalie 2017

This is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License (LGPL)
as published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

This is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License (LGPL) for more details.

You should have received a copy of the GNU Lesser General Public
License (LGPL) along with the library.
If not, see <http://www.gnu.org/licenses/>.

Authors:
 - Aurelien Rodot
 - Sorunome
 - Drakker
*/

namespace Gamebuino_Meta {

#define NUMBER_SYSTEM_LANGUAGES 4

//////////
// Keyboard
//////////

const MultiLang lang_keyboard_type[] = {
	{LangCode::en, "type"},
	{LangCode::de, "tippen"},
	{LangCode::fr, "écrire"},
	{LangCode::es, "tipo"},
};

const MultiLang lang_keyboard_back[] = {
	{LangCode::en, "back"},
	{LangCode::de, "zurück"},
	{LangCode::fr, "retour"},
	{LangCode::es, "espald"},
};

const MultiLang lang_keyboard_save[] = {
	{LangCode::en, "save"},
	{LangCode::de, "fertig"},
	{LangCode::fr, "enreg."},
	{LangCode::es, "guardar"},
};

}; // namespace Gamebuino_Meta
