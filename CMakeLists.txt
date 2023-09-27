cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)

# supress deprecated warnings
#add_compile_options(/wd4996)

add_definitions(-D_UNICODE -DUNICODE)


include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG "dfdbe1eecf0451e0da5420c4b9c39ecb415681ac"
  # GIT_TAG v3.0.0
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()



add_executable(
  hello_test
  src/gpad.cpp
)

SET( LIBS XInput DSound gtest_main )
TARGET_LINK_LIBRARIES(hello_test ${LIBS} )

add_executable(
    camdir
    src/llgame-main.cpp
    src/irsdk/irsdk_client.cpp
    src/irsdk/irsdk_diskclient.cpp
    src/irsdk/irsdk_utils.cpp
    src/irsdk/yaml_parser.cpp
    src/components/rendering-comp.cpp
    src/systems/irtelemetry-sys.cpp
    src/systems/replay-director-system.cpp
    src/systems/window-rendering-sys.cpp
    src/systems/timing-tower-sys.cpp
)

TARGET_LINK_LIBRARIES(camdir)

include(GoogleTest)
gtest_discover_tests(hello_test)
