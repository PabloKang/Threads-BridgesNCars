#include <string>
#include <queue>
#include <vector>
#include <iostream>
#include <fstream>
#include <pthread.h>


using namespace std;


#define DEBUG		0
#define LEFT_SIDE	0
#define RIGHT_SIDE	1


// GLOBAL DATA DECLARATIONS:
Bridge bridge;			// Object of Bridge
pthread_mutex_t LOCK;	// Mutual exclusion lock


// VEHICLE FUNCTION DECLARATIONS:
void* OneVehicle(void* params);
int ArriveBridge(int id, int direc);
void CrossBridge(int id, int direc);
void ExitBridge(int id, int direc, int index);


// FUNC_PARAMS STRUCT ////////////////////////////////////////////////////////////////////////////////////////////
struct func_params
{
	int carID;			// ID of car.
	int direction;		// Intended destination (LEFT_SIDE / RIGHT_SIDE) of car.

	// Struct Constructor
	func_params(int id, int dir)
	{
		carID = id;
		direction = dir;
	}
};


// VEHICLE STRUCT //////////////////////////////////////////////////////////////////////////////////////////////
struct vehicle
{
	pthread_t carThread;
	int carID;
	int carDir;

	vehicle(int id, int dir)
	{
		carID = id;
		carDir = dir;
		func_params* carData = new func_params(carID, carDir);
		pthread_create(&carThread, NULL, OneVehicle, (void*)carData);
	}
};


// BRIDGE CLASS ////////////////////////////////////////////////////////////////////////////////////////////////
class Bridge
{
public:
	int trafficDirection;						// Current direction of traffic flow.
	int carCount;								// Number of cars in the input file.
	int traveler;								// Dynamic array of current travelers across bridge.

	int CAR_CAPACITY = 3;						// Max cars the bridge can support at any given time.

	queue<vehicle> travelingLeftQueue;		// Queue of vehicle threads traveling from right to left.
	queue<vehicle> travelingRightQueue;		// Queue of vehicle threads traveling from left to right.


	// Constructor
	Bridge();
	// Deconstructor
	~Bridge();
};



// VEHICLE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////

// OneVehicle() - Represents one vehicle, and executes the three functions below it.
void* OneVehicle(void* params){
	func_params* p = (func_params*)params;
	int travelerIndex;
	travelerIndex = ArriveBridge(p->carID, p->direction);
	CrossBridge(p->carID, p->direction);
	ExitBridge(p->carID, p->direction, travelerIndex);
}

// ArriveBridge() - Executed by OneVehicle(). Does not return until it is safe for this "vehicle" to cross the bridge.
int ArriveBridge(int id, int direc)
{
	bool waiting = true;
	int index = 0;

	cout << "Vehicle " << id << " has arrived at the bridge, traveling in direction " << direc << endl;

	while (waiting){
		cout << "Vehicle " << id << " is waiting to cross the bridge in direction " << direc << endl;

		if (bridge.traveler == id) {
			waiting = false;
		}
	}
	return index;
}

// CrossBridge() - Executed by OneVehicle(). Used for debugging/checking if vehicles correctly cross bridge.
void CrossBridge(int id, int direc)
{
	if (DEBUG)
		cout << "DEBUG >> Vehicle " << id << " is crossing the bridge in direction " << direc << endl;
}

// ExitBridge() - Called when vechicle crosses bridge and performs operations to maintain flow of traffic.
void ExitBridge(int id, int direc, int index)
{
	pthread_mutex_trylock(&LOCK);	// lock the current traveler
	bridge.traveler = 0;
	pthread_mutex_unlock(&LOCK);	// unclock the current traveler
}
