#include "filemanager_base.h"

const std::string& FilemanagerBase::SetColor(WORD color)
{
	SetConsoleTextAttribute(cHandle, color);
	return "";
}
 void FilemanagerBase::MenuAction()
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
	default:
	{
		break;
	}
	}
}

// // // ������

void FilemanagerBase::Shutdown()
{
	current_menu = nullptr;
}


// ���������� ����������

void FilemanagerBase::ReloadDriveDirs()
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
void FilemanagerBase::ReloadDirs()
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


// ����������� �� �����������

void FilemanagerBase::SelectEntry()
{
	try
	{
		current_index = current_menu->GetItemIndex() + (current_page * entries_per_page);

		if (std::filesystem::is_directory(directories.at(current_index)))
		{
			// ������ ������� ���� �� ��������� �������
			selected_path = directories.at(current_index);

			// ���� � ������ ���� ����� ������� ���� ��������
			//if(action_allowed)
				// ��������� ������� ����, ������ �������� � ������ �������� �� ���� ��������
			directory_history.push(DirectoryData(selected_path, current_page, current_menu->GetItemIndex()));

			// ������������� ������� ��-���������
			current_page = 0;
			current_index = 0;

			DirMenu();
		}
		else
		{
			// ������
		}
	}
	catch (std::exception)
	{
		std::cout << "\n\nNot enough perms\n\n";
		system("pause");
	}

}
void FilemanagerBase::PreviousDir()
{
	// ���� ����� ������� ���� �������� (�.�. �� �� � ���� ��������)
	if (std::filesystem::is_directory(selected_path))
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
		directory_history.pop();
		selected_path = directory_history.top().path;
	}

	DirMenu();
}
void FilemanagerBase::PageLeft()
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
void FilemanagerBase::PageRight()
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

void FilemanagerBase::DirMenu()
{
	// ��� page_left � page_right
	current_paged_menu = std::bind(&FilemanagerBase::DirMenu, this);

	ReloadDirs();

	// ������� ������ (������ ��������)
	int start_index = current_page * entries_per_page;

	// ������� ���-�� �������
	double entry_pages = directories.size() / double(entries_per_page);
	bool leftover = (entry_pages * 10) - round(entry_pages) > 0 ? true : false;
	int pages_amount = entry_pages;

	// ������
	int lim;
	if (current_page + 1 < pages_amount + leftover)
		lim = (current_page + 1) * entries_per_page;
	else
		lim = directories.size();

	// ���� ������� ������ ������ ��������� � ������������ ���������� ����� � ������ ����
	previous_menu = std::bind(directory_history.empty() ? &FilemanagerBase::DriveMenu : &FilemanagerBase::PreviousDir, this);

	// ��������� ����
	std::ostringstream lower;
	lower << "|_ c������� " << std::to_string(current_page) << " �� " << pages_amount << " (" << start_index << "-" << directories.size() << ")_|\n";

	current_menu = std::make_shared<Menu>
		(
			cHandle, fgWHITE,
			"[-- " + selected_path.string() + " --]\n",
			lower.str(),
			"->> ", "  <>-",
			"| ", "  ",
			"\n"
		);

	// ��������� �������
	current_menu->SetItemIndex(current_index);

	// ���������� �������� ����
	std::filesystem::path entry;
	for (int i = start_index; i < lim; i++)
	{
		entry = directories.at(i);
		current_menu->AddItem(std::make_shared<MenuItem>((std::filesystem::is_directory(entry) ? "[�����]\t" : "[����]\t") + entry.filename().string(), fgWHITE, std::bind(&FilemanagerBase::SelectEntry, this)));
	}
}
void FilemanagerBase::DriveMenu()
{
	current_paged_menu = nullptr;

	if (!directory_history.empty())
		directory_history.pop();

	// ��������� ����
	previous_menu = std::bind(&FilemanagerBase::Shutdown, this);
	current_menu = std::make_shared<Menu>
		(
			cHandle, fgWHITE,
			"[-- ����� �����  --]\n",
			"|_                _]\n",
			"[ ", "  ]",
			"| ", "  ",
			"\n"
		);

	//
	ReloadDriveDirs();

	for (auto& entry : directories)
	{
		current_menu->AddItem(std::make_shared<MenuItem>(entry.string(), fgWHITE, std::bind(&FilemanagerBase::SelectEntry, this)));
	}


	current_menu->AddItem(std::make_shared<MenuItem>("�����", fgWHITE, std::bind(&FilemanagerBase::Shutdown, this)));
}

	// �����������

FilemanagerBase::FilemanagerBase()
		: cHandle(GetStdHandle(STD_OUTPUT_HANDLE))
		, current_index(0)
		, current_page(0)
		, entries_per_page(25)
	{}

	// ������ ���������
bool FilemanagerBase::Run()
	{
		setlocale(LC_ALL, "");
		// �� �������� ���� �������� ��� �����
		system("chcp 1251"); system("cls");

		// �������� ������
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO info;
		info.dwSize = 100;
		info.bVisible = FALSE;
		SetConsoleCursorInfo(consoleHandle, &info);

		// ����������� ����
		DriveMenu();

		// ����
		while (current_menu)
		{
			std::cout << "\x1B[2J\x1B[H"
				<< "����������:  ����������� - wasd/�������  ;  ����� - enter/e  ;  ����� - esc\n";
			std::cout << "�����: " << selected_path << " ; ";

			std::cout << "��������� ������: " << (directory_history.empty() ? "�����" : directory_history.top().path);

			std::cout << std::endl << std::endl;

			current_menu->CoutMenu();

			MenuAction();
		}

		return 0;
	}