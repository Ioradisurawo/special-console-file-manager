#include "filemanager_special.h"

// ������
typedef std::mt19937 rng_type;
rng_type rng;
rng_type::result_type const seedval = time(0);
int				Random(const int& from, const int& to)
{
	std::uniform_int_distribution<rng_type::result_type> random(from, to);

	return random(rng);
}

// ���������
const int ScreenHeight = 120;
const int ScreenWidth = 220;
COORD charBufSize = { ScreenWidth, ScreenHeight };
COORD characterPos = { 0, 0 };
SMALL_RECT writeArea = { 0, 0, ScreenWidth, ScreenHeight };
CHAR_INFO charInfo[ScreenWidth * ScreenHeight];

// ���������� � ����� �����������
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
	/*
	if (GetAsyncKeyState(VK_ESCAPE))
		current_control_code = ControlCode::DECLINE;
	*/
}
void SpecialFilemanager::MenuAction()
{
	ControlCode empt = ControlCode::NONE;
	switch (current_control_code)
	{
	case ControlCode::CONFIRM:
	{
		std::cout << "confirm";
		current_menu->InvokeItem();
		break;
	}
	case ControlCode::DECLINE:
	{
		std::cout << "decline";

		if (previous_menu)
			previous_menu();
		break;
	}
	case ControlCode::UP:
	{
		std::cout << "up";

		if (current_menu->IsVertical())
		{
			if (current_index - 1 >= 0)
			{
				current_menu->PreviousItem();
				--current_index;
			}
		}

		audio_player.PlayUserSFX(Audio::SND_MOVE, Random(0, 1));
		break;
	}
	case ControlCode::DOWN:
	{
		std::cout << "down";

		if (current_menu->IsVertical())
		{
			if (current_index + 1 < directories.size())
			{
				current_menu->NextItem();
				++current_index;
			}
		}

		audio_player.PlayUserSFX(Audio::SND_MOVE, Random(0, 1));

		break;
	}
	case ControlCode::LEFT:
	{
		std::cout << "left";

		if (!current_menu->IsVertical())
			current_menu->PreviousItem();


		PageLeft();

		break;
	}
	case ControlCode::RIGHT:
	{
		std::cout << "right";

		if (!current_menu->IsVertical())
			current_menu->NextItem();
		PageRight();

		break;
	}
	case ControlCode::ACTION:
	{
		std::cout << "act";
		break;
	}
	default:
	{
		std::cout << "none";
		break;
	}
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
	if (!the_entity.is_active)
		return;

	if (the_entity.proximity > 1)
		audio_player.PlayAmbientSFX(Audio::NONE);
	else if (the_entity.proximity == 1)
		audio_player.PlayAmbientSFX(Audio::AMB_NEAR);
	else if (the_entity.proximity == 0)
		audio_player.PlayAmbientSFX(Audio::AMB_PROX);
}
void SpecialFilemanager::EntityNavigateToDir()
{
	// ���������� �������� ����
	the_entity.last_path = the_entity.path;

	std::filesystem::path move = std::filesystem::relative(selected_path, the_entity.path);

	// ������� ��������� ������ � ����������� ���������� ����
	the_entity.proximity = 1;
	while (move.parent_path() != "")
	{
		move = move.parent_path();
		++the_entity.proximity;
	}

	// �������� �� ��������
	if (the_entity.counter >= the_entity.counter_limit)
	{
		if (move != "..")
		{
			the_entity.path += "/";
			the_entity.path += move;
		}
		else
		{
			the_entity.path = the_entity.path.parent_path();
		}
		the_entity.counter = 0;
	}
	else
	{
		the_entity.counter += Random(0, the_entity.speed_mod);
	}

}
void SpecialFilemanager::EntityAct()
{
	if (!the_entity.is_active)
		return;

	if (the_entity.index == current_index + (entries_per_page * current_page))
		Shutdown();

	// �� � ����� � �������������
	if (the_entity.path != selected_path)
	{
		the_entity.index = -1;
		the_entity.chase_mode = false;

		EntityNavigateToDir();

	}
	// � ����� � �������������
	else
	{
		the_entity.proximity = 0;
		// ��� ����������
		if (the_entity.chase_mode)
		{
			if (the_entity.chase_counter >= the_entity.chase_counter_limit)
			{
				if (the_entity.index > current_index + (entries_per_page * current_page))
					--the_entity.index;
				else
					++the_entity.index;

				//the_entity.index += current_index > the_entity.index ? 1 : -1;
			}
			else
			{
				the_entity.chase_counter += Random(0, the_entity.speed_mod);
			}
			directories.at(the_entity.index).replace_filename("???");
		}
		// ������ ������������� � ����������
		else
		{
			the_entity.counter = 0;
			the_entity.chase_mode = true;

			int i = 0;
			for (auto& entry : directories)
			{
				if (entry == the_entity.last_path)
					break;
				++i;
			}
			the_entity.index = i - 1;

			the_entity.last_path = the_entity.path;
		}
	}
}


// Generation
void SpecialFilemanager::RecursiveGeneration(const int count, int counter, const std::filesystem::path& path)
{
	if (count >= counter)
	{
		// ����� ���������� ������
		bool found = false;
		for (auto& gen : generated_filesNEW)
		{
			if (gen.path == path)
				found = true;
		}

		if (found)
			counter += Random(0, 10);
		else
		{
			// ������� ������� ����������
			int dir_size = 0;
			for (auto& entry : std::filesystem::directory_iterator(path))
				++dir_size;

			// ����� ����� �� ��������� ������� �� 0 �� ������� ���
			generated_filesNEW.push_back(DirectoryData(path, 0, Random(0, dir_size - 1)));
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
	while (generated_filesNEW.size() < 8)
	{
		RecursiveGeneration(0, 10, selected_path);
	}
}


// ���������� ����������
void SpecialFilemanager::ReloadDriveDirs()
{
	directories.clear();
	// ���������� ������
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
		// ������ �����
		, std::filesystem::directory_options::skip_permission_denied))
	{
		directories.push_back(entry.path().string());
	}
}


// ��������������
void SpecialFilemanager::PreviousDir()
{
	// ���� ����� ������� ���� �������� (�.�. �� �� � ���� ��������)
	if (std::filesystem::is_directory(selected_path))
	{
		// ���� ��� ���� ��������
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
	audio_player.PlayUserSFX(Audio::SND_ENTER, 0);
}
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

	// ��������� �� ������� ���������� 
	int collected_index = current_menu->GetItemIndex() + (current_page * entries_per_page);
	std::filesystem::path file_path = directories.at(collected_index).parent_path();

	// ����� � ������� ������
	int i = 0;
	for (auto& gen : generated_filesNEW)
	{
		if (gen.path == file_path)
		{
			++collected_files;
			// �������� ������ � ����������� �� ���-�� ������
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
	// �������� �� ������� ������
	generated_filesNEW.erase(generated_filesNEW.begin() + i);
	current_index = collected_index;

	CoutPageString();

	DirMenu();
}

void SpecialFilemanager::EnterDir()
{
	try
	{
		current_index = current_menu->GetItemIndex() + (current_page * entries_per_page);

		if (std::filesystem::is_directory(directories.at(current_index)))
		{
			// ������ ������� ���� �� ��������� �������
			selected_path = directories.at(current_index);

			// ��������� ������� ����, ������ �������� � ������ �������� �� ���� ��������
			directory_history.push(DirectoryData(selected_path, current_page, current_menu->GetItemIndex()));

			// ������������� ������� ��-���������
			current_page = 0;
			current_index = 0;

			DirMenu();
			audio_player.PlayUserSFX(Audio::SND_ENTER, 0);
		}
		else
		{
			// ������ ������� ���� �� ��������� �������
			// selected_path = directories.at(current_index);
			// ��������� ������� ����, ������ �������� � ������ �������� �� ���� ��������
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



// ���� 
void SpecialFilemanager::DirMenu()
{
	// ��� ������ ���� ��������
	action_allowed = true;
	// ��� page_left � page_right
	current_paged_menu = std::bind(&SpecialFilemanager::DirMenu, this);

	// ������������ ����������
	ReloadDirs();

	// ����������� ������� ������� ������������
	nav_data.ReCalc(current_page, entries_per_page, directories.size());

	// ��������� ������
	previous_menu = std::bind(directory_history.empty() ? nullptr : &SpecialFilemanager::PreviousDir, this);


	// ��������� ����
	std::ostringstream lower;
	lower << "|_ c������� " << std::to_string(current_page) << " �� " << nav_data.pages_amount << " (" << nav_data.start_index << "-" << directories.size() << ")_|\n";

	current_menu = std::make_shared<Menu>
		(
			cHandle, collected_files == 8 ? fgRED : fgGRAY,
			"[-- " + selected_path.string() + " --]\n",
			lower.str(),
			"->> ", "  <>-",
			"| ", "  ",
			"\n"
		);

	// ��������� ������� �������� ��������
	current_menu->SetItemIndex(current_index);

	// ������� ������ � ������ �������
	if (directory_history.size() > 1)
		directories.insert(directories.begin(), "^");
	else if (collected_files == 8)
		directories.insert(directories.begin(), "EXIT");


	if (the_entity.chase_mode)
	{
		if (the_entity.index >= directories.size())
			the_entity.index = directories.size() - 1;

		directories.at(the_entity.index).replace_filename("???");
	}

	// �������� ���� � ������ ���������� ���� �� ������ � ���������� ��� � ������ ���������� � ��������� �����
	for (auto& gen : generated_filesNEW)
	{
		if (gen.path == selected_path)
		{
			directories.insert(directories.begin() + gen.index, gen.path + "/unk_file???.txt");
		}
	}



	// ���������� �������� ����
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
			// ������
			if (entry.filename() == "^")
				current_menu->AddItem(std::make_shared<MenuItem>("[    ]\t/..", collected_files == 8 ? fgRED : fgWHITE, std::bind(&SpecialFilemanager::PreviousDir, this)));
			else
				// entity
				if (entry.filename() == "???")
					current_menu->AddItem(std::make_shared<MenuItem>("[???]\t???", fgMAGENTA, std::bind(&SpecialFilemanager::Shutdown, this)));
				else
					// collectable file
					if (entry.filename().replace_extension("") == "unk_file???")
						current_menu->AddItem(std::make_shared<MenuItem>("[����]\t" + entry.filename().string(), collected_files == 8 ? fgRED : fgYELLOW, std::bind(&SpecialFilemanager::CollectFile, this)));
					else
					{
						if (std::filesystem::is_directory(entry))
						{
							// entity nearby
							if (the_entity.path == entry)
								current_menu->AddItem(std::make_shared<MenuItem>("[�����]\t" + entry.filename().string(), fgMAGENTA, std::bind(&SpecialFilemanager::EnterDir, this)));
							// default
							else
								current_menu->AddItem(std::make_shared<MenuItem>("[�����]\t" + entry.filename().string(), collected_files == 8 ? fgRED : fgWHITE, std::bind(&SpecialFilemanager::EnterDir, this)));
						}
						// file
						else
							current_menu->AddItem(std::make_shared<MenuItem>("[����]\t" + entry.filename().string(), collected_files == 8 ? fgRED : fgGRAY, std::bind(&SpecialFilemanager::EnterDir, this)));
					}
		}
		++i;
	}

	// ������� �� ����� ����
}
void SpecialFilemanager::DriveMenu()
{
	action_allowed = false;
	current_paged_menu = nullptr;

	if (!directory_history.empty())
		directory_history.pop();

	// ��������� ����
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
	current_menu->AddItem(std::make_shared<MenuItem>("�����", fgRED, std::bind(&SpecialFilemanager::Shutdown, this)));
}

void SpecialFilemanager::Initialize()
{
	// seed rng first:
	rng_type::result_type const seedval = time(0);
	rng.seed(seedval);

	setlocale(LC_ALL, "");
	// �� �������� ���� �������� ��� �����
	system("chcp 1251"); system("cls");

	// �������� ������
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(cHandle, &info);

	// ����������� ����
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
	the_entity.path = generated_filesNEW.at(generated_filesNEW.size() - 1).path;

	system("cls");
	std::cout << "\n\n\n\t\t\t\tcollect 8 files";
	Sleep(2000);

	the_entity.is_active = true;
}

void SpecialFilemanager::CoutEntInfo()
{
	std::cout << SetColor(fgDARK_MAGENTA) << "ENTITY: " << the_entity.path
		<< " : idx=" << the_entity.index
		<< " pg=" << the_entity.page
		<< " cnt=" << the_entity.counter
		<< " prox=" << the_entity.proximity
		<< SetColor(fgGRAY) << "\n";
}
void SpecialFilemanager::CoutInfo()
{
	// ����� ���� ������
	for (auto& gen : generated_filesNEW)
	{
		std::cout << gen.path << " : pos=" << gen.index << "\n";
	}

	if (collected_files == 8)
	{
		std::cout << SetColor(fgRED) << "\tLEAVE\n";
	}
}
void SpecialFilemanager::Update()
{
	// 
	EntityAct();

	AmbienceCheck();

	if (current_paged_menu)
		current_paged_menu();



	// ������� �������
	clearConsole();

	// ����� ��������� ������
	CoutInfo();

	// ����� ����
	current_menu->CoutMenu();

	// �������� �������� ������������
	MenuAction();
}

// �����������
SpecialFilemanager::SpecialFilemanager()
		: FilemanagerBase()
		, the_entity(200, 50, 10)
		, collected_files(0)
		, current_control_code(ControlCode::NONE)
		, action_allowed(false)
	{}

// ������ ���������
bool SpecialFilemanager::Run()
	{
		Initialize();

		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point end;
		std::chrono::milliseconds duration;

		// ����
		while (current_menu)
		{
			end = std::chrono::steady_clock::now();
			duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
			if (duration.count() > 200)
			{
				Update();
				start = std::chrono::steady_clock::now();
			}
			ParseControlAction();
		}

		return false;
	}