cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(third_party)

include(ExternalProject)

set(SPDLOG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/spdlog)
set(JSON_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/json)

ExternalProject_Add(
        spdlog
        PREFIX ${SPDLOG_ROOT}
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG master
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        SOURCE_DIR ${SPDLOG_ROOT}/src/spdlog
        INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/spdlog
        CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/spdlog
        -DSPDLOG_BUILD_SHARED:BOOL=OFF
        -DSPDLOG_BUILD_TESTS:BOOL=OFF
        -DSPDLOG_BUILD_TESTS_HO:BOOL=OFF
        -DSPDLOG_BUILD_BENCH:BOOL=OFF
        -DSPDLOG_BUILD_TESTING:BOOL=OFF
        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
)

ExternalProject_Add(
        json
        PREFIX ${JSON_ROOT}
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG master
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        SOURCE_DIR ${JSON_ROOT}/src/json
        INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/json
        CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/json
        -DJSON_BuildTests:BOOL=OFF
)