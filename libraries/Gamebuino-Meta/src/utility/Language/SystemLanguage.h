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

#define NUMBER_SYSTEM_LANGUAGES 3

//////////
// Home Menu
//////////

const MultiLang lang_homeMenu_exit[] = {
	{LangCode::en, "EXIT GAME "},
	{LangCode::de, "VERLASSEN "},
	{LangCode::fr, "QUITTER   "},
};

const MultiLang lang_homeMenu_volume[] = {
	{LangCode::en, "VOLUME    "},
	{LangCode::de, "LAUTSTÄRKE"},
	{LangCode::fr, "VOLUME    "},
};

const MultiLang lang_homeMenu_save_image[] = {
	{LangCode::en, "SAVE IMAGE"},
	{LangCode::de, "BILD SPCHR"},
	{LangCode::fr, "ENRG IMAGE"},
};

const MultiLang lang_homeMenu_save_video[] = {
	{LangCode::en, "SAVE VIDEO"},
	{LangCode::de, "VID SPCHRN"},
	{LangCode::fr, "ENRG VIDEO"},
};

const MultiLang lang_homeMenu_light[] = {
	{LangCode::en, "LIGHT     "},
	{LangCode::de, "LICHT     "},
	{LangCode::fr, "LUMINOSITÉ"},
};

const MultiLang lang_homeMenu_SELECT[] = {
	{LangCode::en, "SELECT  "},
	{LangCode::de, "WÄHLEN  "},
	{LangCode::fr, "CHOISIR "},
};

const MultiLang lang_homeMenu_RESUME[] = {
	{LangCode::en, "RESUME  "},
	{LangCode::de, "ZURÜCK  "},
	{LangCode::fr, "RETOUR  "},
};

const MultiLang lang_homeMenu_SAVING[] = {
	{LangCode::en, "SAVING... "},
	{LangCode::de, "SICHERE..."},
	{LangCode::fr, "ENREGISTRE"},
};

const MultiLang lang_homeMenu_SAVED[] = {
	{LangCode::en, "SAVED!    "},
	{LangCode::de, "GESICHERT!"},
	{LangCode::fr, "ENREGISTRÉ"},
};

const MultiLang lang_homeMenu_ERROR[] = {
	{LangCode::en, "ERROR     "},
	{LangCode::de, "FEHLER    "},
	{LangCode::fr, "ERREUR    "},
};

const MultiLang lang_homeMenu_READY[] = {
	{LangCode::en, "READY?    "},
	{LangCode::de, "BEREIT?   "},
	{LangCode::fr, "PRÊT?     "},
};

const MultiLang lang_homeMenu_GO[] = {
	{LangCode::en, "GO!       "},
	{LangCode::de, "LOS!      "},
	{LangCode::fr, "PARTEZ!   "},
};


//////////
// Keyboard
//////////

const MultiLang lang_keyboard_type[] = {
	{LangCode::en, "type"},
	{LangCode::de, "tippen"},
	{LangCode::fr, "écrire"},
};

const MultiLang lang_keyboard_back[] = {
	{LangCode::en, "back"},
	{LangCode::de, "zurück"},
	{LangCode::fr, "retour"},
};

const MultiLang lang_keyboard_save[] = {
	{LangCode::en, "save"},
	{LangCode::de, "fertig"},
	{LangCode::fr, "enreg."},
};

}; // namespace Gamebuino_Meta
