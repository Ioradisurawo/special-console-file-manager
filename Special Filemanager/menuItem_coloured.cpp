#include "menuItem_coloured.h"

// Конструктор 
MenuItem::MenuItem(const std::string& text, const WORD& color)
	: text(text)
	, color(color)
{}

// Конструктор 
MenuItem::MenuItem(const std::string& text, const WORD& color, std::function<void()> function)
	: text(text)
	, color(color)
	, func(function)
{}

// Color
const WORD&			MenuItem::GetColor()
{
	return color;
}
void				MenuItem::SetColor(const WORD& color)
{
	this->color = color;
}

// Text
const std::string&	MenuItem::GetText()
{
	return text;
}
void				MenuItem::SetText(std::string text)
{
	this->text = text;
}

// func
void				MenuItem::SetFunc(std::function<void()> function)
{
	func = function;
}
void				MenuItem::InvokeFunction()
{
	if (func)
		func();
}