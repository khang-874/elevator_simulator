#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QListWidget>
#include <QtGlobal>
#include <QPushButton>
#include <QLCDNumber>
#include <QLabel>
#include <math.h>
#include <map>
#include <vector>
#include "defs.h"
#include "ECS.h"
#include "Elevator.h"
#include "Floor.h"
#include "Passenger.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class ECS;
class Elevator;
class Floor;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int getPosition(int elevatorNumber);
    void getAllPosition(std::vector<std::pair<int, char>>& result);
    void updateElevatorState(int elevatorNumber, char state, char direction);
    void assignElevator(int floorNumber, char direction, int assignedElevator);
    void servePassenger(int destinationFloor, int assignElevator);
    void emergencyAlert(const QString& message);
private:
    Ui::MainWindow *ui;
    QListWidget *consoleLog;
    QLabel* floorElevator[MAX_FLOORS][MAX_ELEVATORS];
    std::vector<QPushButton*> floorButton;
    ECS* theECS;
    QThread* theECSThread;
    Elevator* elevators[MAX_ELEVATORS];
    QThread* elevatorThread[MAX_ELEVATORS];
    Floor* floors[MAX_FLOORS];
    std::vector<std::pair<int, int>> request;
    std::vector<Passenger*> passengers;
    QLCDNumber* elevatorLCD[MAX_ELEVATORS];
    QLabel* elevatorDisplayLabel[MAX_ELEVATORS];
    int numFloors;
    int numElevators;
    int numPassengers;
    int getFloor(int floor);
    void openDoor(int floorNumber, int elevatorNumber);
    void closeDoor(int floorNumber, int elevatorNumber);
    void freePreviousData();
    int getPassengerIndex(int passengerNumber);
    void initializeElevatorDisplay();
    bool weightCheck();
private slots:
    void addPassenger();
    void deletePassenger();
    void printToConsole();
    void getDimension();
    void initializeElevatorsAndFloors();
    void initializeFloorButton();
    void carRequest();
    void floorRequest();
    void fireNotified();
    void blockDoor();
    void powerOutageNotified();
    void helpNotified();
};
#endif // MAINWINDOW_H
