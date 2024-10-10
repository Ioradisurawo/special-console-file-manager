#pragma once
#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <random>
#include <ctime>



bool CopyFileToString(const std::filesystem::path& file_path, std::string& string);
bool CopyStringToFile(const std::filesystem::path& file_path, std::string& string);


// Поиск слова в линиях строки
size_t FindWordInLine(const std::string& string, const std::string& query);
// Поиск слова в линиях строки с игнорированием каждой n строки
size_t FindWordInLine(const std::string& string, const std::string& query, const int& ingored_line_index);
// Преобразовать символы строки в нижний регистр
std::string StringToLower(const std::string& string);


// filesystem
std::filesystem::path operator+(std::filesystem::path left, const std::string& right);
// размер директории
const uintmax_t DirectorySize(const std::filesystem::path dir_path);
// Байты в текст (слегка хреново)
std::string BytesToString(const uintmax_t& bytes, unsigned short kept_floats);