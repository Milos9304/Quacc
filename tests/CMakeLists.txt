enable_testing()

#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIRS})

add_library(libquest SHARED IMPORTED)
set_property(TARGET libquest PROPERTY IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/lib/libQuEST.so)

add_executable(gateTest gateTest.cpp)
target_link_libraries(gateTest PRIVATE xacc::xacc xacc::quantum_gate gtest libquest)

add_executable(expectationsTest expectationsTest.cpp)
target_link_libraries(expectationsTest PRIVATE xacc::xacc xacc::quantum_gate gtest libquest)


#target_include_directories(gateTest PRIVATE ${GTEST_INCLUDE_DIRS})
#target_link_libraries(gateTest PRIVATE xacc::xacc xacc::quantum_gate ${GTEST_LIBRARIES} gtest libquest)
#target_compile_features(gateTest PRIVATE cxx_std_14)

add_test(NAME gateTest COMMAND gateTest)
add_test(NAME expectationsTest COMMAND expectationsTest)
 