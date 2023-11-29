#include "Floor.h"

void Floor::inform(char direction){
    theECS -> floorRequest(floorNumber, direction);
}
