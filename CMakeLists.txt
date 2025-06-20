cmake_minimum_required(VERSION 3.10)
project(dcz)
enable_language(CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


include(FetchContent)

FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG main
)
FetchContent_MakeAvailable(CLI11)

include_directories(${CLI11_SOURCE_DIR}/include)

option(COMPILER "Support compile dcz file" OFF)

file(GLOB AST "AST/*.cpp")
file(GLOB ERROR_HANDLER "ErrorHandler/*.cpp")
file(GLOB RUN_AST "RunAST/RunAST.cpp")

configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_SOURCE_DIR}/include/config.h"
)


set(AST2MC "")
if (${COMPILER})
    message("-- Adding compiler support.")
    file(GLOB AST2MC "AST2MC/*.cpp" "AST2MC/codegen/*.cpp")
endif()

add_executable(dcz ${AST}  ${ERROR_HANDLER} ${AST2MC} ${RUN_AST} main.cpp)

target_include_directories(dcz PUBLIC "include")
