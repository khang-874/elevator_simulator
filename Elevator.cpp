#include "Elevator.h"

void Elevator::start(char direction){
    state = S_MOVING;
    this -> direction = direction;
}

void Elevator::stop(){
    state = S_IDLE;
    direction = S_IDLE;

}

char Elevator::getState(){
    return state;
}
char Elevator::getDirection(){
    return direction;
}

void Elevator::openDoor(){
    state = S_OPEN_DOOR;
}
void Elevator::closeDoor(){
    state = S_CLOSE_DOOR;
}

void Elevator::fireNotified(){
    state = FIRE;
}

void Elevator::powerOutageNotified(){
    state = POWER_OUTAGE;
}

void Elevator::floorRequest(int floorNumber){
    theECS -> carRequest(mainWindow ->getPosition(elevatorNumber) ,floorNumber, elevatorNumber);
}

void Elevator::doWork(){
    int count = 0;
    while(isRunning){
        mainWindow -> updateElevatorState(elevatorNumber, state, direction);
        if(state == S_OPEN_DOOR){
            //Open the door for extra amount of OPEN_DOOR_TIME/TIME_FRAME times
            //Since they always open for 1 times first
            if(count == OPEN_DOOR_TIME / TIME_FRAME){
                closeDoor();
                count = 0;

            }else
                count++;

        }else if(state == S_CLOSE_DOOR){
            //Change state to idling after 1 extra time frame
            if(count == 2){
                stop();
                count = 0;
            }else
                count++;
        }
        QThread::msleep(TIME_FRAME);
    }
}
void Elevator::terminate(){
    isRunning = false;
}
