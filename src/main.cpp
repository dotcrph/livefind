#include "main.hpp"

#include <cstdlib>
#include <cstring>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>

#include "flags.hpp"
#include "utils.hpp"
#include "tui.hpp"

namespace fs = std::filesystem;

std::vector<std::string> paths;

int main(int argc, char *argv[])
{
    if (!tryParseArgs(argc, argv))
        return EXIT_FAILURE;

    std::string path = tui::run(std::move(paths));

    if (path.empty())
        return EXIT_FAILURE;

    std::cout << path;
    return EXIT_SUCCESS;
}

bool tryParseArgs(int argc, char **argv)
{
    std::vector<std::string> args = conversions::convertArgs(argc, argv);

    if (args.size() <= 1) {
        log::warning("No arguments provided, adding current directory to index");
        return tryCacheDirectoryTree(".", false);
    }

    if (args.size() == 2 && (args[1] == "-h" || args[1] == "--help")) {
        log::printHelp();
        return false;
    }

    for (size_t i = 1; i < args.size(); i++) {
        if (flags::tryParseAsFlag(i, args))
            continue;

        const auto &arg = args[i];

        log::verbose("Adding directory '%s' to index", arg.c_str());

        if (!tryCacheDirectoryTree(arg, true))
            return false;
    }

    return true;
}

bool tryCacheDirectoryTree(const std::string &path, const bool proceedOnError)
{
    if (!utils::isPathOK(path)) {
        log::warning("'%s' is not a valid directory", path.c_str());

        if (proceedOnError) {
            log::warning(
                "Failed to add \"%s\" to index, ignoring", 
                path.c_str()
            );
        } else {
            log::error(
                "Failed to add \"%s\" to index", 
                path.c_str()
            );
        }

        return proceedOnError;
    }

    paths.push_back(path);

    if (tryIterateDirs(path)) 
        return true;

    // See flags::parseForceMode::forceModeArgs
    if (flags::forceMode == 1) return false;
    if (flags::forceMode == 2) return true;

    std::clog << "Something went wrong while iterating through directories. Do you wish to continue? (y/\033[4mn\033[0m)"
              << std::endl;

    // Reading this as string just in case
    std::string input;
    std::getline(std::cin, input);

    utils::strTrimWhitespace(input);
    utils::strToLower(input);

    return (input == "y" || input == "yes");
}

bool tryIterateDirs(const fs::path &path)
{
    fs::recursive_directory_iterator iterator(path);
    fs::recursive_directory_iterator end; // Default constructed
    std::error_code ec;

    while (iterator != end) {
        if (iterator.depth() > flags::maxDepth) {
            iterator.pop(ec);

            if (ec) {
                log::error(
                    "Failed to pop recursive_directory_iterator, breaking the loop (%s)",
                    ec.message().c_str()
                );

                return false;
            }

            continue;
        }

        const fs::path &path = iterator->path();
        std::string pathStr = path.string();

        if (utils::isPathOK(path))
            paths.push_back(pathStr);

        iterator.increment(ec);

        if (ec) {
            log::error(
                "Failed to increment recursive_directory_iterator, breaking the loop (%s)",
                ec.message().c_str()
            );

            return false;
        }
    }

    return true;
}

