#include "main.hpp"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>

#include "flags.hpp"
#include "utils.hpp"
#include "tui.hpp"

namespace fs = std::filesystem;

template <typename T>
using Uptr = std::unique_ptr<T>;

bool try_parse_args(int &argc, char **(&argv));
bool try_add_root_dir(const std::string &path, const bool canIgnore = false);
bool try_iterate_dirs(const std::filesystem::path &path);

auto paths = std::make_unique<std::vector<std::string>>();

int main(int argc, char *argv[])
{
    if (argc == 2 && (!strcmp(argv[1], "-h") 
                      || !strcmp(argv[1], "--help"))) {
        log::print_help();
        return EXIT_FAILURE;
    }

    if (!try_parse_args(argc, argv)) {
        return EXIT_FAILURE;
    }

    std::string path = tui::run();

    if (path.empty()) {
        return EXIT_FAILURE;
    }

    std::cout << path;
    return EXIT_SUCCESS;
}

bool try_parse_args(int &argc, char **(&argv))
{
    if (argc <= 1) {
        log::warning("No arguments provided, adding current directory to index");
        return try_add_root_dir(".");
    }

    size_t i = 1;
    while (i < argc) {
        std::string arg{argv[i]};

        if (flags::flag_hashmap.find(arg) != flags::flag_hashmap.end()) {
            flags::parse_flag(flags::flag_hashmap.at(arg), i, argc, argv);
        } else {
            log::verbose("Adding directory \"%s\" to index", arg.c_str());

            if (!try_add_root_dir(arg, true)) {
                return false;
            }
        }

        i++;
    }

    if (paths->size() == 0) {
        log::warning("No paths provided, adding current directory to index");
        return try_add_root_dir(".");
    }

    return true;
}

bool try_add_root_dir(const std::string &path, const bool canIgnore)
{
    if (!utils::try_push_dir(path, paths.get())) {
        if (canIgnore) {
            log::warning("Failed to add \"%s\" to index, ignoring",
                            path.c_str());
            return true;
        } else {
            log::error("Failed to add \"%s\" to index", path.c_str());
            return false;
        }
    }

    if (!try_iterate_dirs(paths->back())) {
        switch (flags::force_mode) {
            case 1:
                return false;
            case 2:
                return true;
            case 0:
                std::clog << "Something went wrong while iterating through directories. Do you wish to continue? (y/\033[4mn\033[0m)"
                          << std::endl;
                char input = 'n';
                std::cin >> input;
                return (input == 'y' || input == 'Y');
        }
    }

    return true;
}

bool try_iterate_dirs(const fs::path &path)
{
    fs::recursive_directory_iterator itr(path);
    fs::recursive_directory_iterator end;

    while (itr != end) {
        if (itr.depth() > flags::max_depth) {
            std::error_code ec;
            itr.pop(ec);
            if (ec) {
                log::error("Failed to pop recursive_directory_iterator, breaking the loop (%s)",
                              ec.message().c_str());
                return false;
            }

            continue;
        }

        if (fs::is_directory(itr->path())) {
            log::verbose("Adding entry \"%s\"", itr->path().c_str());

            try {
                paths->push_back(itr->path());
            } catch (const fs::filesystem_error &e) {
                log::warning("( %s ) FILESYSTEM: %s\nIgnoring this entry", 
                                itr->path().c_str(), e.what());
            }
        }

        std::error_code ec;
        itr.increment(ec);
        if (ec) {
            log::error("Failed to increment recursive_directory_iterator, breaking the loop (%s)",
                          ec.message().c_str());
            return false;
        }
    }

    return true;
}

