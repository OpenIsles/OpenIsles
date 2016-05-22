#include <algorithm>
#include <cassert>
#include <list>
#include <string>
#include "global.h"
#include "map/Structure.h"
#include "gui/Identifiers.h"
#include "gui/components/map/GuiMap.h"
#include "gui/components/GuiAddBuildingWidget.h"
#include "gui/components/GuiBuildMenu.h"
#include "gui/components/GuiButton.h"
#include "gui/components/GuiPushButton.h"
#include "gui/components/GuiStaticGraphicElement.h"
#include "gui/panel-widgets/GuiBuildMenuWidget.h"


GuiBuildMenuWidget::GuiBuildMenuWidget(const Context& context) : GuiPanelWidget(context) {
    // TODO in Config auslagern
    static struct {
        BuildingGroup buildingGroup;

        struct {
            int x, y;
        } buttonPosition;

        const std::string name;
        const std::string graphicSetName;
        const std::string graphicSetPressedName; // TODO Zustände nutzen

        std::list<const MapObjectType*> mapObjectTypes; // leere Liste für Abreißmodus

    } buildingGroups[BuildingGroup::_SIZE] = {
        {
            BuildingGroup::DEMOLISH,
            { 177, 280 },
            _("Switch to demolition mode"),
            "build-menu/demolish",
            "build-menu/demolish-pressed",
            {}
        }, {
            BuildingGroup::CRAFTSMAN,
            { 12, 345 },
            _("Craftsmanship"),
            "add-building-group/craftsman", // TODO build-menu-Grafik verwenden
            "add-building-group/craftsman-pressed", { // TODO build-menu-Grafik verwenden
                context.configMgr->getMapObjectType("stonemason"),
                nullptr,
                nullptr,
                context.configMgr->getMapObjectType("toolsmiths"),
                context.configMgr->getMapObjectType("tobacco-goods"),
                context.configMgr->getMapObjectType("distillery"),
                nullptr,
                nullptr,
                context.configMgr->getMapObjectType("weaving-mill1"),
                context.configMgr->getMapObjectType("butchers"),
                context.configMgr->getMapObjectType("grain-mill"),
                context.configMgr->getMapObjectType("bakery")
            }
        }, {
            BuildingGroup::FARM,
            { 67, 345 },
            _("Farms & plantations"),
            "add-building-group/farm", // TODO build-menu-Grafik verwenden
            "add-building-group/farm-pressed", { // TODO build-menu-Grafik verwenden
                nullptr,
                nullptr,
                context.configMgr->getMapObjectType("sugarcane-plantation"),
                context.configMgr->getMapObjectType("sugarcane-field"),
                context.configMgr->getMapObjectType("tobacco-plantation"),
                context.configMgr->getMapObjectType("tobacco-field"),
                context.configMgr->getMapObjectType("spices-plantation"),
                context.configMgr->getMapObjectType("spices-field"),
                context.configMgr->getMapObjectType("grain-farm"),
                context.configMgr->getMapObjectType("grain-field"),
                context.configMgr->getMapObjectType("sheep-farm"),
                context.configMgr->getMapObjectType("cattle-farm"),
                context.configMgr->getMapObjectType("hunters-hut"),
                context.configMgr->getMapObjectType("foresters"),
                context.configMgr->getMapObjectType("northern-forest1")
            }
        }, {
            BuildingGroup::PORT,
            { 122, 345 },
            _("Port facilities"),
            "build-menu/port",
            "build-menu/port-pressed", {
                nullptr,
                context.configMgr->getMapObjectType("pier"),
                nullptr,
                nullptr,
                context.configMgr->getMapObjectType("office1"),
                context.configMgr->getMapObjectType("office2"),
                nullptr,
                nullptr
            }
        }, {
            BuildingGroup::PUBLIC,
            { 177, 345 },
            _("Public buildings"),
            "build-menu/public",
            "build-menu/public-pressed", {
                context.configMgr->getMapObjectType("square1"),
                context.configMgr->getMapObjectType("square2"),
                context.configMgr->getMapObjectType("square3"),
                nullptr,
                context.configMgr->getMapObjectType("cathedral"),
                context.configMgr->getMapObjectType("tavern"),
                nullptr,
                context.configMgr->getMapObjectType("school"),
                nullptr,
                nullptr,
                context.configMgr->getMapObjectType("farm-road"),
                context.configMgr->getMapObjectType("cobbled-street"),
                context.configMgr->getMapObjectType("pioneers-house1"),
                context.configMgr->getMapObjectType("marketplace"),
                context.configMgr->getMapObjectType("chapel"),
                nullptr
            }
        }
    };

    for (int groupIndex = 0; groupIndex < BuildingGroup::_SIZE; groupIndex++) {
        // Button für die Gruppe
        const IGraphic* graphic =
            context.graphicsMgr->getGraphicSet(buildingGroups[groupIndex].graphicSetName)->getStatic()->getGraphic();
        const IGraphic* graphicPressed =
            context.graphicsMgr->getGraphicSet(buildingGroups[groupIndex].graphicSetPressedName)->getStatic()->getGraphic();

        GuiPushButton* addBuildingPushButton = new GuiPushButton(context);
        addBuildingPushButton->setGraphic(graphic);
        addBuildingPushButton->setGraphicPressed(graphicPressed);
        addBuildingPushButton->setUseShadow((groupIndex == 0 || groupIndex >= 3) ? true : false); // TODO einheitlich true setzen, wenn die Blender-Grafiken komplett sind
        addBuildingPushButton->setCoords(
            buildingGroups[groupIndex].buttonPosition.x, buildingGroups[groupIndex].buttonPosition.y,
            graphic->getWidth(), graphic->getHeight());
        addBuildingPushButton->setStatusBarText(buildingGroups[groupIndex].name);
        addBuildingPushButton->setOnClickFunction([this, &context, addBuildingPushButton, groupIndex]() {
            // Sonderfall: Abreißmodus
            if (buildingGroups[groupIndex].mapObjectTypes.empty()) {
                Log::debug("TODO Abreißmodus");

                context.guiMgr->panelState.selectedBuildingGroup = (BuildingGroup) groupIndex;
                context.guiMgr->panelState.buildingMenuOpen = false;
                context.guiMgr->updateGuiFromPanelState();
                return;
            }

            GuiBuildMenu* guiBuildMenu = dynamic_cast<GuiBuildMenu*>(context.guiMgr->findElement(GUI_ID_BUILD_MENU));

            // Wenn man die Gruppe nochmal klickt, die bereits ausgewählt ist und das ausgewählte Gebäude nicht
            // aus dieser Gruppe ist, wird das Gebäude gewechselt und eins aus der Gruppe genommen
            if (context.guiMgr->panelState.selectedBuildingGroup == (BuildingGroup) groupIndex &&
                context.guiMgr->panelState.buildingMenuOpen) {

                bool addingStructureInSelectedBuildingGroup =
                    (std::find(buildingGroups[groupIndex].mapObjectTypes.cbegin(),
                               buildingGroups[groupIndex].mapObjectTypes.cend(),
                               context.guiMgr->panelState.addingMapObject) !=
                                        buildingGroups[groupIndex].mapObjectTypes.cend());

                if (!addingStructureInSelectedBuildingGroup) {
                    // Button unten links nehmen (is etwas tricky, das auch zu rechnen, da die Liste nicht
                    // unbedingt ein 4-Vielfaches an Elementen hat)
                    auto iter = buildingGroups[groupIndex].mapObjectTypes.cend();
                    std::size_t decreaseIter = 1 + // eins extra, weil cend() immer eins nach dem Ende is
                        (buildingGroups[groupIndex].mapObjectTypes.size() == 0) ?
                                               4 : (buildingGroups[groupIndex].mapObjectTypes.size() % 4);
                    std::advance(iter, -decreaseIter);

                    // Unten links sollte immer freigeschaltet sein
                    assert (*iter != nullptr);

                    context.guiMgr->panelState.addingMapObject = *iter;
                }
            }
            else {
                context.guiMgr->panelState.selectedBuildingGroup = (BuildingGroup) groupIndex;
                context.guiMgr->panelState.buildingMenuOpen = true;

                // Build-Menü mit den richtigen Buttons füllen (das ändert seine Höhe)
                guiBuildMenu->setButtons(buildingGroups[groupIndex].mapObjectTypes);

                // nun umpositionieren
                int newBuildMenuY = addBuildingPushButton->getWindowY() - guiBuildMenu->getHeight() - 5;
                guiBuildMenu->setY(newBuildMenuY);
            }
            context.guiMgr->updateGuiFromPanelState();
        });
        context.guiMgr->registerElement(GUI_ID_ADD_BUILDING_PUSH_BUTTON_BASE + groupIndex, addBuildingPushButton);
        addChildElement(addBuildingPushButton);
    }

    // Gebäudebau: Infos, über zu platzierendes Gebäude
    GuiAddBuildingWidget* addBuildingWidget = new GuiAddBuildingWidget(context);
    context.guiMgr->registerElement(GUI_ID_ADD_BUILDING_WIDGET, addBuildingWidget);
    addChildElement(addBuildingWidget);
}