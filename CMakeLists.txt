cmake_minimum_required(VERSION 3.20)
project(myproject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(wxWidgets CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
add_executable(myproject WIN32 ${SOURCES})


target_include_directories(myproject PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(myproject PRIVATE wx::core wx::base cpr::cpr nlohmann_json::nlohmann_json)