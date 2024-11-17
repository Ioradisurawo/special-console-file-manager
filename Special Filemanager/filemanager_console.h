#pragma once
#include "filemanager_base.h"


class ConsoleFilemanager : public FilemanagerBase
{
	std::vector<ControlCode> special_code;

	bool special_mode, 
		 move_mode, 
		 action_allowed, 
		 tooltips_enabled;

	std::filesystem::path saved_path;

	// // // Базированность

	virtual void	SpecialCodeCheck(const ControlCode& code);
	void			MenuAction();
	void			Shutdown();



	// Заполнение директорий
	void ReloadDriveDirs();
	void ReloadDirs();


	
	// Действия (общее)
	void CalculateSize();

	// Действия (директория)
	void DirCreate();
	void FileCreate();
	void EntryCreateMenu();
	void EntryPaste();

	void SearchResultMenu();
	void RecursiveQuerySearch(const std::filesystem::path path, const std::string query);
	void FolderSearch();

	// Действие (файл)
	void EntryRename();
	void EntryCopy();
	void EntryMove();
	void EntryDelete();



	// Перемещение по директориям
	void PreviousDir();
	void SelectEntry();
	void PageLeft();
	void PageRight();



	// Меню 
	void EntryMenu();
	void DirMenu();
	void DriveMenu();
public:

	// Конструктор
	ConsoleFilemanager();

	// Запуск программы
	bool Run();
};