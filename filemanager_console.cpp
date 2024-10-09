#include "filemanager_console.h"


std::vector<ControlCode> k_code = { ControlCode::UP, ControlCode::UP, ControlCode::DOWN, ControlCode::DOWN, ControlCode::LEFT, ControlCode::RIGHT, ControlCode::LEFT, ControlCode::RIGHT, ControlCode::ACTION, ControlCode::ACTION };

void ConsoleFilemanager::SpecialCodeCheck(const ControlCode& code)
{
	special_code.push_back(code);
	for (int i = 0; i < special_code.size(); i++)
	{
		if (k_code.at(i) != special_code.at(i))
			special_code.clear();
	}
	if (special_code.size() == k_code.size())
	{
		audio_player.PlayUserSFX(Audio::SND_INIT, 0);
		Sleep(3000);
		special_mode = true;
	}
}
void ConsoleFilemanager::MenuAction()
{
	ControlCode c_code = MenuController::ParseControlAction();
	switch (c_code)
	{
	case ControlCode::CONFIRM:
	{
		current_menu->InvokeItem();
		break;
	}
	case ControlCode::DECLINE:
	{
		if (previous_menu)
			previous_menu();
		break;
	}
	case ControlCode::UP:
	{
		if (current_menu->IsVertical())
			current_menu->PreviousItem();
		break;
	}
	case ControlCode::DOWN:
	{
		if (current_menu->IsVertical())
			current_menu->NextItem();
		break;
	}
	case ControlCode::LEFT:
	{
		if (!current_menu->IsVertical())
			current_menu->PreviousItem();
		PageLeft();

		break;
	}
	case ControlCode::RIGHT:
	{
		if (!current_menu->IsVertical())
			current_menu->NextItem();
		PageRight();

		break;
	}
	case ControlCode::ACTION:
	{
		if (action_allowed)
			EntryMenu();
		break;
	}
	case ControlCode::ADDITIONAL_1:
	{
		tooltips_enabled = !tooltips_enabled;
		break;
	}
	default:
	{
		break;
	}
	}
	if (!special_mode)
		SpecialCodeCheck(c_code);
}
void ConsoleFilemanager::Shutdown()
{
	current_menu = nullptr;
}



// Заполнение директорий
void ConsoleFilemanager::ReloadDriveDirs()
{
	directories.clear();
	// Заполнение дисков
	for (char drive = 'a'; drive <= 'z'; ++drive)
	{
		try
		{
			std::string path = std::string(1, drive) + ":/";
			if (std::filesystem::exists(path))
				directories.push_back(path);
		}
		catch (std::exception)
		{
		}
	}
}
void ConsoleFilemanager::ReloadDirs()
{
	directories.clear();
	for (auto& entry : std::filesystem::directory_iterator(selected_path
		// Важная штука
		, std::filesystem::directory_options::skip_permission_denied))
	{
		directories.push_back(entry.path().string());
	}
}



// Действия (общее)
void ConsoleFilemanager::CalculateSize()
{
	bool is_directory = std::filesystem::is_directory(selected_path);
	std::string size = BytesToString(is_directory ? DirectorySize(selected_path) : std::filesystem::file_size(selected_path), 2);
	current_menu->top_string.insert(current_menu->top_string.size() - 1, "[ Размер " + size + " ]");
	current_menu->GetItem()->SetText("Назад");
	current_menu->GetItem()->SetFunc(std::bind(&ConsoleFilemanager::PreviousDir, this));
}

// Действия (директория)
void ConsoleFilemanager::DirCreate()
{
	std::cout << "\nНазвание новой директории\n>";
	std::string dir_name;
	std::getline(std::cin >> std::ws, dir_name);

	// Поиск папки с таким же названием
	bool found_identical = false;
	for (auto& entry : selected_path)
	{
		if (entry.filename() == dir_name)
		{
			found_identical = true;
			break;
		}
	}


	if (found_identical)
	{
		std::cout << "> " << selected_path + "/" + dir_name << " уже существует";
	}
	else
	{
		try
		{
			std::filesystem::create_directory(selected_path + "/" + dir_name);
			std::cout << "> успешно";
		}
		catch (std::exception)
		{
			std::cout << "> ошибка";
		}
	}

	std::cout << std::endl; system("pause");

	DirMenu();
}
void ConsoleFilemanager::FileCreate()
{
	std::cout << "\nНазвание нового файла\n>";
	std::string file_name;
	std::getline(std::cin >> std::ws, file_name);

	// Поиск папки с таким же названием
	bool found_identical = false;
	for (auto& entry : selected_path)
	{
		if (entry.filename() == file_name)
		{
			found_identical = true;
			break;
		}
	}

	if (found_identical)
	{
		std::cout << "> " << selected_path + "/" + file_name << " уже существует";
	}
	else
	{
		try
		{
			std::ofstream file(selected_path + "/" + file_name);
			std::cout << "> успешно";
		}
		catch (std::exception)
		{
			std::cout << "> ошибка";
		}
	}

	std::cout << std::endl; system("pause");

	DirMenu();
}
void ConsoleFilemanager::EntryCreateMenu()
{
	action_allowed = false;
	//page_allowed = false;
	current_paged_menu = nullptr;

	// Действие при decline
	previous_menu = std::bind(&ConsoleFilemanager::EntryMenu, this);

	// Указатель на предыдущее меню
	std::shared_ptr<Menu> saved_menu = current_menu;


	// Настройка меню
	current_menu = std::make_shared<Menu>
		(
			cHandle, fgWHITE,
			std::string(*saved_menu) + "\n",
			"\n[]",
			"[ ", "  ]",
			"  ", "  ",
			"   "
		);
	current_menu->SetVertical(false);

	current_menu->AddItem(std::make_shared<MenuItem>("Создать папку", fgWHITE, std::bind(&ConsoleFilemanager::DirCreate, this)));
	current_menu->AddItem(std::make_shared<MenuItem>("Создать файл", fgWHITE, std::bind(&ConsoleFilemanager::FileCreate, this)));
}
void ConsoleFilemanager::EntryPaste()
{
	bool saved_is_directory = std::filesystem::is_directory(saved_path);
	std::filesystem::path file_path = selected_path + "/" + saved_path.filename().string();

	if (move_mode)
	{
		std::filesystem::rename(saved_path, selected_path + "/" + saved_path.filename().string());

		std::cout << "> перемещение успешно";
	}


	if (saved_is_directory)
	{
		try
		{
			std::filesystem::copy(saved_path, file_path,
				std::filesystem::copy_options::update_existing
				//|fs::copy_options::overwrite_existing
				| std::filesystem::copy_options::recursive);
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	else
	{
		try
		{
			std::filesystem::copy_file(saved_path, file_path);
			std::cout << "> копирование успешно";
		}
		catch (std::exception)
		{
			std::cout << "> ошибка";
		}
		/*
		std::ifstream check_file;
		check_file.open(file_path);
		if (check_file.is_open())
		{
			std::cout << "> файл уже существует";
		}
		else
		{
			// sel_path to file_path ?
			std::filesystem::copy_file(saved_path, selected_path);

			std::cout << "> копирование успешно";
			saved_path.clear();
		}
		*/
	}

	std::cout << std::endl; system("pause");

	DirMenu();
}

void ConsoleFilemanager::SearchResultMenu()
{
	// для page_left и page_right
	current_paged_menu = std::bind(&ConsoleFilemanager::SearchResultMenu, this);
	//
	action_allowed = false;
	//page_allowed = true;

	// текущий индекс (начало страницы)
	int start_index = current_page * entries_per_page;

	// подсчёт кол-ва страниц
	double entry_pages = directories.size() / double(entries_per_page);
	bool leftover = (entry_pages * 10) - round(entry_pages) > 0 ? true : false;
	int pages_amount = entry_pages;

	// предел
	int lim;
	if (current_page + 1 < pages_amount + leftover)
		lim = (current_page + 1) * entries_per_page;
	else
		lim = directories.size();

	// Пред.
	previous_menu = std::bind(&ConsoleFilemanager::PreviousDir, this);

	// Настройка меню
	std::ostringstream lower;
	lower << "|_ cтраница " << std::to_string(current_page) << " из " << pages_amount << " (" << start_index << "-" << directories.size() << ")_|\n";

	current_menu = std::make_shared<Menu>
		(
			cHandle, fgWHITE,
			"[-- Поиск в " + selected_path.string() + " --]\n",
			lower.str(),
			"->> ", "  <>-",
			"| ", "  ",
			"\n"
		);

	// Установка индекса
	current_menu->SetItemIndex(0);

	// Заполнение пунктами меню
	std::filesystem::path entry;
	for (int i = start_index; i < lim; i++)
	{
		entry = directories.at(i);
		current_menu->AddItem(std::make_shared<MenuItem>((std::filesystem::is_directory(entry) ? "[папка]\t" : "[файл]\t") + entry.filename().string(), fgWHITE, std::bind(&ConsoleFilemanager::SelectEntry, this)));
	}
}
void ConsoleFilemanager::RecursiveQuerySearch(const std::filesystem::path path, const std::string query)
{
	for (auto& entry : std::filesystem::directory_iterator(path))
	{
		if (FindWordInLine(StringToLower(entry.path().filename().replace_extension("").string()), StringToLower(query)) != std::string::npos)
		{
			directories.push_back(entry);
		}
		if (entry.is_directory())
			RecursiveQuerySearch(entry.path(), query);
	}
}
void ConsoleFilemanager::FolderSearch()
{
	std::cout << "\nВведите запрос\n>";
	std::string query;
	std::getline(std::cin >> std::ws, query);

	// Заполнение результатами поиска 
	directories.clear();
	RecursiveQuerySearch(selected_path, query);
	SearchResultMenu();
}

// Действие (файл)
void ConsoleFilemanager::EntryRename()
{
	bool is_directory = std::filesystem::is_directory(selected_path);
	std::cout << "\nНовое название\n>";
	std::string file_name;
	std::getline(std::cin >> std::ws, file_name);

	std::filesystem::path new_p;
	try
	{
		std::filesystem::path old_p = selected_path;

		if (is_directory)
			new_p = selected_path.replace_filename(file_name);
		else
			new_p = selected_path.replace_extension("").replace_filename(file_name);

		std::filesystem::rename(old_p, new_p);
		std::cout << "> успешно";
	}
	catch (std::exception)
	{
		std::cout << "> ошибка";
	}

	std::cout << std::endl; system("pause");

	if (is_directory)
	{
		directory_history.pop();
		directory_history.push(DirectoryData(new_p, 0, 0));
		DirMenu();
	}
	else
		PreviousDir();
}
void ConsoleFilemanager::EntryCopy()
{
	move_mode = false;

	saved_path = selected_path;

	std::cout << ">скопировано в буфер";

	std::cout << std::endl; system("pause");
	PreviousDir();
}
void ConsoleFilemanager::EntryMove()
{
	move_mode = true;

	saved_path = selected_path;

	std::cout << ">файл скопирован для перемещения в буфер";


	std::cout << std::endl; system("pause");
	PreviousDir();
}
void ConsoleFilemanager::EntryDelete()
{
	// удаление файла
	if (std::filesystem::is_directory(selected_path))
		std::filesystem::remove_all(selected_path);
	else
		std::filesystem::remove(selected_path);

	std::cout << ">удаление прошло успешно";

	std::cout << std::endl; system("pause");
	PreviousDir();
}



// Перемещение по директориям
void ConsoleFilemanager::PreviousDir()
{
	// если можно вызвать меню действий (т.е. мы НЕ в меню действий)
	if (std::filesystem::is_directory(selected_path))
	{
		// Если вне меню действий
		if (action_allowed)
		{
			current_page = directory_history.top().page;
			current_index = directory_history.top().index;
			directory_history.pop();
			if (directory_history.empty())
			{
				DriveMenu();
				return;
			}
			DirectoryData dir_data = directory_history.top();
			selected_path = dir_data.path;
		}
		else
		{
			selected_path = directory_history.top().path;
		}
		//current_page = dir_data.page;
		//current_index = dir_data.index;
	}
	else
	{
		directory_history.pop();
		selected_path = directory_history.top().path;
	}

	DirMenu();
}
void ConsoleFilemanager::SelectEntry()
{
	try
	{
		current_index = current_menu->GetItemIndex() + (current_page * entries_per_page);

		if (std::filesystem::is_directory(directories.at(current_index)))
		{
			// меняем текущий путь на выбранный элемент
			selected_path = directories.at(current_index);

			// если в данном меню МОЖНО вызвать меню действий
			//if(action_allowed)
				// сохраняем текущий путь, индекс страницы и индекс элемента на этой странице
			directory_history.push(DirectoryData(selected_path, current_page, current_menu->GetItemIndex()));

			// устанавливаем индексы по-умолчанию
			current_page = 0;
			current_index = 0;

			DirMenu();
		}
		else
		{
			// сохраняем текущий путь, индекс страницы и индекс элемента на этой странице
			// directory_history.push(DirectoryData(selected_path, current_page, current_menu->GetItemIndex()));
			// меняем текущий путь на выбранный элемент
			selected_path = directories.at(current_index);
			// сохраняем текущий путь, индекс страницы и индекс элемента на этой странице
			directory_history.push(DirectoryData(selected_path, current_page, current_menu->GetItemIndex()));
			EntryMenu();
		}
	}
	catch (std::exception)
	{
		std::cout << "\n\nNot enough perms\n\n";
		system("pause");
	}

}
void ConsoleFilemanager::PageLeft()
{
	if (!current_paged_menu)
		return;

	current_index = 0;
	if (current_page - 1 >= 0)
		--current_page;
	else
		current_page = directories.size() / entries_per_page;

	//DirMenu();
	current_paged_menu();
}
void ConsoleFilemanager::PageRight()
{
	if (!current_paged_menu)
		return;

	current_index = 0;
	if (current_page + 1 <= directories.size() / entries_per_page)
		++current_page;
	else
		current_page = 0;

	//DirMenu();
	current_paged_menu();
}



// Меню 
void ConsoleFilemanager::EntryMenu()
{
	action_allowed = false;
	current_paged_menu = nullptr;

	bool is_directory = std::filesystem::is_directory(selected_path);

	// Действие при decline
	previous_menu = std::bind(&ConsoleFilemanager::PreviousDir, this);

	// Указатель на предыдущее меню
	std::shared_ptr<Menu> saved_menu = current_menu;

	// Настройка меню
	current_menu = std::make_shared<Menu>
		(
			cHandle, fgWHITE,
			std::string(*saved_menu) + "\n[ " + (is_directory ? "текущая папка" : "файл - " + selected_path.filename().string()) + " ]\n",
			"\n[]",
			"[ ", "  ]",
			"  ", "  ",
			"   "
		);
	current_menu->SetVertical(false);

	current_menu->AddItem(std::make_shared<MenuItem>("Посчитать размер", fgWHITE, std::bind(&ConsoleFilemanager::CalculateSize, this)));
	current_menu->AddItem(std::make_shared<MenuItem>("Создать", fgWHITE, std::bind(&ConsoleFilemanager::EntryCreateMenu, this)));
	if (is_directory)
	{
		if (!saved_path.empty())
			current_menu->AddItem(std::make_shared<MenuItem>("Вставить в директорию", fgWHITE, std::bind(&ConsoleFilemanager::EntryPaste, this)));
		current_menu->AddItem(std::make_shared<MenuItem>("Поиск", fgWHITE, std::bind(&ConsoleFilemanager::FolderSearch, this)));
	}
	current_menu->AddItem(std::make_shared<MenuItem>("Переименовать", fgWHITE, std::bind(&ConsoleFilemanager::EntryRename, this)));
	current_menu->AddItem(std::make_shared<MenuItem>("Копировать", fgWHITE, std::bind(&ConsoleFilemanager::EntryCopy, this)));
	current_menu->AddItem(std::make_shared<MenuItem>("Переместить", fgWHITE, std::bind(&ConsoleFilemanager::EntryMove, this)));
	current_menu->AddItem(std::make_shared<MenuItem>("Удалить", fgWHITE, std::bind(&ConsoleFilemanager::EntryDelete, this)));
}
void ConsoleFilemanager::DirMenu()
{
	// для вызова меню действий
	action_allowed = true;
	// для page_left и page_right
	current_paged_menu = std::bind(&ConsoleFilemanager::DirMenu, this);

	ReloadDirs();

	// текущий индекс (начало страницы)
	int start_index = current_page * entries_per_page;

	// подсчёт кол-ва страниц
	double entry_pages = directories.size() / double(entries_per_page);
	bool leftover = (entry_pages * 10) - round(entry_pages) > 0 ? true : false;
	int pages_amount = entry_pages;

	// предел
	int lim;
	if (current_page + 1 < pages_amount + leftover)
		lim = (current_page + 1) * entries_per_page;
	else
		lim = directories.size();

	// Если глубина больше одного вернуться в родительскую директорию иначе к выбору тома
	previous_menu = std::bind(directory_history.empty() ? &ConsoleFilemanager::DriveMenu : &ConsoleFilemanager::PreviousDir, this);
	/*
	if(directory_history.size() != 1)
		previous_menu = std::bind(&ConsoleFilemanager::PreviousDir, this);
	else
		previous_menu = std::bind(&ConsoleFilemanager::DriveMenu, this);
	*/

	// Настройка меню
	std::ostringstream lower;
	lower << "|_ cтраница " << std::to_string(current_page) << " из " << pages_amount << " (" << start_index << "-" << directories.size() << ")_|\n";

	current_menu = std::make_shared<Menu>
		(
			cHandle, fgWHITE,
			"[-- " + selected_path.string() + " --]\n",
			lower.str(),
			"->> ", "  <>-",
			"| ", "  ",
			"\n"
		);

	// Установка индекса
	current_menu->SetItemIndex(current_index);

	// Заполнение пунктами меню
	std::filesystem::path entry;
	for (int i = start_index; i < lim; i++)
	{
		entry = directories.at(i);
		current_menu->AddItem(std::make_shared<MenuItem>((std::filesystem::is_directory(entry) ? "[папка]\t" : "[файл]\t") + entry.filename().string(), fgWHITE, std::bind(&ConsoleFilemanager::SelectEntry, this)));
	}
}
void ConsoleFilemanager::DriveMenu()
{
	action_allowed = false;
	current_paged_menu = nullptr;

	if (!directory_history.empty())
		directory_history.pop();

	// Настройка меню
	previous_menu = std::bind(&ConsoleFilemanager::Shutdown, this);
	current_menu = std::make_shared<Menu>
		(
			cHandle, fgWHITE,
			"[-- Выбор Диска  --]\n",
			"|_                _]\n",
			"[ ", "  ]",
			"| ", "  ",
			"\n"
		);

	//
	ReloadDriveDirs();

	for (auto& entry : directories)
	{
		current_menu->AddItem(std::make_shared<MenuItem>(entry.string(), fgWHITE, std::bind(&ConsoleFilemanager::SelectEntry, this)));
	}


	current_menu->AddItem(std::make_shared<MenuItem>("Выход", fgWHITE, std::bind(&ConsoleFilemanager::Shutdown, this)));
}

// Конструктор
ConsoleFilemanager::ConsoleFilemanager()
		: FilemanagerBase()
		, tooltips_enabled(false)
		, action_allowed(false)
		, special_mode(false)
		, move_mode(false)
	{}

// Запуск программы
bool ConsoleFilemanager::Run()
	{
		setlocale(LC_ALL, "");
		// Не работает ввод кирилицы без этого
		system("chcp 1251"); system("cls");

		// Спрятать курсор
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO info;
		info.dwSize = 100;
		info.bVisible = FALSE;
		SetConsoleCursorInfo(consoleHandle, &info);

		// Изначальное меню
		DriveMenu();

		// Цикл
		while (current_menu && !special_mode)
		{
			std::cout << "\x1B[2J\x1B[H";

			if (tooltips_enabled)
			{
				std::cout << "Управление:  Перемещение - wasd/стрелки  ;  Выбор - enter/e  ;  Назад - esc ; Скрыть подсказки - q/й\n";
			}
			else
			{
				std::cout << "Показать подсказки - q/й\n";
			}
			std::cout << "Буфер: " << selected_path << " ; ";
			std::cout << "Последняя запись: " << (directory_history.empty() ? "пусто" : directory_history.top().path);
			std::cout << std::endl << std::endl;

			current_menu->CoutMenu();

			MenuAction();
		}

		return special_mode;
	}