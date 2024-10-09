#include "additional_funcs.h"


bool			CopyFileToString(const std::filesystem::path& file_path, std::string& string)
{
	std::ifstream file;
	file.open(file_path);
	if (!file.is_open())
	{
		return false;
	}
	std::getline(file, string, '\0');
	file.close();

	return true;
}
bool			CopyStringToFile(const std::filesystem::path& file_path, std::string& string)
{
	std::ofstream file;
	file.open(file_path);
	if (!file.is_open())
	{
		return false;
	}
	file << string;
	file.close();

	return true;
}

// Поиск слова в линиях строки
size_t			FindWordInLine(const std::string& string, const std::string& query)
{
	size_t pos = 0;
	std::istringstream iss(string);
	std::string line_str;
	while (std::getline(iss, line_str))
	{
		if (line_str == query)
		{
			return pos;
		}
		pos += line_str.length() + 1;
	}
	return std::string::npos;
}
// Поиск слова в линиях строки с игнорированием каждой n строки
size_t			FindWordInLine(const std::string& string, const std::string& query, const int& ingored_line_index)
{
	int current_line = 0;
	size_t pos = 0;
	std::istringstream iss(string);
	std::string line_str;
	while (std::getline(iss, line_str))
	{
		if (line_str == query && current_line % ingored_line_index == 0)
		{
			return pos;
		}
		pos += line_str.length() + 1;
		++current_line;
	}
	return std::string::npos;
}
// Преобразовать символы строки в нижний регистр
std::string		StringToLower(const std::string& string)
{
	std::stringstream result;
	for (auto& ch : string)
	{
		result << char(std::tolower(ch));
	}
	return result.str();
}


// filesystem
std::filesystem::path operator+(std::filesystem::path left, const std::string& right)
{
	return left.concat(right);
}
// размер директории
const uintmax_t DirectorySize(const std::filesystem::path dir_path)
{
	uintmax_t bytes = 0;
	for (auto& entry : std::filesystem::directory_iterator(dir_path
		// Важная штука
		, std::filesystem::directory_options::skip_permission_denied))
	{
		bytes += std::filesystem::file_size(entry);
		if (entry.is_directory())
			bytes += DirectorySize(entry);
	}
	return bytes;
}
// Байты в текст (слегка хреново)
std::string		BytesToString(const uintmax_t& bytes, unsigned short kept_floats)
{
	int pos = 0;
	double result_size = bytes;
	std::vector<std::string> si{ " Байт", " КБ", " МБ", " ГБ", " ТБ" };

	for (; result_size > 1024; pos++)
	{
		result_size /= 1024;
	}

	std::string result = std::to_string(kept_floats == 0 ? int(result_size) : result_size);
	if (kept_floats > 0)
	{
		result = result.substr(0, result.find('.') + 1 + kept_floats);
	}

	return result + si.at(pos);
}