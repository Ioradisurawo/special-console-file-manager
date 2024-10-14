#pragma once
#include "filemanager_base.h"
#include <chrono>
#include <ctime>

// BASS documentation
// https://www.un4seen.com/doc/#bass/
// BASS 1
// https://www.cyberforum.ru/blogs/201419/blog5149.html?ysclid=m1o08i6hp1226754258
// BASS 2 
// http://www.tiflocomp.ru/games/design/sound_games/bass.php?ysclid=m1o1is83eg536343391



struct DirNavData
{
	// текущий индекс (начало страницы)
	int start_index;

	// подсчёт кол-ва страниц
	double entry_pages;
	// есть-ли остаток
	bool leftover;
	// кол-во страниц
	int pages_amount;

	// предел
	int lim;

public:
	DirNavData()
		: start_index(0)
		, entry_pages(0)
		, leftover(false)
		, pages_amount(0)
		, lim(0)
	{}
	void ReCalc(int current_page, int entries_per_page, int directories_size)
	{
		// текущий индекс (начало страницы)
		start_index = current_page * entries_per_page;

		// подсчёт кол-ва страниц
		double entry_pages = directories_size / double(entries_per_page);
		bool leftover = (entry_pages * 10) - round(entry_pages) > 0 ? true : false;
		int pages_amount = entry_pages;

		// предел
		if (current_page + 1 < pages_amount + leftover)
			lim = (current_page + 1) * entries_per_page;
		else
			lim = directories_size;
	}
};
struct EntityData
{
	std::filesystem::path path;
	// для корректного появления в директории
	std::filesystem::path last_path;
	int page, index,
		counter, // счётчик для перехода из директории в директорию
		counter_limit,
		chase_counter, // счётчик для перемещения в директории
		chase_counter_limit,
		speed_mod, // максимальное прибавляемое число к счётчику
		proximity; // насколько близок
	bool chase_mode, 
		 is_active;

	// Конструктор по-умолчанию
	EntityData(int count_lim, int chs_count_lim, int speed_mod)
	: path("none"), last_path("")
	, is_active(false)
	, chase_mode(false)
	, speed_mod(speed_mod)
	, proximity(-1), page(-1), index(-1) // -1 означает отсутствие в текущей дериктории
	, counter(0), counter_limit(count_lim)
	, chase_counter(0), chase_counter_limit(chs_count_lim)
	{}
};

class SpecialFilemanager : public FilemanagerBase
{
	// Переменные
	
	int collected_files;
	
	std::vector<DirectoryData> generated_files;
	ControlCode current_control_code;

	// entity
	EntityData entity;

	// данные о текущей позиции пользователя в директории
	DirNavData nav_data;

	// пересмотреть надобность данной переменной
	bool action_allowed;

	// проверка на надобность обновления графики
	bool graphics_update_required;

	// Управление и вывод изображения
	void clearConsole();
	void ParseControlAction();
	void MenuAction();

	// End
	void Exit();
	void Shutdown();


	// Entity
	void AmbienceCheck();
	void EntityNavigateToDir();
	void EntityAct();



	// Generation
	void RecursiveGeneration(const int count, int counter, const std::filesystem::path& path);
	void GenerateFiles();


	// Заполнение директорий
	void ReloadDriveDirs();
	void ReloadDirs();


	// Взаимодействие
	void PreviousDir();
	void CoutPageString();
	void CollectFile();

	void EnterDir();
	void PageLeft();
	void PageRight();



	// Меню 
	void DirMenu();
	void DriveMenu();

	void Initialize();
	void Start();
	
	void CoutEntInfo();
	void CoutInfo();
	void Update();
public:

	// Конструктор
	SpecialFilemanager();

	// Запуск программы
	bool Run();
};