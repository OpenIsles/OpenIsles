#include "map/Street.h"
#include "map/Structure.h"


bool Structure::isStreet() const {
    return (dynamic_cast<const Street*>(this) != nullptr);
}