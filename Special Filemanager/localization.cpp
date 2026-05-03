#include "localization.h"

void Localization::UpdateLocale()
{
	if (LANGUAGE == "RUSSIAN")
	{
		system("chcp 65001");

		lcmap["folder"] = "папка";
		lcmap["file"] = "файл";
		lcmap["size"] = "размер";
		lcmap["new_dir_name"] = "Название новой директории";
		lcmap["back"] = "Назад";

		lcmap["control_tips"] = "Управление:  Перемещение - wasd/стрелки  ;  Выбор - enter/e  ;  Назад - esc ; Скрыть подсказки - q/й";
		lcmap["show_tips"] = "Показать подсказки - q/й";
		lcmap["buffer"] = "Буфер";
		lcmap["empty"] = "Пусто";
		lcmap["last_entry"] = "Последняя запись";
		lcmap["exit"] = "Выход";


	}
	if (LANGUAGE == "ENGLISH")
	{
		lcmap["folder"] = "folder";
		lcmap["file"] = "file";
		lcmap["size"] = "size";
		lcmap["new_dir_name"] = "New directory name";
		lcmap["back"] = "Back";

		lcmap["control_tips"] = "Controls:  Movement - wasd/arrowkeys  ;  Select - enter/e  ;  Back - esc ; Hide tips - q";
		lcmap["show_tips"] = "Show tips - q";
		lcmap["buffer"] = "Buffer";
		lcmap["empty"] = "Empty";
		lcmap["last_entry"] = "Last entry";
		lcmap["exit"] = "Exit";
	}
}