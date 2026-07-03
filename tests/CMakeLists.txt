project(CWire_test)
add_subdirectory(gtest_lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(GTest MultidelegateTest.cpp DelegateTest.cpp)

target_link_libraries(GTest CWire)

target_link_libraries(GTest gtest gtest_main)