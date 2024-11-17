#pragma once

#include <iostream>
#include <conio.h>
#include <vector>

enum class ControlCode
{
	NONE,
	CONFIRM,
	DECLINE,
	UP,
	DOWN,
	LEFT,
	RIGHT,

	ACTION,
	ADDITIONAL_1
};

class MenuController
{
public:
	// ��������� ������� �������
	static std::string			ParseArrowKey();

	// ��������� ������� �������
	static std::string			ParseKey();

	// ��������� ���� �������� ���� �� �������
	static ControlCode			GetControlActionCode(const std::string& key_str);

	static ControlCode			ParseControlAction()
	{
		// ��������� ���� �������� ����
		std::string key = MenuController::ParseKey();

		ControlCode code = MenuController::GetControlActionCode(key);

		return code;
	}
};
