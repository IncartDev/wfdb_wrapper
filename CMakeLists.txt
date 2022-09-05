cmake_minimum_required(VERSION 3.0)
project(sharedLibsDemo)                 # create a project with the given name
# https://gernotklingler.com/blog/creating-using-shared-libraries-different-compilers-different-operating-systems/


add_library(mylibrary SHARED mylibrary.cpp)   # compile "shared.cpp" and create a SHARED library called "shared"

add_executable(main main.cpp)           # compile "main.cpp" the the executable called "main"

include (GenerateExportHeader) 

generate_export_header(mylibrary           # generates the export header shared_EXPORTS.h automatically
    BASE_NAME mylibrary
    EXPORT_MACRO_NAME MYLIBRARY_EXPORT
    EXPORT_FILE_NAME mylibrary_EXPORTS.h
    STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)

# сгенерированный файл mylibrary_EXPORTS.h не видится в инклудах - есть два варианта решения:
# а) генерить не в папке с билдом, а в папке с исходниками:
# set(_export_file "${CMAKE_CURRENT_SOURCE_DIR}/mylibrary_EXPORTS.h")
# generate_export_header("${LIBRARY_NAME}" EXPORT_FILE_NAME "${_export_file}")
# б) инклудить путь с билдом
target_include_directories(mylibrary PUBLIC ${CMAKE_CURRENT_BINARY_DIR}) # <-- инклуд только для одного таргета (mylibrary)
# include_directories(${CMAKE_CURRENT_BINARY_DIR}) # <-- или вот так, для всей папки

target_link_libraries(main mylibrary)      # link our previously created shared library "shared" to the exectable "main"
