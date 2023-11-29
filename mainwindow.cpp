#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //set up UI
    ui->setupUi(this);

    //Initialize private member
    consoleLog = ui -> consoleLogsListWidget;
    theECS = nullptr;

    //Connect button to console to print the result
    connect(ui->firePushButton, &QPushButton::clicked, this, &MainWindow::printToConsole);
    connect(ui->helpPushButton, &QPushButton::clicked, this, &MainWindow::printToConsole);
    connect(ui->closePushButton, &QPushButton::clicked, this, &MainWindow::printToConsole);
    connect(ui->openPushButton, &QPushButton::clicked, this, &MainWindow::printToConsole);
    connect(ui->blockDoorPushButton, &QPushButton::clicked, this, &MainWindow::printToConsole);
    connect(ui->downFloorPushButton, &QPushButton::clicked, this, &MainWindow::printToConsole);
    connect(ui->upFloorPushButton, &QPushButton::clicked, this, &MainWindow::printToConsole);
    connect(ui->powerOutagePushButton, &QPushButton::clicked, this, &MainWindow::printToConsole);
    connect(ui->addPassengerPushButton, &QPushButton::clicked, this, &MainWindow::printToConsole);
    connect(ui->deletePassengerPushButton, &QPushButton::clicked, this, &MainWindow::printToConsole);

    //Connect changeDimension button to get the dimension information and initialize floors and elevators.
    connect(ui->changeDimensionsButton, &QPushButton::clicked, this, &MainWindow::getDimension);
    connect(ui->changeDimensionsButton, &QPushButton::clicked, this, &MainWindow::initializeElevatorsAndFloors);
    connect(ui->changeDimensionsButton, &QPushButton::clicked, this, &MainWindow::initializeFloorButton);

    //Connect passenger to corresponding function
    connect(ui->addPassengerPushButton, &QPushButton::clicked, this, &MainWindow::addPassenger);
    connect(ui->deletePassengerPushButton, &QPushButton::clicked, this, &MainWindow::deletePassenger);

    //Connect up and down floor push button
    connect(ui -> upFloorPushButton, &QPushButton::clicked, this, &MainWindow::floorRequest);
    connect(ui -> downFloorPushButton, &QPushButton::clicked, this, &MainWindow::floorRequest);

    //Connect fire and power outage push button
    connect(ui -> firePushButton, &QPushButton::clicked, this, &MainWindow::fireNotified);
    connect(ui -> powerOutagePushButton, &QPushButton::clicked, this, &MainWindow::powerOutageNotified);
    //Set default value for number of floors and number of elevators.
    ui -> numberElevatorsSpinBox -> setValue(MIN_ELEVATORS);
    ui -> numberFloorsSpinBox -> setValue(MIN_FLOORS);

    //Set threshhold for spinbox
    ui -> numberElevatorsSpinBox -> setMaximum(MAX_ELEVATORS);
    ui -> numberFloorsSpinBox -> setMaximum(MAX_FLOORS);
    ui -> numberElevatorsSpinBox -> setMinimum(MIN_ELEVATORS);
    ui -> numberFloorsSpinBox -> setMinimum(MIN_FLOORS);

    //Initilaize numFloors and numElevators
    numFloors = -1;
    numElevators = -1;

    //Initialize the elevators and floors
    initializeElevatorsAndFloors();
    initializeFloorButton();

    //Add 1 passenger at the beginning
    addPassenger();

    //set the weight testing input only accept number
    ui -> weightElevatorInput -> setInputMask(QString("999999"));
    ui -> maxWeightElevatorInput -> setInputMask(QString("999999"));
    //set the initial weight limit
    ui -> maxWeightElevatorInput -> setText(QString("1000"));
    // set the inital weight
    ui -> weightElevatorInput -> setText(QString("0"));

    //Conect block door button to the appropriate slot
    connect(ui -> blockDoorPushButton, &QPushButton::clicked, this, &MainWindow::blockDoor);

    //Connect help button to helpNotified slot
    connect(ui -> helpPushButton, &QPushButton::clicked, this, &MainWindow::helpNotified);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::getPosition(int elevatorNumber){
    for(int floor = 0; floor < numFloors; ++floor)
        if(floorElevator[getFloor(floor)][elevatorNumber] -> styleSheet() != C_EMPTY)
            return floor;
    return -1;
}

void MainWindow::getAllPosition(std::vector<std::pair<int, char>>& result){
    for(int elevator = 0; elevator < numElevators; ++elevator)
        for(int floor = 0; floor < numFloors; ++floor)
            if(floorElevator[getFloor(floor)][elevator] -> styleSheet() != C_EMPTY){
                result.push_back(std::make_pair(floor, elevators[elevator] -> getDirection()));
            }
}

void MainWindow::servePassenger(int destinationFloor, int assignElevator){
    int passengerIndex = -1;
    for(int i = 0; i < (int) passengers.size(); ++i)
        if(passengers[i] -> getAssignElevator() == assignElevator && passengers[i] -> getDestinationFloor() == destinationFloor
                && floorElevator[getFloor(destinationFloor)][assignElevator] -> styleSheet() != C_EMPTY){
            passengerIndex = i;
            break;
        }
    //If there is a request that being fulfilled.
    if(passengerIndex != -1){
        ui -> passengerComboBox -> removeItem(passengerIndex);
        passengers.erase(passengers.begin() + passengerIndex);
    }

}
void MainWindow::updateElevatorState(int elevatorNumber, char state, char direction){
       int floor = getPosition(elevatorNumber);
       int floorOnDisplay = getFloor(floor);
       if(state == S_IDLE){
            floorElevator[floorOnDisplay][elevatorNumber] -> setStyleSheet(C_IDLING);
       }else if(state == S_OPEN_DOOR){
            openDoor(floor, elevatorNumber);
       }else if(state == S_CLOSE_DOOR){
            closeDoor(floor, elevatorNumber);
       }else if(state == S_MOVING){
            if(direction == UP){
                if(floor < numFloors - 1){
                    floorElevator[floorOnDisplay][elevatorNumber] -> setStyleSheet(C_EMPTY);
                    floorElevator[getFloor(floor + 1)][elevatorNumber] -> setStyleSheet(C_MOVING);
                    elevatorLCD[elevatorNumber] -> display(floor + 1);
                    theECS -> newFloor(floor + 1, elevatorNumber);
                }else{
                    theECS -> newFloor(floor, elevatorNumber);
                }
            }else{
                if(floor > 0){
                    floorElevator[floorOnDisplay][elevatorNumber] -> setStyleSheet(C_EMPTY);
                    floorElevator[getFloor(floor - 1)][elevatorNumber] -> setStyleSheet(C_MOVING);
                    elevatorLCD[elevatorNumber] -> display(floor - 1);
                    theECS -> newFloor(floor - 1, elevatorNumber);
                }else{
                    theECS -> newFloor(floor, elevatorNumber);
                }
            }
       }
}

void MainWindow::assignElevator(int floorNumber, char direction, int assignedElevator){
    //Find the correct passenger to assign to
    for(int i = 0; i < (int) passengers.size(); ++i)
        if(passengers[i] -> getInitialFloor() == floorNumber && passengers[i] -> getDirectionRequest() == direction){
            passengers[i] -> setAssignElevator(assignedElevator);
            break;
        }
}

void MainWindow::emergencyAlert(const QString &message){
    consoleLog -> addItem(message);
}
//Private functions
int MainWindow::getFloor(int floor){
    return numFloors - 1 - floor >= 0 ? numFloors - 1 - floor : 0;
}

void MainWindow::openDoor(int floorNumber, int elevatorNumber){
    //consoleLog -> addItem(QString("Elevator ") + QString::number(elevatorNumber) + QString(" arrivate at floor ") + QString::number(floorNumber));
    consoleLog -> addItem(QString("Elevator ") + QString::number(elevatorNumber) + QString(" at floor ") + QString::number(floorNumber) + QString(" open door!"));
    floorElevator[getFloor(floorNumber)][elevatorNumber] -> setStyleSheet(C_OPEN_DOOR);
}

void MainWindow::closeDoor(int floorNumber, int elevatorNumber){
    consoleLog -> addItem(QString("Elevator ") + QString::number(elevatorNumber) + QString(" at floor ") + QString::number(floorNumber) + QString(" close door!"));
    consoleLog -> addItem(QString("Please choose the floor"));
    floorElevator[getFloor(floorNumber)][elevatorNumber] -> setStyleSheet(C_CLOSE_DOOR);
}

void MainWindow::freePreviousData(){
    if(theECS != nullptr)
        delete theECS;
    //Delete previous element
    if(numFloors != -1 && numElevators != -1){
           for(int floor = 0; floor < numFloors; ++floor)
               for(int elevator = 0; elevator < numElevators; ++elevator)
                   delete floorElevator[floor][elevator];
//           for(int elevator = 0; elevator < numElevators; ++elevator){
//               elevators[elevator] -> terminate();
//               delete elevators[elevator];
//           }
            for(int elevator = 0; elevator < numElevators; ++elevator){
                delete elevatorLCD[elevator];
                delete elevatorDisplayLabel[elevator];
            }
    }
}

int MainWindow::getPassengerIndex(int passengerNumber){
    for(int i = 0; i < (int)passengers.size(); ++i)
        if(passengers[i]->getPassengerNum() == passengerNumber)
            return i;
    return 0;
}

bool MainWindow::weightCheck(){
    int weightLimit = ui -> maxWeightElevatorInput -> text().toInt();
    int currentWeight = ui -> weightElevatorInput -> text().toInt();
    return currentWeight <= weightLimit;
}

//Slot functions
void MainWindow::printToConsole(){
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if(button != nullptr){
        consoleLog -> addItem(QString(button -> text() + " button pressed"));
        //qInfo("Button pressed!");
    }
}

void MainWindow::getDimension(){
    consoleLog -> addItem(QString("Change dimension button pressed!"));
    consoleLog -> addItem(QString("Change dimension to ") + QString::number(ui->numberFloorsSpinBox->value()) + QString(" floors and ")
                          + QString::number(ui->numberElevatorsSpinBox->value()) + QString(" elevators!"));

}

void MainWindow::initializeElevatorsAndFloors(){
    freePreviousData();


    numFloors = ui -> numberFloorsSpinBox -> value();
    numElevators = ui -> numberElevatorsSpinBox -> value();
    ui -> startingFloorSpinBox -> setMaximum(numFloors - 1);

    theECS = new ECS(numFloors, numElevators, this);
    theECSThread = new QThread();
    theECS -> moveToThread(theECSThread);
    theECSThread -> start();

    for(int floor = 0; floor < numFloors; ++floor)
        for(int elevator = 0; elevator < numElevators; ++elevator){
            floorElevator[floor][elevator] = new QLabel(this);
            floorElevator[floor][elevator] -> setStyleSheet(C_EMPTY);
            ui -> elevatorGrid -> addWidget(floorElevator[floor][elevator], floor, elevator);
        }
    for(int elevator = 0; elevator < numElevators; ++elevator){
        floorElevator[getFloor(0)][elevator] -> setStyleSheet(C_IDLING);
    }


    for(int floor = 0; floor < numFloors; ++floor){
       floors[floor] = new Floor(floor, theECS, this);
    }
    theECS -> setFloors(floors, numFloors);
    for(int elevator = 0; elevator < numElevators; ++elevator){
       elevators[elevator] = new Elevator(elevator, theECS, this);

       //Create thread, move to thread and spin up to run
       elevatorThread[elevator] = new QThread();
       elevators[elevator]->moveToThread(elevatorThread[elevator]);
       QObject::connect(elevatorThread[elevator], &QThread::started, elevators[elevator], &Elevator::doWork);
       elevatorThread[elevator]->start();
    }
    theECS -> setElevators(elevators, numElevators);
    initializeElevatorDisplay();
}

void MainWindow::initializeFloorButton(){
    if(floorButton.size() != 0){
        for(int floor = 0; floor < (int)floorButton.size(); ++floor)
            if(floorButton[floor] != nullptr)
                delete floorButton[floor];
    }
    floorButton = std::vector<QPushButton *>();
    numFloors = ui -> numberFloorsSpinBox -> value();
    //int numColumn = (int) std::floor(numFloors / 8);
    for(int floor = 0; floor < numFloors; ++floor){
            floorButton.push_back(new QPushButton(this));
            floorButton[floor] -> setText(QString("Floor ") + QString::number(floor));
            connect(floorButton[floor], &QPushButton::clicked, this, &MainWindow::printToConsole);
            connect(floorButton[floor], &QPushButton::clicked, this, &MainWindow::carRequest);
            ui -> floorHelpButtonGrid -> addWidget(floorButton[floor], (floor % 8) + 1, (int) std::floor(floor / 8));
    }
}

void MainWindow::initializeElevatorDisplay(){
    for(int elevator = 0; elevator < numElevators; ++elevator){
        elevatorDisplayLabel[elevator] = new QLabel(QString("Elevator ") + QString::number(elevator));
        elevatorDisplayLabel[elevator]->setStyleSheet("font-weight:bold");
        ui -> elevatorDisplayGrid -> addWidget(elevatorDisplayLabel[elevator], elevator, 0);
        elevatorLCD[elevator] = new QLCDNumber();
        ui -> elevatorDisplayGrid -> addWidget(elevatorLCD[elevator], elevator, 1);
    }
}
void MainWindow::carRequest(){
    if(ui -> passengerComboBox ->count() == 0){
        consoleLog -> addItem(QString("You haven't add any passenger yet!"));
        return;
    }
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    QString floorButton = button -> text();
    int destinationFloorNumber = floorButton.remove(0, FLOOR_STRING_LENGTH).toInt();
    int passengerNumber = ui -> passengerComboBox -> currentText() . toInt();
    int passengerIndex = getPassengerIndex(passengerNumber);
    //Check if there is any elevator at the current Floor
    bool isOnElevator = false;
    for(int elevator = 0; elevator < numElevators; ++elevator){
        if(getPosition(elevator) == passengers[passengerIndex] -> getInitialFloor()){
            isOnElevator = true;

            break;
        }
    }
    if(!isOnElevator){
        consoleLog -> addItem(QString("Passenger ") + QString::number(passengerNumber) + QString(" is not on the elevator yet!"));
        return;
    }

    if(!weightCheck()){
        consoleLog -> addItem(QString("Passenger ") + QString::number(passengerNumber) + QString(" please reduce the load, the weight limit is")
                              + QString::number(ui -> maxWeightElevatorInput -> text().toInt()));
        return;
    }
    int elevatorNumber = passengers[passengerIndex] -> getAssignElevator();
    int currentFloorNumber = passengers[passengerIndex] -> getInitialFloor();
    if(destinationFloorNumber == passengers[passengerIndex] -> getInitialFloor()){
        consoleLog -> addItem(QString("Passenger ") + QString::number(passengerNumber) + QString(" choosing the same destination floor!"));
        return;
    }
    passengers[passengerIndex] -> setDestinationFloor(destinationFloorNumber);
    theECS -> carRequest(currentFloorNumber, destinationFloorNumber, elevatorNumber);
}
void MainWindow::addPassenger(){
    numPassengers++;
    ui -> passengerComboBox -> addItem(QString::number(numPassengers));
    passengers.push_back(new Passenger(numPassengers));
}

void MainWindow::deletePassenger(){
    int currentPassenger = ui -> passengerComboBox -> currentText().toInt();
    int removePassenger = getPassengerIndex(currentPassenger);
    if(removePassenger != -1){
        ui -> passengerComboBox -> removeItem(removePassenger);
        passengers.erase(passengers.begin() + removePassenger);
    }
}

void MainWindow::floorRequest(){
    if(ui -> passengerComboBox -> count() == 0){
        consoleLog -> addItem(QString("You haven't add any passenger yet!"));
        return;
    }
    int floorNumber = ui -> startingFloorSpinBox -> value();
    int passengerNumber = ui -> passengerComboBox -> currentText() . toInt();
    int passengerIndex = getPassengerIndex(passengerNumber);

    passengers[passengerIndex] -> setInitialFloor(floorNumber);
    QPushButton *button = qobject_cast<QPushButton*> (sender());

    if(button -> text() == UP_BUTTON){
        passengers[passengerIndex] -> setDirectionRequest(UP);
        floors[floorNumber] -> inform(UP);
    }else{
        passengers[passengerIndex] -> setDirectionRequest(DOWN);
        floors[floorNumber] -> inform(DOWN);
    }
}

void MainWindow::fireNotified(){
    theECS -> fireNotified();
}

void MainWindow::powerOutageNotified(){
    theECS -> powerOutagedNotified();
}

void MainWindow::blockDoor(){
    int passengerNumber = ui -> passengerComboBox -> currentText(). toInt();
    int passengerIndex = getPassengerIndex(passengerNumber);
    int initialFloor = passengers[passengerIndex] -> getInitialFloor();
    int assignElevator = passengers[passengerIndex] -> getAssignElevator();
    if(floorElevator[getFloor(initialFloor)][assignElevator] -> styleSheet() == C_EMPTY){
        consoleLog -> addItem(QString("Passenger ") + QString::number(passengerNumber) + QString(", your elevator haven't arrived yet!"));
        return;
    }
    if(elevators[assignElevator] -> getState() != S_CLOSE_DOOR){
        consoleLog -> addItem(QString("Passenger ") + QString::number(passengerNumber) + QString(", the door is closed already!"));
        return;
    }
    theECS -> blockDoor(passengers[passengerIndex]->getAssignElevator());
    consoleLog -> addItem(QString("Passenger ") + QString :: number(passengerNumber)
                         + QString(" block door at elevator ") + QString::number(assignElevator));
}

void MainWindow::helpNotified(){
    int passengerNumber = ui -> passengerComboBox -> currentText().toInt();
    int passengerIndex = getPassengerIndex(passengerNumber);
    int assignElevator = passengers[passengerIndex] -> getAssignElevator();
    deletePassenger();
    if(assignElevator == -1){
        consoleLog -> addItem(QString("Passenger ") + QString::number(passengerNumber) + QString(" havaen't on any elevator yet!!"));
        return;
    }
    if(!theECS -> helpNotified(assignElevator)){
        consoleLog -> addItem(QString("No response, calll emergency!!!"));
    }else{
        consoleLog -> addItem(QString("Passenger ") + QString::number(passengerNumber) + QString(" received help!"));
    }
}


