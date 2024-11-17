#pragma once
#include <iostream>
#include <vector>
#include <stack>
#include <memory>
#include <filesystem>
#include <functional>
#include <windows.h>

#include "additional.h"
#include "menu_coloured.h"
#include "menu_controller.h"

#include "audio_player.h"

struct DirectoryData
{
	std::filesystem::path path;
	int page;
	int index;
	DirectoryData(std::filesystem::path path, int page, int index)
		: path(path)
		, page(page)
		, index(index)
	{}
};

class FilemanagerBase
{
protected:
	HANDLE									cHandle;

	// BASS поток
	AudioPlayer								audio_player;

	std::function<void()>					current_paged_menu;
	std::function<void()>					previous_menu;
	std::shared_ptr<Menu>					current_menu;

	std::vector<std::filesystem::path>		directories;
	std::stack<DirectoryData>				directory_history;

	int entries_per_page;
	int current_page;
	int current_index;

	std::filesystem::path selected_path;

	// // // Базированность

	const std::string& SetColor(WORD color);
	virtual void MenuAction();

	// // // Прочее

	virtual void Shutdown();

	// Заполнение директорий
	virtual void ReloadDriveDirs();
	virtual void ReloadDirs();

	// Перемещение по директориям
	virtual void SelectEntry();
	virtual void PreviousDir();
	virtual void PageLeft();
	virtual void PageRight();

	virtual void DirMenu();
	virtual void DriveMenu();
public:

	// Конструктор
	FilemanagerBase();

	// Запуск программы
	virtual bool Run();
};