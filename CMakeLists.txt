project(OpenIsles)

cmake_minimum_required(VERSION 2.8)

add_definitions("-std=c++11 -Wall -Werror -pedantic")

# TODO pkg-config funktioniert unter Windows nicht
include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image>=2.0.0)
pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer>=2.0.0)
pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf>=2.0.0)

set(SOURCE_FILES
    src/cpp/config/BuildingConfigMgr.cpp
	src/cpp/game/Colony.cpp
	src/cpp/game/Game.cpp
	src/cpp/game/Player.cpp
	src/cpp/graphics/Animation.cpp
	src/cpp/graphics/Graphic.cpp
    src/cpp/graphics/GraphicsMgr.cpp
	src/cpp/gui/FontMgr.cpp
	src/cpp/gui/GuiAddBuildingWidget.cpp
	src/cpp/gui/GuiButton.cpp
	src/cpp/gui/GuiMgr.cpp
	src/cpp/gui/GuiPushButton.cpp
	src/cpp/gui/GuiStaticElement.cpp
	src/cpp/map/Building.cpp
	src/cpp/map/Isle.cpp
	src/cpp/map/Map.cpp
	src/cpp/map/MapUtils.cpp
	src/cpp/sound/SoundMgr.cpp
	src/cpp/main.cpp
)

add_executable(OpenIsles ${SOURCE_FILES})
include_directories(${CMAKE_SOURCE_DIR}/src/cpp)

exec_program(sdl2-config ARGS "--cflags" OUTPUT_VARIABLE SDL_CFLAGS)
add_definitions(${SDL_CFLAGS})

target_link_libraries(OpenIsles ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${SDL2_TTF_LIBRARIES})

file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})