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
	// Обработка нажатой стрелки
	static std::string			ParseArrowKey();

	// Обработка нажатой клавиши
	static std::string			ParseKey();

	// Получение кода действия меню по клавише
	static ControlCode			GetControlActionCode(const std::string& key_str);

	static ControlCode			ParseControlAction()
	{
		// Обработка кода действия меню
		std::string key = MenuController::ParseKey();

		ControlCode code = MenuController::GetControlActionCode(key);

		return code;
	}
};
