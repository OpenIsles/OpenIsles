#include "Building.h"
#include "Map.h"

/**
 * @brief die Karte
 */
extern Map* map;

void Building::onClick(int mouseXInObject, int mouseYInObject) {
	std::cout << "Klick auf Building " << std::to_string(this->object) << "@(" << std::to_string(mouseXInObject) << ", "
			<< std::to_string(mouseYInObject) << ")" << std::endl;

	map->setSelectedMapObject(this);
}
