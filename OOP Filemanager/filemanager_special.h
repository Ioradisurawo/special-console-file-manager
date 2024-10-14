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
	// ������� ������ (������ ��������)
	int start_index;

	// ������� ���-�� �������
	double entry_pages;
	// ����-�� �������
	bool leftover;
	// ���-�� �������
	int pages_amount;

	// ������
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
		// ������� ������ (������ ��������)
		start_index = current_page * entries_per_page;

		// ������� ���-�� �������
		double entry_pages = directories_size / double(entries_per_page);
		bool leftover = (entry_pages * 10) - round(entry_pages) > 0 ? true : false;
		int pages_amount = entry_pages;

		// ������
		if (current_page + 1 < pages_amount + leftover)
			lim = (current_page + 1) * entries_per_page;
		else
			lim = directories_size;
	}
};
struct EntityData
{
	std::filesystem::path path;
	// ��� ����������� ��������� � ����������
	std::filesystem::path last_path;
	int page, index,
		counter, // ������� ��� �������� �� ���������� � ����������
		counter_limit,
		chase_counter, // ������� ��� ����������� � ����������
		chase_counter_limit,
		speed_mod, // ������������ ������������ ����� � ��������
		proximity; // ��������� ������
	bool chase_mode, 
		 is_active;

	// ����������� ��-���������
	EntityData(int count_lim, int chs_count_lim, int speed_mod)
	: path("none"), last_path("")
	, is_active(false)
	, chase_mode(false)
	, speed_mod(speed_mod)
	, proximity(-1), page(-1), index(-1) // -1 �������� ���������� � ������� ����������
	, counter(0), counter_limit(count_lim)
	, chase_counter(0), chase_counter_limit(chs_count_lim)
	{}
};

class SpecialFilemanager : public FilemanagerBase
{
	// ����������
	
	int collected_files;
	
	std::vector<DirectoryData> generated_files;
	ControlCode current_control_code;

	// entity
	EntityData entity;

	// ������ � ������� ������� ������������ � ����������
	DirNavData nav_data;

	// ������������ ���������� ������ ����������
	bool action_allowed;

	// �������� �� ���������� ���������� �������
	bool graphics_update_required;

	// ���������� � ����� �����������
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


	// ���������� ����������
	void ReloadDriveDirs();
	void ReloadDirs();


	// ��������������
	void PreviousDir();
	void CoutPageString();
	void CollectFile();

	void EnterDir();
	void PageLeft();
	void PageRight();



	// ���� 
	void DirMenu();
	void DriveMenu();

	void Initialize();
	void Start();
	
	void CoutEntInfo();
	void CoutInfo();
	void Update();
public:

	// �����������
	SpecialFilemanager();

	// ������ ���������
	bool Run();
};