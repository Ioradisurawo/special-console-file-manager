#include "localization.h"
#include "language_select.h"
#include "menu_controller.h"

// // // Базированность

const std::string& LanguageSelect::SetColor(WORD color)
{
	SetConsoleTextAttribute(cHandle, color);
	return "";
}
void LanguageSelect::MenuAction()
{
	ControlCode c_code = MenuController::ParseControlAction();
	switch (c_code)
	{
	case ControlCode::CONFIRM:
	{
		menu->InvokeItem();
		break;
	}
	case ControlCode::UP:
	{
		if (menu->IsVertical())
			menu->PreviousItem();
		break;
	}
	case ControlCode::DOWN:
	{
		if (menu->IsVertical())
			menu->NextItem();
		break;
	}
	default:
	{
		break;
	}
	}
}


// // // Прочее


void LanguageSelect::SetLanguage(const std::string& language)
{
	Localization::LANGUAGE = language;

	Localization::UpdateLocale();

	// Выключить меню
	menu = nullptr;
}

void LanguageSelect::SetRussian()
{
	SetLanguage("RUSSIAN");
}
void LanguageSelect::SetEnglish()
{
	SetLanguage("ENGLISH");
}

void LanguageSelect::SelectionMenu()
{
	// Настройка меню
	menu = std::make_shared<Menu>
		(
			cHandle, fgWHITE,
			"[-- Language select  --]\n",
			"|_                _]\n",
			"[ ", "  ]",
			"| ", "  ",
			"\n"
		);

	menu->AddItem(std::make_shared<MenuItem>("Russian", fgWHITE, std::bind(&LanguageSelect::SetRussian, this)));

	menu->AddItem(std::make_shared<MenuItem>("English", fgWHITE, std::bind(&LanguageSelect::SetEnglish, this)));
}

// Запуск программы
bool LanguageSelect::Run()
{

	//// Спрятать курсор
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);

	// Изначальное меню
	SelectionMenu();

	// Цикл
	while (menu)
	{
		std::cout << "\x1B[2J\x1B[H";

		menu->CoutMenu();

		MenuAction();
	}

	return true;
}
