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
	std::string top_string, // Верхняя строка
		bottom_string, // Закрывающая строка
		left_selector, // Левый выделитель
		right_selector, // Правый выделитель
		left_empty, // Левое отсутствие выделения
		right_empty, // Правое отсутствие выделения
		string_indent; // После пункта меню

	const std::string& SetColor(WORD color);

	// Конструктор
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

	// Вывод

	void						CoutMenu();

	operator std::string();
};