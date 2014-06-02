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
};

