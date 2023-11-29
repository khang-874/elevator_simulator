#ifndef FLOOR_H
#define FLOOR_H

#include "ECS.h"
#include "mainwindow.h"
class ECS;
class MainWindow;

class Floor{
public:
    Floor(int number, ECS* theECS, MainWindow *mainWindow) : floorNumber(number), theECS(theECS), mainWindow(mainWindow){};
    ~Floor();
    void inform(char);
private:
    int floorNumber;
    ECS *theECS;
    MainWindow *mainWindow;
};
#endif // FLOOR_H
