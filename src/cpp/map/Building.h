#ifndef _BUILDING_H
#define _BUILDING_H

#include "Structure.h"

/**
 * @brief Structure, die ein Gebäude darstellt.
 */
class Building : public Structure {

public:
	/**
	 * @brief Callback, der sich um einen Mausklick auf das Gebäude kümmert
	 * @param mouseXInBuilding X-Koordinate innerhalb des Gebäude, wo geklickt wurde
	 * @param mouseYInBuilding Y-Koordinate innerhalb des Gebäude, wo geklickt wurde
	 */
	void onClick(int mouseXInBuilding, int mouseYInBuilding);
};

#endif
