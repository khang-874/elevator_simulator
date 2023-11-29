#ifndef DEFS_H
#define DEFS_H

#include <QString>

const int MAX_FLOORS=100;
const int MAX_ELEVATORS=100;
const int MIN_FLOORS = 4;
const int MIN_ELEVATORS = 2;
const char UP = 0;
const char DOWN = 1;
//Prefix S stands for state, C stands for color
const char S_IDLE = 2;
const char S_OPEN_DOOR = 4;
const char S_CLOSE_DOOR = 5;
const char S_EMPTY = 6;
const char S_MOVING = 7;
const char HELP = 0;
const char FIRE = 1;
const char POWER_OUTAGE=2;
const int ON = 1;
const int OFF = 0;
const int FLOOR_STRING_LENGTH = 6;
const int TIME_FRAME = 1000;
const int OPEN_DOOR_TIME = 2000;
const QString C_IDLING = QString("background-color:yellow;");
const QString C_MOVING= QString("background-color:green;");
const QString C_OPEN_DOOR = QString("background-color:orange;");
const QString C_CLOSE_DOOR = QString("background-color:red;");
const QString C_EMPTY = QString("background-color:white;");
const QString UP_BUTTON = QString("Up");
const QString DOWN_BUTTON = QString("Down");
#endif // DEFS_H
