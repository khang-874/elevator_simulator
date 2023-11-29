#ifndef ELEVATOR_H
#define ELEVATOR_H

#include<string>
#include<QThread>
#include<QObject>
#include "ECS.h"
#include "mainwindow.h"
class ECS;
class MainWindow;

class Elevator : public QObject{
    Q_OBJECT
public:
    Elevator(int elevatorNumber, ECS* theECS, MainWindow *mainWindow) : isRunning(true), direction(S_IDLE), elevatorNumber(elevatorNumber),
        theECS(theECS),  mainWindow(mainWindow){};
    void start(char);
    void stop();
    void floorRequest(int floorNumber);
    char getDirection();
    char getState();
    void openDoor();
    void closeDoor();
    void fireNotified();
    void powerOutageNotified();
private:
    bool isRunning;
    char state;
    char direction;
    int elevatorNumber;
    ECS* theECS;
    MainWindow *mainWindow;

public slots:
    void doWork();
    void terminate();
};
#endif // ELEVATOR_H
