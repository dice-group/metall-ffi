cmake_minimum_required(VERSION 3.22)
project(metall-ffi VERSION 0.2.1)

include(cmake/boilerplate_init.cmake)
boilerplate_init()

OPTION(USE_CONAN "If available, use conan to retrieve dependencies." ON)
if (IS_TOP_LEVEL AND USE_CONAN)
    include(cmake/conan_cmake.cmake)
    if (PROJECT_IS_TOP_LEVEL AND BUILD_TESTING)
        set(CONAN_HYPERTRIE_WITH_TEST_DEPS "True")
    else()
        set(CONAN_HYPERTRIE_WITH_TEST_DEPS "False")
    endif()
    set(CONAN_OPTIONS "with_test_deps=${CONAN_HYPERTRIE_WITH_TEST_DEPS}")
    install_packages_via_conan("${CMAKE_SOURCE_DIR}/conanfile.py" "${CONAN_OPTIONS};boost:header_only=True")
endif ()

find_package(Metall REQUIRED)

add_library(metall-ffi
        src/dice/ffi/metall.cpp
)

add_library(metall-ffi::metall-ffi ALIAS metall-ffi)

target_include_directories(
        metall-ffi
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
)

target_link_libraries(metall-ffi
        PUBLIC
        Metall::Metall
)

set_target_properties(metall-ffi PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)

if (IS_TOP_LEVEL)
    include(cmake/install_library.cmake)
    install_cpp_library(metall-ffi src)
endif ()

if (BUILD_TESTING AND IS_TOP_LEVEL)
    message("Tests are configured to be build.")
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif ()
