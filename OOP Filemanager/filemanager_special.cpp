#include "filemanager_special.h"



// Древность
const int ScreenHeight = 120;
const int ScreenWidth = 220;
COORD charBufSize = { ScreenWidth, ScreenHeight };
COORD characterPos = { 0, 0 };
SMALL_RECT writeArea = { 0, 0, ScreenWidth, ScreenHeight };
CHAR_INFO charInfo[ScreenWidth * ScreenHeight];



// Управление и вывод изображения
void SpecialFilemanager::clearConsole() {
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	SetConsoleCursorPosition(console, topLeft);
}
void SpecialFilemanager::ParseControlAction()
{
	if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		current_control_code = ControlCode::UP;
	if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
		current_control_code = ControlCode::LEFT;
	if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		current_control_code = ControlCode::DOWN;
	if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
		current_control_code = ControlCode::RIGHT;
	if (GetAsyncKeyState((unsigned short)'E') & 0x8000 || GetAsyncKeyState(VK_RETURN))
		current_control_code = ControlCode::CONFIRM;
}
void SpecialFilemanager::MenuAction()
{
	switch (current_control_code)
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
			{
				if (current_index - 1 >= 0)
				{
					current_menu->PreviousItem();
					--current_index;
					graphics_update_required = true;
				}
			}

			audio_player.PlayUserSFX(Audio::SND_MOVE, Random(0, 1));
			break;
		}
		case ControlCode::DOWN:
		{
			if (current_menu->IsVertical())
			{
				if (current_index + 1 < directories.size())
				{
					current_menu->NextItem();
					++current_index;
					graphics_update_required = true;
				}
			}

			audio_player.PlayUserSFX(Audio::SND_MOVE, Random(0, 1));

			break;
		}
		case ControlCode::LEFT:
		{
			if (!current_menu->IsVertical())
				current_menu->PreviousItem();


			PageLeft();
			graphics_update_required = true;

			break;
		}
		case ControlCode::RIGHT:
		{
			if (!current_menu->IsVertical())
				current_menu->NextItem();

			PageRight();
			graphics_update_required = true;

			break;
		}
		default:
			break;
	}

	current_control_code = ControlCode::NONE;
}

// End
void SpecialFilemanager::Exit()
{
	system("cls");
	current_menu = nullptr;

	audio_player.PlayBGM(Audio::MUS_EXIT);
	audio_player.PlayAmbientSFX(Audio::NONE);
	audio_player.PlayUserSFX(Audio::NONE, 0);
	SetColor(bgYELLOW | BLACK);

	std::cout << "\n\n\n\n\n\t\t\t\tyou have managed to collect the files...\n\n\n\t\t\t\t";
	system("pause");
}
void SpecialFilemanager::Shutdown()
{
	current_menu = nullptr;
	audio_player.PlayBGM(Audio::MUS_OVER);
	audio_player.PlayAmbientSFX(Audio::NONE);
	audio_player.PlayUserSFX(Audio::NONE, 0);
	while (true)
	{
		for (int x = 0, gen = 0; x < ScreenWidth; x++)
		{
			for (int y = 0; y < ScreenHeight; y++)
			{
				charInfo[y * ScreenWidth + x].Char.AsciiChar = ' ';
				gen = Random(0, 5);
				switch (gen)
				{
				case 0:
				{
					gen = 0;
					break;
				}
				case 1:
				{
					gen = 128;
					break;

				}
				case 2:
				{
					gen = 240;
					break;

				}
				}
				charInfo[y * ScreenWidth + x].Attributes = gen;
			}
		}
		WriteConsoleOutputA(this->cHandle, charInfo, charBufSize, characterPos, &writeArea);
	}
}


// Entity
void SpecialFilemanager::AmbienceCheck()
{
	if (!entity.is_active)
		return;

	if (entity.proximity > 1)
		audio_player.PlayAmbientSFX(Audio::NONE);
	else if (entity.proximity == 1)
		audio_player.PlayAmbientSFX(Audio::AMB_NEAR);
	else if (entity.proximity == 0)
		audio_player.PlayAmbientSFX(Audio::AMB_PROX);
}
void SpecialFilemanager::EntityNavigateToDir()
{
	// обновление крайнего пути
	entity.last_path = entity.path;

	std::filesystem::path move = std::filesystem::relative(selected_path, entity.path);

	// подсчёт насколько близок и определение следующего пути
	entity.proximity = 1;
	while (move.parent_path() != "")
	{
		move = move.parent_path();
		++entity.proximity;
	}

	// проверка на действие
	if (entity.counter >= entity.counter_limit)
	{
		if (move != "..")
		{
			entity.path += "/";
			entity.path += move;
		}
		else
		{
			entity.path = entity.path.parent_path();
		}
		entity.counter = 0;
	}
	else
	{
		entity.counter += Random(0, entity.speed_mod);
	}

}
void SpecialFilemanager::EntityAct()
{
	if (!entity.is_active)
		return;

	if (entity.index == current_index + (entries_per_page * current_page))
		Shutdown();

	// не в папке с пользователем
	if (entity.path != selected_path)
	{
		entity.index = -1;
		entity.chase_mode = false;

		EntityNavigateToDir();

	}
	// в папке с пользователем
	else
	{
		entity.proximity = 0;
		// уже преследует
		if (entity.chase_mode)
		{
			if (entity.chase_counter >= entity.chase_counter_limit)
			{
				if (entity.index > current_index + (entries_per_page * current_page))
					--entity.index;
				else
					++entity.index;

				//the_entity.index += current_index > the_entity.index ? 1 : -1;
			}
			else
			{
				entity.chase_counter += Random(0, entity.speed_mod);
			}
			directories.at(entity.index).replace_filename("???");
		}
		// начало преследования в директории
		else
		{
			entity.counter = 0;
			entity.chase_mode = true;

			int i = 0;
			for (auto& entry : directories)
			{
				if (entry == entity.last_path)
					break;
				++i;
			}
			entity.index = i - 1;

			entity.last_path = entity.path;
		}
	}
}


// Generation
void SpecialFilemanager::RecursiveGeneration(const int count, int counter, const std::filesystem::path& path)
{
	if (count >= counter)
	{
		// Поиск одинаковой записи
		bool found = false;
		for (auto& gen : generated_files)
		{
			if (gen.path == path)
				found = true;
		}

		if (found)
			counter += Random(0, 10);
		else
		{
			// подсчёт размера директории
			int dir_size = 0;
			for (auto& entry : std::filesystem::directory_iterator(path))
				++dir_size;

			// спавн файла на случайной позиции от 0 до размера дир
			generated_files.push_back(DirectoryData(path, 0, Random(0, dir_size - 1)));
			return;
		}
	}

	try
	{
		for (auto& entry : std::filesystem::directory_iterator(path, std::filesystem::directory_options::skip_permission_denied))
		{
			if (entry.path().filename().string().at(0) != '$' && entry.is_directory() && Random(0, 10) > 5)
			{
				RecursiveGeneration(count + Random(0, 10), counter, entry);
				return;
			}
		}
	}
	catch (std::exception)
	{

	}
}
void SpecialFilemanager::GenerateFiles()
{
	while (generated_files.size() < 8)
	{
		RecursiveGeneration(0, 10, selected_path);
	}
}


// Заполнение директорий
void SpecialFilemanager::ReloadDriveDirs()
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
void SpecialFilemanager::ReloadDirs()
{
	if (selected_path.empty())
		return;
	directories.clear();
	for (auto& entry : std::filesystem::directory_iterator(selected_path
		// Важная штука
		, std::filesystem::directory_options::skip_permission_denied))
	{
		directories.push_back(entry.path().string());
	}
}


// Взаимодействие
void SpecialFilemanager::CoutPageString()
{
	std::filesystem::path file_str = std::filesystem::relative("data/img/");
	switch (collected_files)
	{
	case 8:
	{
		file_str.append("scp.txt");
		break;
	}
	case 7:
	{
		file_str.append("osk.txt");
		break;
	}
	case 6:
	{
		file_str.append("melnitsa.txt");
		break;
	}
	case 5:
	{
		file_str.append("heihachi.txt");
		break;
	}
	case 4:
	{
		file_str.append("smile.txt");
		break;
	}
	case 3:
	{
		file_str.append("nagiev.txt");
		break;
	}
	case 2:
	{
		file_str.append("bman.txt");
		break;
	}
	case 1:
	{
		file_str.append("laugh.txt");
		break;
	}
	}

	std::string text;
	CopyFileToString(std::filesystem::relative(file_str), text);
	std::cout << text << "\n";
	system("pause");
}
void SpecialFilemanager::CollectFile()
{
	audio_player.PlayUserSFX(Audio::SND_COLLECT, 0);

	// получение из вектора директорий 
	int collected_index = current_menu->GetItemIndex() + (current_page * entries_per_page);
	std::filesystem::path file_path = directories.at(collected_index).parent_path();

	// поиск в векторе файлов
	int i = 0;
	for (auto& gen : generated_files)
	{
		if (gen.path == file_path)
		{
			++collected_files;
			// изменить музыку в зависимости от кол-ва файлов
			switch (collected_files)
			{
			case 8:
			{
				audio_player.PlayBGM(Audio::MUS_STAGE_5);
				break;
			}
			case 6:
			{
				audio_player.PlayBGM(Audio::MUS_STAGE_4);
				break;
			}
			case 4:
			{
				audio_player.PlayBGM(Audio::MUS_STAGE_3);

				break;
			}
			case 2:
			{
				audio_player.PlayBGM(Audio::MUS_STAGE_2);

				break;
			}
			}
			break;
		}
		++i;
	}
	// удаление из вектора файлов
	generated_files.erase(generated_files.begin() + i);
	current_index = collected_index;

	CoutPageString();

	DirMenu();
}


// Перемещение
void SpecialFilemanager::PreviousDir()
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
	}
	else
	{
		directory_history.pop();
		selected_path = directory_history.top().path;
	}

	DirMenu();
	graphics_update_required = false;
	audio_player.PlayUserSFX(Audio::SND_ENTER, 0);
}
void SpecialFilemanager::EnterDir()
{
	try
	{
		current_index = current_menu->GetItemIndex() + (current_page * entries_per_page);

		if (std::filesystem::is_directory(directories.at(current_index)))
		{
			// меняем текущий путь на выбранный элемент
			selected_path = directories.at(current_index);

			// сохраняем текущий путь, индекс страницы и индекс элемента на этой странице
			directory_history.push(DirectoryData(selected_path, current_page, current_menu->GetItemIndex()));

			// устанавливаем индексы по-умолчанию
			current_page = 0;
			current_index = 0;

			DirMenu();
			graphics_update_required = true;
			audio_player.PlayUserSFX(Audio::SND_ENTER, 0);
		}
		else
		{
			// меняем текущий путь на выбранный элемент
			// selected_path = directories.at(current_index);
			// сохраняем текущий путь, индекс страницы и индекс элемента на этой странице
			// directory_history.push(DirectoryData(selected_path, current_page, current_menu->GetItemIndex()));
		}
	}
	catch (std::exception)
	{
		std::cout << "\n\nNot enough perms\n\n";
		system("pause");
	}

}
void SpecialFilemanager::PageLeft()
{
	if (!current_paged_menu)
		return;

	current_index = 0;
	if (current_page - 1 >= 0)
	{
		--current_page;
		audio_player.PlayUserSFX(Audio::SND_MOVE, Random(0, 1));
		current_paged_menu();
	}
}
void SpecialFilemanager::PageRight()
{
	if (!current_paged_menu)
		return;

	current_index = 0;
	if (current_page + 1 <= directories.size() / entries_per_page)
	{
		++current_page;
		audio_player.PlayUserSFX(Audio::SND_MOVE, Random(0, 1));
		current_paged_menu();
	}
}


// Меню 
void SpecialFilemanager::DirMenu()
{
	// для вызова меню действий
	action_allowed = true;
	// для page_left и page_right
	current_paged_menu = std::bind(&SpecialFilemanager::DirMenu, this);

	// перезагрузка директорий
	ReloadDirs();

	// Пересчитать текущую позицию пользователя
	nav_data.ReCalc(current_page, entries_per_page, directories.size());

	// Вернуться нельзя
	previous_menu = std::bind(directory_history.empty() ? nullptr : &SpecialFilemanager::PreviousDir, this);


	// Настройка меню
	std::ostringstream lower;
	lower << "|_ cтраница " << std::to_string(current_page) << " из " << nav_data.pages_amount << " (" << nav_data.start_index << "-" << directories.size() << ")_|\n";

	current_menu = std::make_shared<Menu>
		(
			cHandle, collected_files == 8 ? fgRED : fgGRAY,
			"[-- " + selected_path.string() + " --]\n",
			lower.str(),
			"->> ", "  <>-",
			"| ", "  ",
			"\n"
		);

	// Установка индекса текущего элемента
	current_menu->SetItemIndex(current_index);

	// Вставка выхода в первый элемент
	if (directory_history.size() > 1)
		directories.insert(directories.begin(), "^");
	else if (collected_files == 8)
		directories.insert(directories.begin(), "EXIT");


	if (entity.chase_mode)
	{
		if (entity.index >= directories.size())
			entity.index = directories.size() - 1;

		directories.at(entity.index).replace_filename("???");
	}

	// Проверка есть в данной директории один из файлов и добавление его в вектор дерикторий в случайной точке
	for (auto& gen : generated_files)
	{
		if (gen.path == selected_path)
		{
			directories.insert(directories.begin() + gen.index, gen.path + "/unk_file???.txt");
		}
	}



	// Заполнение пунктами меню
	int i = 0;
	std::string type_str;
	Color type_color;
	for (auto& entry : directories)
	{
		if (i == nav_data.lim)
			break;

		if (i >= nav_data.start_index)
		{
			if (entry.filename() == "EXIT")
				current_menu->AddItem(std::make_shared<MenuItem>("\t\t[ RETURN ]", fgYELLOW, std::bind(&SpecialFilemanager::Exit, this)));
			// подъем
			if (entry.filename() == "^")
				current_menu->AddItem(std::make_shared<MenuItem>("[    ]\t/..", collected_files == 8 ? fgRED : fgWHITE, std::bind(&SpecialFilemanager::PreviousDir, this)));
			else
				// entity
				if (entry.filename() == "???")
					current_menu->AddItem(std::make_shared<MenuItem>("[???]\t???", fgMAGENTA, std::bind(&SpecialFilemanager::Shutdown, this)));
				else
					// collectable file
					if (entry.filename().replace_extension("") == "unk_file???")
						current_menu->AddItem(std::make_shared<MenuItem>("[файл]\t" + entry.filename().string(), collected_files == 8 ? fgRED : fgYELLOW, std::bind(&SpecialFilemanager::CollectFile, this)));
					else
					{
						if (std::filesystem::is_directory(entry))
						{
							// entity nearby
							if (entity.path == entry)
								current_menu->AddItem(std::make_shared<MenuItem>("[папка]\t" + entry.filename().string(), fgMAGENTA, std::bind(&SpecialFilemanager::EnterDir, this)));
							// default
							else
								current_menu->AddItem(std::make_shared<MenuItem>("[папка]\t" + entry.filename().string(), collected_files == 8 ? fgRED : fgWHITE, std::bind(&SpecialFilemanager::EnterDir, this)));
						}
						// file
						else
							current_menu->AddItem(std::make_shared<MenuItem>("[файл]\t" + entry.filename().string(), collected_files == 8 ? fgRED : fgGRAY, std::bind(&SpecialFilemanager::EnterDir, this)));
					}
		}
		++i;
	}

	// Возврат на пункт выше
}
void SpecialFilemanager::DriveMenu()
{
	action_allowed = false;
	current_paged_menu = nullptr;

	if (!directory_history.empty())
		directory_history.pop();

	// Настройка меню
	previous_menu = std::bind(&SpecialFilemanager::Shutdown, this);
	current_menu = std::make_shared<Menu>
		(
			cHandle, fgGRAY,
			"[-- . . .  --]\n",
			"|_                _]\n",
			"[ ", "  ]",
			"| ", "  ",
			"\n"
		);

	//
	ReloadDriveDirs();

	for (auto& entry : directories)
	{
		current_menu->AddItem(std::make_shared<MenuItem>(entry.string(), fgGRAY, std::bind(&SpecialFilemanager::Start, this)));
	}
	current_menu->AddItem(std::make_shared<MenuItem>("Выход", fgRED, std::bind(&SpecialFilemanager::Shutdown, this)));
}

// Инициализация игры
void SpecialFilemanager::Initialize()
{
	setlocale(LC_ALL, "");
	// Не работает ввод кирилицы без этого
	system("chcp 1251"); system("cls");

	// Спрятать курсор
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(cHandle, &info);

	// Изначальное меню
	DriveMenu();

	audio_player.mute = true;
}
void SpecialFilemanager::Start()
{
	SelectEntry();

	audio_player.mute = false;
	audio_player.PlayBGM(Audio::MUS_STAGE_1);

	//
	GenerateFiles();
	entity.path = generated_files.at(generated_files.size() - 1).path;

	system("cls");
	std::cout << "\n\n\n\t\t\t\tcollect 8 files";
	Sleep(2000);
	graphics_update_required = true;

	entity.is_active = true;
}

// Вывод информации
void SpecialFilemanager::CoutEntInfo()
{
	std::cout << SetColor(fgDARK_MAGENTA) << "ENTITY: " << entity.path
		<< " : idx=" << entity.index
		<< " pg=" << entity.page
		<< " cnt=" << entity.counter
		<< " prox=" << entity.proximity
		<< SetColor(fgGRAY) << "\n";
}
void SpecialFilemanager::CoutInfo()
{
	// вывод мест файлов
	for (auto& gen : generated_files)
	{
		std::cout << gen.path << " : pos=" << gen.index << "\n";
	}

	if (collected_files == 8)
	{
		std::cout << SetColor(fgRED) << "\tLEAVE\n";
	}
}

// Обновление логики и графики
void SpecialFilemanager::Update()
{
	// 
	EntityAct();

	AmbienceCheck();

	if (current_paged_menu)
		current_paged_menu();

	if (graphics_update_required)
	{
		// очистка консоли
		clearConsole();

		// вывод подробных данных
		CoutInfo();

		// вывод меню
		current_menu->CoutMenu();

		graphics_update_required = false;
	}

	// проверка действия пользователя
	MenuAction();
}

// Конструктор
SpecialFilemanager::SpecialFilemanager()
		: FilemanagerBase()
		, entity(20000, 50, 10)
		, collected_files(0)
		, current_control_code(ControlCode::NONE)
		, action_allowed(false)
		, graphics_update_required(true)
	{}

// Запуск
bool SpecialFilemanager::Run()
	{
		Initialize();

		// Таймер графики
		Timer timer_graphics;

		// Таймер ввода
		Timer timer_input;

		// Цикл
		while (current_menu)
		{
			if(timer_input.ThresholdReached(50))
				ParseControlAction();

			if (timer_graphics.ThresholdReached(10))
				Update();
		}

		return false;
	}