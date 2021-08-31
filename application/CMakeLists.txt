#
# Application
#

#
# Library
#

file(GLOB_RECURSE application_headers ${CMAKE_CURRENT_SOURCE_DIR}/source/*.h)
file(GLOB_RECURSE application_sources ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_library(application STATIC ${application_sources} include/asmlgen/application/config/version.h)

set(application_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(application_source_directories ${CMAKE_CURRENT_SOURCE_DIR}/source)

target_include_directories(application PRIVATE ${application_include_directories})
target_include_directories(application PRIVATE ${application_source_directories})

target_compile_features(application INTERFACE cxx_std_20)

set_project_warnings(application)

find_package(SFML REQUIRED)

macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

print_all_variables()

target_include_directories(application PUBLIC ${SFML_INCLUDE_DIRS})
target_link_libraries(application ${SFML_LIBRARIES})

#target_include_directories(application PRIVATE ${glfw_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})
#target_link_libraries(application ${glfw_LIBRARIES} ${Vulkan_LIBRARIES})