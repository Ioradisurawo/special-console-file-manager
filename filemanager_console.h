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

	// // // ��������������

	virtual void	SpecialCodeCheck(const ControlCode& code);
	void			MenuAction();
	void			Shutdown();



	// ���������� ����������
	void ReloadDriveDirs();
	void ReloadDirs();


	
	// �������� (�����)
	void CalculateSize();

	// �������� (����������)
	void DirCreate();
	void FileCreate();
	void EntryCreateMenu();
	void EntryPaste();

	void SearchResultMenu();
	void RecursiveQuerySearch(const std::filesystem::path path, const std::string query);
	void FolderSearch();

	// �������� (����)
	void EntryRename();
	void EntryCopy();
	void EntryMove();
	void EntryDelete();



	// ����������� �� �����������
	void PreviousDir();
	void SelectEntry();
	void PageLeft();
	void PageRight();



	// ���� 
	void EntryMenu();
	void DirMenu();
	void DriveMenu();
public:

	// �����������
	ConsoleFilemanager();

	// ������ ���������
	bool Run();
};