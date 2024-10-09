#pragma once

#include <iostream>
#include <list>
#include <memory>
#include <functional>

#include "menuItem_coloured.h"

class Menu
{
	HANDLE& cHandle;
	WORD default_color;

	bool is_vertical;
	int current_item;

	std::vector<std::shared_ptr<MenuItem>> menu_items;
public:
	std::string top_string, // ������� ������
		bottom_string, // ����������� ������
		left_selector, // ����� ����������
		right_selector, // ������ ����������
		left_empty, // ����� ���������� ���������
		right_empty, // ������ ���������� ���������
		string_indent; // ����� ������ ����

	const std::string& SetColor(WORD color);

	// �����������
	Menu
	(
		HANDLE& consoleHandle,
		WORD default_color,
		const std::string& top_string, const std::string& bottom_string,
		const std::string& left_selector, const std::string& right_selector,
		const std::string& left_empty, const std::string& right_empty,
		const std::string& string_indent
	);


	// Is Vertical
	void SetVertical(bool boolean);
	bool IsVertical();

	// Item

	void						NextItem();
	void						PreviousItem();
	void						SetItemIndex(const int& index);

	void						InvokeItem();
	void						AddItem(std::shared_ptr<MenuItem> item);
	std::shared_ptr<MenuItem>	GetItem();
	const int& GetItemIndex();

	// �����

	void						CoutMenu();

	operator std::string();
};