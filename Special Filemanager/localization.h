#pragma once
#include <string>
#include <unordered_map>

#ifndef LOCALIZATION
#define LOCALIZATION
#define lc_folder Localization::lcmap["folder"]
#define lc_file Localization::lcmap["file"]
#define lc_size Localization::lcmap["size"]
#define lc_new_dir_name Localization::lcmap["new_dir_name"]
#define lc_back Localization::lcmap["back"]
#define lc_control_tips Localization::lcmap["control_tips"]
#define lc_show_tips Localization::lcmap["show_tips"]
#define lc_buffer Localization::lcmap["buffer"]
#define lc_empty Localization::lcmap["empty"]
#define lc_last_entry Localization::lcmap["last_entry"]
#define lc_exit Localization::lcmap["exit"]
#define lc_drive_selection Localization::lcmap["drive_selection"]


// For later

#define lc_already_exists Localization::lcmap["new_dir_name"]
#define lc_new_file_name Localization::lcmap["new_file_name"]
#define lc_success Localization::lcmap["success"]
#define lc_error Localization::lcmap["error"]
#define lc_deletion Localization::lcmap["deletion"]
#define lc_create Localization::lcmap["create"]
#define lc_move Localization::lcmap["move"]
#define lc_copy Localization::lcmap["copy"]
#define lc_copied Localization::lcmap["copied"]

#define lc_page Localization::lcmap["page"]
#define lc_search_in Localization::lcmap["search_in"]
#define lc_enter Localization::lcmap["enter"]
#define lc_query Localization::lcmap["query"]
#define lc_new Localization::lcmap["new"]
#define lc_name Localization::lcmap["name"]

#define lc_current Localization::lcmap["current"]

#endif // !LOCALIZATION

namespace Localization
{
	inline std::string LANGUAGE = ""; // TEMPORARY

	inline std::unordered_map<std::string, std::string> lcmap; // localized map

	void UpdateLocale();
};