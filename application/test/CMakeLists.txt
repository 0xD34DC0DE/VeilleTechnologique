find_package(GTest REQUIRED)

function(add_application_test subdirectory)
    message(STATUS "Adding Test: " ${subdirectory})

    file(GLOB_RECURSE ${subdirectory}_test_headers ${CMAKE_CURRENT_SOURCE_DIR}/${subdirectory}/*.h)
    file(GLOB_RECURSE ${subdirectory}_test_sources ${CMAKE_CURRENT_SOURCE_DIR}/${subdirectory}/*.cpp)

    add_executable(${subdirectory}_test ${${subdirectory}_test_headers} ${${subdirectory}_test_sources})

    target_include_directories(${subdirectory}_test PRIVATE ${application_include_directories} ${GTest_INCLUDE_DIRS})
    target_link_libraries(${subdirectory}_test PRIVATE application ${GTest_LIBRARIES})

    add_test(NAME ${subdirectory}_test COMMAND ${subdirectory}_test)
endfunction()

#
# Tests
#

add_application_test(dataset)