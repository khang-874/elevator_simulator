#ifndef PASSENGER_H
#define PASSENGER_H

class Passenger{
public:
    Passenger(int passengerNum) : passengerNum(passengerNum), assignElevator(-1), initialFloor(-1), destinationFloor(-1), directionRequest(0){};
    void setAssignElevator(int assignElevator){this -> assignElevator = assignElevator;}
    void setPassengerNum(int num){passengerNum = num;}
    void setInitialFloor(int num){initialFloor = num;}
    void setDestinationFloor(int num){destinationFloor = num;}
    void setDirectionRequest(char dir){directionRequest = dir;}
    char getDirectionRequest(){return directionRequest;}
    int getDestinationFloor(){return destinationFloor;}
    int getInitialFloor(){return initialFloor;}
    int getAssignElevator(){return assignElevator;}
    int getPassengerNum(){return passengerNum;}
private:
    int passengerNum;
    int assignElevator;
    int initialFloor;
    int destinationFloor;
    char directionRequest;
};
#endif // PASSENGER_H
