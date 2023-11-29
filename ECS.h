#ifndef ECS_H
#define ECS_H

#include <QObject>
#include <vector>
#include <utility>
#include "defs.h"
#include "Floor.h"
#include "Elevator.h"
#include "mainwindow.h"



class MainWindow;

class ECS : public QObject{
    Q_OBJECT
public:
    ECS(int numElevators, int numFloors, MainWindow *mainWindow) : numElevators(numElevators), numFloors(numFloors),mainWindow(mainWindow) {};
    void setElevators(Elevator **elevators, int numElevators);
    void setFloors(Floor **floors, int numFloors);
    void floorRequest(int floorNumber, char direction);
    void carRequest(int startFloor, int destinationFloor, int carNumber);
    void newFloor(int floorNumber, int carNumber);
    bool helpNotified(int elevatorNumber);
    void fireNotified();
    void powerOutagedNotified();
    void blockDoor(int elevatorNumber);

private:
    int numElevators;
    int numFloors;
    MainWindow *mainWindow;
    Elevator* elevators[MAX_ELEVATORS];
    Floor* floors[MAX_FLOORS];
    std::vector<std::pair<int, char>> request;
    std::vector<std::pair<int, int>> task;
};
#endif // ECS_H
