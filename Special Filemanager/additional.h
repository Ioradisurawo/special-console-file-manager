#pragma once
#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <random>
#include <ctime>
#include <chrono>

// ������
int	Random(const int& from, const int& to);

// ������
class Timer {
	std::chrono::steady_clock::time_point start, end;
	std::chrono::milliseconds duration;

public:
	// �����������
	Timer()
	: start(std::chrono::steady_clock::now())
	, end()
	, duration()
	{}
	// 
	bool ThresholdReached(const int& threshold_milisec)
	{
		end = std::chrono::steady_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		if (duration.count() > threshold_milisec)
		{
			start = std::chrono::steady_clock::now();
			return true;
		}
		return false;
	}
};

// ����� �����
bool CopyFileToString(const std::filesystem::path& file_path, std::string& string);
bool CopyStringToFile(const std::filesystem::path& file_path, std::string& string);


// ����� ����� � ������ ������
size_t FindWordInLine(const std::string& string, const std::string& query);
// ����� ����� � ������ ������ � �������������� ������ n ������
size_t FindWordInLine(const std::string& string, const std::string& query, const int& ingored_line_index);
// ������������� ������� ������ � ������ �������
std::string StringToLower(const std::string& string);


// filesystem
std::filesystem::path operator+(std::filesystem::path left, const std::string& right);
// ������ ����������
const uintmax_t DirectorySize(const std::filesystem::path dir_path);
// ����� � ����� (������ �������)
std::string BytesToString(const uintmax_t& bytes, unsigned short kept_floats);