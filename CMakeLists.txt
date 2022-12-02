cmake_minimum_required(VERSION 3.0.0)
project(IMSProject VERSION 0.1.0)

add_library(
    Window
    src/window.hpp
    src/window.cpp
)

add_library(
    HexagonGUI
    src/hexagonGUI.hpp
    src/hexagonGUI.cpp
)

add_executable(IMSProject src/main.cpp src/window.hpp src/window.cpp src/hexagonGUI.hpp src/hexagonGUI.cpp src/text.hpp src/text.cpp src/hexagonField.hpp src/hexagonField.cpp)


target_link_libraries(IMSProject PRIVATE Window SDL2main SDL2 SDL2_image SDL2_ttf)