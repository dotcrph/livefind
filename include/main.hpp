#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include <ncurses.h>
#include <menu.h>
#include <form.h>

extern std::unique_ptr<std::vector<std::string>> paths;
extern std::string out_path;

extern std::unique_ptr<WINDOW> g_input_w;
extern std::unique_ptr<FORM> g_input_fm;
extern std::unique_ptr<FIELD> g_input_fd[2];

extern std::unique_ptr<WINDOW> g_files_w;
extern std::unique_ptr<MENU> g_files_m;
extern std::unique_ptr<std::vector<ITEM*>> g_files_itms;

bool try_parse_args(int &argc, char **(&argv));
bool try_add_root_dir(const std::string &path, const bool canIgnore = false);
bool try_iterate_dirs(const std::filesystem::path &path);

