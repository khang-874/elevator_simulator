#include "ECS.h"

void ECS::setElevators(Elevator **elevators, int numElevators){
    for(int i = 0; i < numElevators; ++i)
        this -> elevators[i] = elevators[i];
    this -> numElevators = numElevators;
}

void ECS::setFloors(Floor **floors, int numFloors){
    for(int i = 0; i < numFloors; ++i)
        this -> floors[i] = floors[i];
    this -> numFloors = numFloors;
}

void ECS::floorRequest(int floorNumber, char direction){
    std::vector<std::pair<int, char>> position = std::vector<std::pair<int, char>>();
    mainWindow -> getAllPosition(position);

    std::vector<std::pair<int, std::pair<int, char>>> distance = std::vector<std::pair<int, std::pair<int, char>>>();
    int dis, dir, elevator;
    for(elevator = 0; elevator < (int)position.size(); ++elevator){
        dis = abs(position[elevator].first - floorNumber);
        dir = position[elevator].second;
        distance.push_back(std::make_pair(dis, std::make_pair(elevator, dir)));
    }
    sort(distance.begin(), distance.end());
    for(int i = 0; i < (int) distance.size(); ++i){
        dir = distance[i].second.second;
        elevator = distance[i].second.first;
        if(dir == S_IDLE){
            if(position[elevator].first < floorNumber)
                elevators[elevator] -> start(UP);
            else if(position[elevator].first > floorNumber){
                elevators[elevator] -> start(DOWN);
            }else{
                //if the elevator is already at the floor serve the passenger immediately
                elevators[elevator] -> stop();
                elevators[elevator] -> openDoor();
                mainWindow -> assignElevator(floorNumber, direction, elevator);
                return;
            }
            mainWindow -> assignElevator(floorNumber, direction, elevator);
            break;
        }else if(dir == direction){
            mainWindow -> assignElevator(floorNumber, direction, elevator);
            break;
        }
    }

    task.push_back(std::make_pair(floorNumber, elevator));
}

void ECS::carRequest(int startFloor, int destinationFloor, int carNumber){
    task.push_back(std::make_pair(destinationFloor, carNumber));
    elevators[carNumber] -> closeDoor();
    if(startFloor < destinationFloor){
        elevators[carNumber] -> start(UP);
    }else if(startFloor > destinationFloor){
        elevators[carNumber] -> start(DOWN);
    }
}

void ECS::newFloor(int floorNumber, int carNumber){
    int completeTask;
    while(true){
        //Check all possible task.
        completeTask = -1;
        for(int i = 0; i < (int) task.size(); ++i)
            if(task[i].first == floorNumber && task[i].second == carNumber){
                completeTask = i;
                break;
            }
       if(completeTask != -1){
           elevators[carNumber] -> stop();
           elevators[carNumber] -> openDoor();
           mainWindow -> servePassenger(floorNumber, carNumber);
           task.erase(task.begin() + completeTask);
       }else{
           break;
       }
   }
}

bool ECS::helpNotified(int elevatorNumber){
    //start go to ground for assistance
    elevators[elevatorNumber] -> stop();
    elevators[elevatorNumber] -> start(DOWN);
    int completeTask = -1;
    while(true){
        completeTask = -1;
        for(int i = 0; i < (int) task.size(); ++i)
            if(task[i].second == elevatorNumber){
                completeTask = i;
                break;
            }
        if(completeTask != -1)
            task.erase(task.begin() + completeTask);
        else
            break;
    }
    task.push_back(std::make_pair(0, elevatorNumber));
    int random = rand() % 2;
    return random == 0;
}

void ECS::powerOutagedNotified(){
    task = std::vector<std::pair<int, int>>();
    for(int elevator = 0; elevator < numElevators; ++elevator){
        elevators[elevator] -> stop();
        elevators[elevator] -> start(DOWN);
        task.push_back(std::make_pair(0, elevator));
    }
    mainWindow -> emergencyAlert(QString("No power! No Power! No Power! Try to move out the building as fast as possible!!"));
}
void ECS::fireNotified(){
    task = std::vector<std::pair<int, int>>();
    for(int elevator = 0; elevator < numElevators; ++elevator){
        elevators[elevator] -> stop();
        elevators[elevator] -> start(DOWN);
        task.push_back(std::make_pair(0, elevator));
    }
    mainWindow -> emergencyAlert(QString("Fire! Fire! Fire! Try to move out the building as fast as possible!!"));
}

void ECS::blockDoor(int elevatorNumber){
    elevators[elevatorNumber] -> openDoor();
}
