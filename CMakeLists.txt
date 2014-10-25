project(OpenIsles)

cmake_minimum_required(VERSION 2.8)

add_definitions("-std=c++11 -Wall -Werror -pedantic")

# Debuginfos für Debug-Buildconfig aktivieren
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -DDEBUG_A_STAR -DDEBUG_ECONOMICS")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

# TODO pkg-config funktioniert unter Windows nicht
include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image>=2.0.0)
pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer>=2.0.0)
pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf>=2.0.0)

set(SOURCE_FILES
    src/cpp/config/BuildingConfigMgr.cpp
    src/cpp/economics/EconomicsMgr.cpp
    src/cpp/economics/Carrier.cpp
	src/cpp/game/Colony.cpp
	src/cpp/game/Game.cpp
	src/cpp/game/GameIO.cpp
	src/cpp/game/Player.cpp
	src/cpp/graphics/Animation.cpp
	src/cpp/graphics/Graphic.cpp
    src/cpp/graphics/GraphicsMgr.cpp
    src/cpp/graphics/MapObjectGraphic.cpp
    src/cpp/graphics/PlainGraphic.cpp
	src/cpp/gui/FontMgr.cpp
	src/cpp/gui/GuiAddBuildingWidget.cpp
	src/cpp/gui/GuiButton.cpp
	src/cpp/gui/GuiColonyGoodsWidget.cpp
	src/cpp/gui/GuiMgr.cpp
	src/cpp/gui/GuiPushButton.cpp
	src/cpp/gui/GuiSelectedBuildingWidget.cpp
	src/cpp/gui/GuiStaticElement.cpp
	src/cpp/gui/GuiUtils.cpp
	src/cpp/map/Building.cpp
	src/cpp/map/Isle.cpp
	src/cpp/map/Map.cpp
	src/cpp/map/MapUtils.cpp
	src/cpp/pathfinding/AStar.cpp
	src/cpp/sound/SoundMgr.cpp
	src/cpp/main.cpp
)

add_executable(OpenIsles ${SOURCE_FILES})
include_directories(${CMAKE_SOURCE_DIR}/src/cpp)

exec_program(sdl2-config ARGS "--cflags" OUTPUT_VARIABLE SDL_CFLAGS)
add_definitions(${SDL_CFLAGS})

target_link_libraries(OpenIsles ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${SDL2_TTF_LIBRARIES})

file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})

# Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile)
	add_custom_target(
		generate-doxygen
		rm -rf ${CMAKE_SOURCE_DIR}/doxygen && ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
		VERBATIM
	)
endif(DOXYGEN_FOUND)
