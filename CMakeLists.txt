cmake_minimum_required(VERSION 3.0)
project(wfdb_wrapper)                 # create a project with the given name
# https://gernotklingler.com/blog/creating-using-shared-libraries-different-compilers-different-operating-systems/

add_executable(read_ann 
    wfdblib/annot.c
    wfdblib/calib.c
    wfdblib/signal.c
    wfdblib/sortann.c
    wfdblib/wfdbio.c
    wfdblib/wfdbinit.c
    read_ann.cpp)

add_executable(read_samples 
    wfdblib/annot.c
    wfdblib/calib.c
    wfdblib/signal.c
    wfdblib/sortann.c
    wfdblib/wfdbio.c
    wfdblib/wfdbinit.c
    read_samples.cpp)

add_executable(read_header
    wfdblib/annot.c
    wfdblib/calib.c
    wfdblib/signal.c
    wfdblib/sortann.c
    wfdblib/wfdbio.c
    wfdblib/wfdbinit.c
    read_header.cpp)

# ВСЕ ЧТО НИЖЕ - ПРИМЕР ДИНАМИЧЕСКОЙ БИБЛИОТЕКИ ИЗ ПРЕДЫДУЩЕЙ ПОПЫТКИ
# add_library(cphysiofile SHARED 
#     wfdblib/annot.c
#     wfdblib/calib.c
#     wfdblib/signal.c
#     wfdblib/sortann.c
#     wfdblib/wfdbio.c
#     wfdblib/wfdbinit.c
#     cphysiofile.cpp)   # compile "shared.cpp" and create a SHARED library called "shared"
# add_executable(main main.cpp)           # compile "main.cpp" the the executable called "main"
# include (GenerateExportHeader) 
# generate_export_header(cphysiofile           # generates the export header shared_EXPORTS.h automatically
#     BASE_NAME cphysiofile
#     EXPORT_MACRO_NAME CPHYSIOFILE_EXPORT
#     EXPORT_FILE_NAME cphysiofile_EXPORTS.h
#     STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
# target_include_directories(cphysiofile PUBLIC ${CMAKE_CURRENT_BINARY_DIR}) # <-- инклуд только для одного таргета (cphysiofile)
# target_link_libraries(main cphysiofile)      # link our previously created shared library "shared" to the exectable "main"

