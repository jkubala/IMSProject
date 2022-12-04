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

add_executable(
        IMSProject
        src/main.cpp
        src/text.hpp
        src/text.cpp
        src/hexagonField.hpp
        src/hexagonField.cpp
        src/battle.cpp
        src/battle.hpp)

add_executable(
        battle_test
        src/test/battle_test.cpp
        src/battle.cpp
        src/battle.hpp
        )
target_link_libraries(IMSProject PRIVATE HexagonGUI Window SDL2main SDL2 SDL2_image SDL2_ttf pthread)