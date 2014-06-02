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
	int carID;
	int carDir;

	vehicle(int id, int dir)
	{
		carID = id;
		carDir = dir;
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

	queue<vehicle> left_ID_queue;				// Queue of vehicle IDs & directions traveling from right to left.
	queue<vehicle> right_ID_queue;				// Queue of vehicle IDs & directions traveling from left to right.
	vector<pthread_t> car_threads;				// Vector of all running car threads.
	vector<pthread_t>::iterator threadIter;

	// Constructor
	Bridge() 
	{
		string fileName = "";

		cout << "Enter input file name: ";
		getline(cin, fileName);

		fstream inFile(fileName.c_str());

		if (!inFile){
			cout << "ERROR >> File not found or could not be opened.\n";
		}
		else {
			inFile >> carCount;

			car_threads = vector<pthread_t>(carCount);

			// Fill both Queues with car info from input file and Vector with car threads.
			for (int i = 0; i < carCount; i++){
				int carID;
				int carDir;

				inFile >> carID;
				inFile >> carDir;

				vehicle car(carID, carDir);

				if (carDir == LEFT_SIDE) 
					left_ID_queue.push(car);
				else if (carDir == RIGHT_SIDE) 
					right_ID_queue.push(car);
				else 
					cout << "ERROR >> Invalid direction of travel across bridge.\n";

				pthread_t carThread;
				func_params* carData = new func_params(carID, carDir);
				pthread_create(&carThread, NULL, OneVehicle, (void*)carData);

				threadIter = car_threads.begin();
				car_threads.insert(threadIter + (carID-1), carThread);
			}
			inFile.close();

			// Set current traveler on bridge to nobody.
			traveler = 0;
		}
	}


	// Deconstructor
	~Bridge() {}
};


// GLOBAL DATA DECLARATIONS:
Bridge bridge;			// Object of Bridge
pthread_mutex_t LOCK;	// Mutual exclusion lock
int currentDirection;
int carCounter;


// VEHICLE FUNCTIONS ////////////////////////////////////////////////////////////////////////////////////////////////////
// OneVehicle() - Represents one vehicle, and executes the three functions below it.
void* OneVehicle(void* params){
	func_params* p = (func_params*)params;
	int travelerIndex;
	travelerIndex = ArriveBridge(p->carID, p->direction);
	CrossBridge(p->carID, p->direction);
	ExitBridge(p->carID, p->direction, travelerIndex);
	
	return 0;
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
	pthread_mutex_trylock(&LOCK);	// lock the carCounter
	carCounter++;
	pthread_mutex_unlock(&LOCK);	// unclock the carCounter

	if (DEBUG)
		cout << "DEBUG >> Vehicle " << id << " is crossing the bridge in direction " << direc << endl;
}

// ExitBridge() - Called when vechicle crosses bridge and performs operations to maintain flow of traffic.
void ExitBridge(int id, int direc, int index)
{
	pthread_mutex_trylock(&LOCK);	// lock the current traveler & carCounter
	bridge.traveler = 0;
	carCounter--;
	pthread_mutex_unlock(&LOCK);	// unclock the current traveler & carCounter
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTION OF BRIDGE CONTROL ////////////////////////////////////////////////////////////////////////////////////
int main(int argc, const char * argv[])
{
	bridge = Bridge();
	
	currentDirection = LEFT_SIDE;
	carCounter = 0;

	while (!bridge.left_ID_queue.empty() || !bridge.right_ID_queue.empty()) {
		// Check if only one side has exhausted all vehicles or capacity reached.
		if (currentDirection == LEFT_SIDE && bridge.left_ID_queue.empty() || carCounter == bridge.CAR_CAPACITY)
			currentDirection = RIGHT_SIDE;
		else if (currentDirection == RIGHT_SIDE && bridge.right_ID_queue.empty() || carCounter == bridge.CAR_CAPACITY)
			currentDirection = LEFT_SIDE;

		// If bridge has room in current direction.
		if (carCounter < bridge.CAR_CAPACITY) {
			// Get carID of next available traveler.
			if (currentDirection == LEFT_SIDE && bridge.left_ID_queue.front().carDir == LEFT_SIDE)
				bridge.traveler = bridge.left_ID_queue.front().carID;

			else if (currentDirection == RIGHT_SIDE && bridge.right_ID_queue.front().carDir == RIGHT_SIDE)
				bridge.traveler = bridge.right_ID_queue.front().carID;
		}
	}
	return 0;
}