set(CMAKE_CXX_STANDARD 20)

find_package(doctest REQUIRED)

# add the executable for all tests
add_executable(tests_Sanity tests_Sanity.cpp)
target_link_libraries(tests_Sanity
        doctest::doctest
        metall-ffi::metall-ffi
)
set_property(TARGET tests_Sanity PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Sanity COMMAND tests_Sanity)
