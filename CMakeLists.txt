cmake_minimum_required(VERSION 3.31)
project(YT-DLCOMP)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(wxWidgets CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
add_executable(YT-DLCOMP WIN32 ${SOURCES})


target_include_directories(YT-DLCOMP PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(YT-DLCOMP PRIVATE wx::core wx::base cpr::cpr nlohmann_json::nlohmann_json)



# Install the executable
install(TARGETS YT-DLCOMP
    RUNTIME_DEPENDENCIES
        PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
        POST_EXCLUDE_REGEXES ".*system32/.*"
    RUNTIME DESTINATION .
)

# CPack configuration
set(CPACK_PACKAGE_NAME "YT-DLCOMP")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_GENERATOR "ZIP")

include(CPack)