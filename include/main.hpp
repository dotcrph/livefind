#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include <ncurses.h>
#include <menu.h>
#include <form.h>

extern std::unique_ptr<std::vector<std::string>> paths;

bool try_parse_args(int &argc, char **(&argv));
bool try_add_root_dir(const std::string &path, const bool canIgnore = false);
bool try_iterate_dirs(const std::filesystem::path &path);

