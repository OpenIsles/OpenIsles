#include "Building.h"
#include "Map.h"

/**
 * @brief die Karte
 */
extern Map* map;

void Building::onClick(int mouseXInBuilding, int mouseYInBuilding) {
	std::cout << "Klick auf Building " << std::to_string(this->object) << "@(" << std::to_string(mouseXInBuilding)
			<< ", " << std::to_string(mouseYInBuilding) << ")" << std::endl;

	map->setSelectedMapObject(this);
}
