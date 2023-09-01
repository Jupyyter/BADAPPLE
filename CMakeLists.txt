cmake_minimum_required(VERSION 3.21)
project(CMakeSFMLProject LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

set(OpenCV_DIR E:/opencv/opencv/build)
find_package(OpenCV REQUIRED)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE sfml-graphics sfml-audio ${OpenCV_LIBS})  # Link both SFML and OpenCV
target_compile_features(main PRIVATE cxx_std_17)
if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:main> $<TARGET_FILE_DIR:main> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS main)
