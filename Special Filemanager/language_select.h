#include "menu_coloured.h"

class LanguageSelect
{

	HANDLE cHandle;
	std::shared_ptr<Menu> menu;

public:


	// // // Базированность

	const std::string& SetColor(WORD color);
	void MenuAction();

	// // // Прочее

	void SetLanguage(const std::string& language);

	void SetRussian();

	void SetEnglish();

	void SelectionMenu();

	// Запуск программы
	bool Run();
};