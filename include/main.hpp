#pragma once

#include <filesystem>
#include <string>

bool tryParseArgs(int argc, char **argv);
/// Description:
///     Parses cli arguments and, if arg is a flag, passes it to 
///     flags::parseFlag, else treats this arg as a directory and 
///     passes it to tryCacheDirectoryTree.
/// 
/// Returns:
///     True if we can can proceed to the TUI file explorer screen, else 
///     false (i.e. if some file is bad or if the user used --help)

bool tryCacheDirectoryTree(const std::string &path, const bool proceedOnError);
/// Description:
///     First checks if path is ok and pushes it to paths vector, then passes 
///     path to tryIterateDirs.
/// 
/// Returns:
///     True in any of these cases:
///         1) Everything goes fine
///         2) Something is wrong with path and proceedOnError == true
///         3) tryIterateDirs fails (returns false) and flags::forceMode == 2
///         4) tryIterateDirs fails and flags::forceMode == 0 and user answers 
///            'y' or 'Y' to the prompt
///
///     False in any of these cases:
///         2) Something is wrong with path and proceedOnError == false
///         3) tryIterateDirs fails (returns false) and flags::forceMode == 1
///         4) tryIterateDirs fails and flags::forceMode == 0 and user answers 
///            ANYTHING BUT 'y' or 'Y' to the prompt

bool tryIterateDirs(const std::filesystem::path &path);
/// Description:
///     Iterates through all subpaths of a given path using 
///     std::filesystem::recursive_directory_iterator, ignoring all paths that 
///     are deeper that flags::maxDepth. Pushes only those paths that return 
///     true when passed to isPathOK().
/// 
/// Returns:
///     True if no OS errors happen, else false

