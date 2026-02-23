set(SourceFiles
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/utils.cpp"
    "${PROJECT_SOURCE_DIR}/src/tui.cpp"
    "${PROJECT_SOURCE_DIR}/src/flags.cpp"
)

add_executable(livefind ${SourceFiles})

target_link_libraries(livefind PRIVATE ncurses)
target_link_libraries(livefind PRIVATE menu)
target_link_libraries(livefind PRIVATE form)

target_include_directories(livefind PRIVATE ../include)
target_compile_features(livefind PRIVATE cxx_std_20)

