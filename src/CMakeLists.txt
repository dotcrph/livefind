set(SourceFiles
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/utils.cpp"
    "${PROJECT_SOURCE_DIR}/src/tui.cpp"
    "${PROJECT_SOURCE_DIR}/src/flags.cpp"
)

add_executable(livefind ${SourceFiles})

target_include_directories(livefind PRIVATE ../include)
target_compile_features(livefind PRIVATE cxx_std_20)

