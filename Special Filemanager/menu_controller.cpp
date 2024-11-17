#include "menu_controller.h"


// ��������� ������� �������
std::string				MenuController::ParseArrowKey()
{
	char arrowkey = _getch();
	switch (arrowkey)
	{
	case 72:
	{
		return "up";
		break;
	}
	case 80:
	{
		return "down";
		break;
	}
	case 75:
	{
		return "left";
		break;
	}
	case 77:
	{
		return "right";
		break;
	}
	}
}

// ��������� ������� �������
std::string				MenuController::ParseKey()
{
	char key = _getch();

	switch (key)
	{
	case 13:
	{
		return "enter";
	}
	case 27:
	{
		return "escape";
	}

	case -32:
		return ParseArrowKey();

	default:
		return std::string(1, key);
	}
}

// ��������� ���� �������� ���� �� �������
ControlCode		MenuController::GetControlActionCode(const std::string& key_str)
{
	if (key_str == "enter" || key_str == "e" || key_str == "�")
		return ControlCode::CONFIRM;

	if (key_str == "escape")
		return ControlCode::DECLINE;

	if (key_str == "w" || key_str == "�" || key_str == "up")
		return ControlCode::UP;

	if (key_str == "a" || key_str == "�" || key_str == "left")
		return ControlCode::LEFT;

	if (key_str == "s" || key_str == "�" || key_str == "down")
		return ControlCode::DOWN;

	if (key_str == "d" || key_str == "�" || key_str == "right")
		return ControlCode::RIGHT;

	if(key_str == "c" || key_str == "�")
		return ControlCode::ACTION;

	if (key_str == "q" || key_str == "�")
		return ControlCode::ADDITIONAL_1;

	return ControlCode::NONE;
}