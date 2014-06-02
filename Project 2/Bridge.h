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


// Parameters to the OneVehicle function
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


class Bridge
{
	int trafficDirection;					// Current direction of traffic flow.
	int carCount;							// Number of cars in the input file.

	queue<pthread_t> travelingLeftQueue;		// Queue of vehicle threads traveling from right to left.
	queue<pthread_t> travelingRightQueue;		// Queue of vehicle threads traveling from left to right.

public:
	// Constructor
	Bridge();
	// Deconstructor
	~Bridge();

	// OneVehicle() - Represents one vehicle, and executes the three functions below it.
	//void* OneVehicle(void* params);

	// ArriveBridge() - Executed by OneVehicle(). Does not return until it is safe for this "vehicle" to cross the bridge.
	//void ArriveBridge(int id, int direc);
	//// CrossBridge() - Executed by OneVehicle(). Used for debugging/checking if vehicles correctly cross bridge.
	//void CrossBridge(int id, int direc);
	//// ExitBridge() - Called when vechicle crosses bridge and performs operations to maintain flow of traffic.
	//void ExitBridge(int id, int direc);
};

