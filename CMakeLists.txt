project(OpenIsles)
#set(CMAKE_VERBOSE_MAKEFILE on)

cmake_minimum_required(VERSION 2.8.12) #  target_compile_options() gibts erst seit v2.8.12


## Quelldateien ########################################################################################################

set(COMMON_SOURCE_FILES
    src/cpp/config/BuildingCosts.h
    src/cpp/config/ConfigMgr.cpp
    src/cpp/config/ConfigMgr.h
    src/cpp/config/Good.h
    src/cpp/economics/EconomicsMgr.cpp
    src/cpp/economics/EconomicsMgr.h
    src/cpp/economics/Carrier.cpp
    src/cpp/economics/Carrier.h
    src/cpp/game/Colony.cpp
    src/cpp/game/Colony.h
    src/cpp/game/Game.cpp
    src/cpp/game/Game.h
    src/cpp/game/GameIO.cpp
    src/cpp/game/GameIO.h
    src/cpp/game/GoodsSlot.h
    src/cpp/game/Player.cpp
    src/cpp/game/Player.h
    src/cpp/game/ProductionSlots.h
    src/cpp/graphics/graphic/Animation.h
    src/cpp/graphics/graphic/GraphicSet.h
    src/cpp/graphics/graphic/IGraphic.h
    src/cpp/graphics/mgr/AbstractGraphicsMgr.cpp
    src/cpp/graphics/mgr/AbstractGraphicsMgr.h
    src/cpp/graphics/mgr/IFontMgr.h
    src/cpp/graphics/mgr/IGraphicsMgr.h
    src/cpp/graphics/renderer/IRenderer.h
    src/cpp/map/coords/DrawCoords.h
    src/cpp/map/coords/MapCoords.cpp
    src/cpp/map/coords/MapCoords.h
    src/cpp/map/coords/ScreenCoords.cpp
    src/cpp/map/coords/ScreenCoords.h
    src/cpp/map/coords/WindowCoords.h
    src/cpp/map/coords/XYCoords.h
    src/cpp/map/Building.cpp
    src/cpp/map/Building.h
    src/cpp/map/Direction.cpp
    src/cpp/map/Direction.h
    src/cpp/map/Harvestable.cpp
    src/cpp/map/Harvestable.h
    src/cpp/map/Isle.cpp
    src/cpp/map/Isle.h
    src/cpp/map/Map.cpp
    src/cpp/map/Map.h
    src/cpp/map/MapCoordUtils.cpp
    src/cpp/map/MapCoordUtils.h
    src/cpp/map/MapObject.h
    src/cpp/map/MapObjectType.h
    src/cpp/map/Structure.h
    src/cpp/pathfinding/AStar.cpp
    src/cpp/pathfinding/AStar.h
    src/cpp/rapidxml/rapidxml.hpp
    src/cpp/rapidxml/rapidxml_iterators.hpp
    src/cpp/rapidxml/rapidxml_print.hpp
    src/cpp/rapidxml/rapidxml_utils.hpp
    src/cpp/sound/ISoundMgr.h
    src/cpp/utils/Color.h
    src/cpp/utils/Consts.cpp
    src/cpp/utils/Consts.h
    src/cpp/utils/Events.h
    src/cpp/utils/MapCoordsIterator.h
    src/cpp/utils/Rect.h
    src/cpp/utils/RectangleData.h
    src/cpp/utils/PerformanceCounter.h
    src/cpp/utils/StringFormat.cpp
    src/cpp/utils/StringFormat.h
    src/cpp/utils/Ticks.cpp
    src/cpp/utils/Ticks.h
    src/cpp/Context.h
    src/cpp/defines.h
)
set(GUI_SOURCE_FILES
    src/cpp/gui/components/GuiAddBuildingWidget.cpp
    src/cpp/gui/components/GuiAddBuildingWidget.h
    src/cpp/gui/components/GuiBase.h
    src/cpp/gui/components/GuiButton.cpp
    src/cpp/gui/components/GuiButton.h
    src/cpp/gui/components/GuiMap.cpp
    src/cpp/gui/components/GuiMap.h
    src/cpp/gui/components/GuiMinimap.cpp
    src/cpp/gui/components/GuiMinimap.h
    src/cpp/gui/components/GuiPushButton.cpp
    src/cpp/gui/components/GuiPushButton.h
    src/cpp/gui/components/GuiResourcesBar.cpp
    src/cpp/gui/components/GuiResourcesBar.h
    src/cpp/gui/components/GuiStaticElement.cpp
    src/cpp/gui/components/GuiStaticElement.h
    src/cpp/gui/panel-widgets/GuiSelectedBuildingWidget.cpp
    src/cpp/gui/panel-widgets/GuiSelectedBuildingWidget.h
    src/cpp/gui/panel-widgets/GuiBuildMenuWidget.cpp
    src/cpp/gui/panel-widgets/GuiBuildMenuWidget.h
    src/cpp/gui/panel-widgets/GuiColonyGoodsWidget.cpp
    src/cpp/gui/panel-widgets/GuiColonyGoodsWidget.h
    src/cpp/gui/panel-widgets/GuiDummyWidget.cpp
    src/cpp/gui/panel-widgets/GuiDummyWidget.h
    src/cpp/gui/panel-widgets/GuiOptionsMenuWidget.cpp
    src/cpp/gui/panel-widgets/GuiOptionsMenuWidget.h
    src/cpp/gui/GuiMgr.cpp
    src/cpp/gui/GuiMgr.h
    src/cpp/gui/Identifiers.h
)
set(GRAPHICS_SOURCE_FILES
    src/cpp/graphics/mgr/sdl/SDLFontMgr.cpp
    src/cpp/graphics/mgr/sdl/SDLFontMgr.h
    src/cpp/graphics/mgr/sdl/SDLGraphicsMgr.cpp
    src/cpp/graphics/mgr/sdl/SDLGraphicsMgr.h
    src/cpp/graphics/graphic/sdl/SDLGraphic.cpp
    src/cpp/graphics/graphic/sdl/SDLGraphic.h
    src/cpp/graphics/renderer/sdl/SDLRenderer.cpp
    src/cpp/graphics/renderer/sdl/SDLRenderer.h
)
set(SOUND_SOURCE_FILES
    src/cpp/sound/sdl/SDLSoundMgr.cpp
    src/cpp/sound/sdl/SDLSoundMgr.h
)
set(NOSDL_FILES
    src/cpp/graphics/graphic/nosdl/NoSDLGraphic.h
    src/cpp/graphics/mgr/nosdl/NoSDLFontMgr.h
    src/cpp/graphics/mgr/nosdl/NoSDLGraphicsMgr.h
    src/cpp/graphics/renderer/nosdl/NoSDLRenderer.h
)
set(TEST_FILES
    test/cpp/GameTest.cpp
    test/cpp/GameTest.h
    test/cpp/config/ConfigMgrTest.cpp
    test/cpp/economics/EconomicsMgrTest.cpp
    test/cpp/graphics/GraphicSetTest.cpp
    test/cpp/map/coords/MapCoordsTest.cpp
    test/cpp/map/BuildingTest.cpp
    test/cpp/map/DirectionTest.cpp
    test/cpp/map/HarvestableTest.cpp
    test/cpp/map/MapCoordUtilsTest.cpp
    test/cpp/utils/MapCoordsIteratorTest.cpp
)


## Einstellungen für alle Targets ######################################################################################

add_definitions("-std=c++11 -Wall -Werror -pedantic")
include_directories(${CMAKE_SOURCE_DIR}/src/cpp)

# Debuginfos für Debug-Buildconfig aktivieren - nicht vergessen, neue Variablen in die Doxyfile.in einzutragen
# Weitere mögliche Debug-Variablen, die nicht gesetzt sind: DEBUG_GUIMAP
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -DDEBUG_A_STAR -DDEBUG_ECONOMICS")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")


## Target: OpenIsles - das Spiel selber ################################################################################

add_executable(OpenIsles
    src/cpp/main.cpp
    ${COMMON_SOURCE_FILES}
    ${GUI_SOURCE_FILES}
    ${GRAPHICS_SOURCE_FILES}
    ${SOUND_SOURCE_FILES}
)

# Windows mit MinGW
if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWINDOWS")

    # statisch linken, dann sind die MinGW-Runtimes schon in der EXE drin
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

    # SDL2: pkg-config funktioniert unter Windows nicht, drum benutzen wir unser eigenen CMake-Script
    set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image 2.0 REQUIRED)
    find_package(SDL2_mixer 2.0 REQUIRED)
    find_package(SDL2_ttf 2.0 REQUIRED)

    # sdl2-config gibts auch nicht. also konfigurieren wir die Include-Verzeichnisse für den Compiler manuell
    target_include_directories(OpenIsles PUBLIC "${SDL2_INCLUDE_DIR}")
    target_include_directories(OpenIsles PUBLIC "${SDL2_IMAGE_INCLUDE_DIR}")
    target_include_directories(OpenIsles PUBLIC "${SDL2_MIXER_INCLUDE_DIR}")
    target_include_directories(OpenIsles PUBLIC "${SDL2_TTF_INCLUDE_DIR}")

# Linux
else(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLINUX")

    # SDL2: pkg-config und sdl2-config unter Linux nutzen
    include(FindPkgConfig)
    pkg_search_module(SDL2 REQUIRED sdl2)
    pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image>=2.0.0)
    pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer>=2.0.0)
    pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf>=2.0.0)

    # sdl2-config für die Include-Verzeichnisse
    execute_process(COMMAND "sdl2-config" "--cflags" OUTPUT_VARIABLE SDL2_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
    separate_arguments(SDL2_CFLAGS UNIX_COMMAND "${SDL2_CFLAGS}")
    target_compile_options(OpenIsles PUBLIC ${SDL2_CFLAGS})
endif(MINGW)

# Windows
if(MINGW)
    target_link_libraries(OpenIsles ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_MIXER_LIBRARY} ${SDL2_TTF_LIBRARY} winmm imm32 version freetype)

# Linux
else(MINGW)
    target_link_libraries(OpenIsles ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${SDL2_TTF_LIBRARIES})

endif(MINGW)


file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})


## Target: OpenIsles-Tests - die Tests #################################################################################

enable_testing()

add_executable(OpenIsles-Tests
    ${COMMON_SOURCE_FILES}
    ${NOSDL_FILES}
    ${TEST_FILES}
)

target_include_directories(OpenIsles-Tests PUBLIC ${CMAKE_SOURCE_DIR}/test/cpp)

# data-Verzeichnis für Tests erweitern
file(COPY ${CMAKE_SOURCE_DIR}/test/data DESTINATION ${CMAKE_BINARY_DIR})

# Google Test
find_package(GTest REQUIRED)
target_include_directories(OpenIsles-Tests PUBLIC ${GTEST_INCLUDE_DIRS})
target_compile_definitions(OpenIsles-Tests PUBLIC "-DNO_SDL -DDEFINE_VIRTUAL_ONLY_IN_TESTS")
target_link_libraries(OpenIsles-Tests ${GTEST_BOTH_LIBRARIES} pthread) # BOTH-Variable benutzen, damit wir uns an jeder Testdatei die main()-Methode sparen

add_test(OpenIslesTests OpenIsles-Tests)


## Doxygen #############################################################################################################

find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile)
    add_custom_target(
        generate-doxygen
        rm -rf ${CMAKE_SOURCE_DIR}/doxygen && ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
        VERBATIM
    )
endif(DOXYGEN_FOUND)