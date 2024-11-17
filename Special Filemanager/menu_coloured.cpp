#include "menu_coloured.h"

const std::string& Menu::SetColor(WORD color)
{
	SetConsoleTextAttribute(cHandle, color);
	return "";
}

// Конструктор
Menu::Menu
(
	HANDLE& consoleHandle,
	WORD default_color,
	const std::string& top_string, const std::string& bottom_string,
	const std::string& left_selector, const std::string& right_selector,
	const std::string& left_empty, const std::string& right_empty,
	const std::string& string_indent
)
	: cHandle(consoleHandle),
	default_color(default_color),
	current_item(0),
	is_vertical(true),
	top_string(top_string), bottom_string(bottom_string),
	left_selector(left_selector), right_selector(right_selector),
	left_empty(left_empty), right_empty(right_empty),
	string_indent(string_indent)
{}


// Is Vertical

void Menu::SetVertical(bool boolean)
{
	is_vertical = boolean;
}
bool Menu::IsVertical()
{
	return is_vertical;
}



// Item

void						Menu::NextItem()
{
	current_item = current_item + 1 >= menu_items.size() ? 0 : current_item + 1;
}
void						Menu::PreviousItem()
{
	current_item = current_item - 1 < 0 ? menu_items.size() - 1 : current_item - 1;
}
void						Menu::SetItemIndex(const int& index)
{
	current_item = index;
}

void						Menu::InvokeItem()
{
	menu_items.at(current_item)->InvokeFunction();
}
void						Menu::AddItem(std::shared_ptr<MenuItem> item) { menu_items.push_back(item); }
std::shared_ptr<MenuItem>	Menu::GetItem() { return menu_items.at(current_item); }
const int&					Menu::GetItemIndex() { return current_item; }



// Вывод
void						Menu::CoutMenu()
{
	std::cout << top_string;

	int i = 0;
	for (auto& menu_item : menu_items)
	{
		std::cout << (current_item == i ? left_selector : left_empty)
			<< SetColor(menu_item->GetColor()) << menu_item->GetText() << SetColor(default_color)
			<< (current_item == i ? right_selector : right_empty)
			<< string_indent;
		++i;
	}

	std::cout << bottom_string;
}
Menu::operator std::string()
{
	std::string str;
	str.append(top_string);
	for (int i = 0; i < menu_items.size(); i++)
	{
		str.append(current_item == i ? left_selector : left_empty);
		str.append(menu_items.at(i)->GetText());
		str.append(current_item == i ? right_selector : right_empty);
		str.append(string_indent);
	}
	str.append(bottom_string);
	return str;
}