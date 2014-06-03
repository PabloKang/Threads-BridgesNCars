#include <string>
#include <queue>
#include <vector>
#include <iostream>
#include <fstream>
#include <pthread.h>


using namespace std;


#define DEBUG		1
#define LEFT_SIDE	0
#define RIGHT_SIDE	1


ifstream inFile("bridge.in");
ofstream outFile("bridge.out");


// VEHICLE FUNCTION DECLARATIONS:
void* OneVehicle(void* params);
void ArriveBridge(int id, int direc);
void CrossBridge(int id, int direc);
void ExitBridge(int id, int direc);


// VEHICLE STRUCT //////////////////////////////////////////////////////////////////////////////////////////////
struct vehicle
{
	int carID;
	int direction;

	vehicle(int id, int dir)
	{
		carID = id;
		direction = dir;
	}
};


// BRIDGE CLASS ////////////////////////////////////////////////////////////////////////////////////////////////
class Bridge
{
public:
	int trafficDirection;						// Current direction of traffic flow.
	int carCount;								// Number of cars in the input file.
	int nextTraveler;							// Next car that's allowed to drive onto bridge.
	unsigned int BRIDGE_CAPACITY = 3;			// Max cars the bridge can support at any given time.

	queue<vehicle> left_queue;					// Queue of vehicles waiting to cross bridge to the left side.
	queue<vehicle> right_queue;					// Queue of vehicles waiting to cross bridge to the right side.
	queue<vehicle> bridge_queue;				// Queue of vehicles currently crossing the bridge.
	
	vector<pthread_t> car_threads;				// Vector of all running car threads (each thread represents one vehicle.
	vector<pthread_t>::iterator threadIter;		// Iterator for car threads.



	// Constructor
	Bridge() {}
	// Deconstructor
	~Bridge() {}

	void bridgeScenario() 
	{
		if (!inFile){
			cout << "ERROR >> File 'bridge.in' was not found or could not be opened.\n" <<
				"         Unable to load data; exiting simulation...\n\n";
			exit(0);
		}
		else {
			inFile >> carCount;

			// Fill both Queues with car info from input file and Vector with car threads.
			for (int i = 0; i < carCount; i++){
				int carID;
				int carDir;

				inFile >> carID;
				inFile >> carDir;

				vehicle car(carID, carDir);

				if (carDir == LEFT_SIDE)
					left_queue.push(car);
				else if (carDir == RIGHT_SIDE)
					right_queue.push(car);
				else {
					cout << "ERROR >> Invalid direction of travel across bridge.\n";
					continue;
				}

				pthread_t carThread;
				vehicle* carData = new vehicle(carID, carDir);
				pthread_create(&carThread, NULL, OneVehicle, (void*)carData);

				threadIter = car_threads.begin();
				car_threads.insert(threadIter + (carID - 1), carThread);
			}
			inFile.close();

			// Set current nextTraveler on bridge to nobody.
			nextTraveler = 0;
		}
	}
};


// GLOBAL DATA DECLARATIONS:
Bridge bridge;										// Object of Bridge
pthread_mutex_t LOCK = PTHREAD_MUTEX_INITIALIZER;	// Mutual exclusion lock
int currentDirection;								// Current direction of travel across bridge


// VEHICLE FUNCTIONS ////////////////////////////////////////////////////////////////////////////////////////////////////
// OneVehicle() - Represents one vehicle, and executes the three functions below it.
void* OneVehicle(void* params){
	vehicle* p = (vehicle*)params;

	ArriveBridge(p->carID, p->direction);
	CrossBridge(p->carID, p->direction);
	ExitBridge(p->carID, p->direction);
	
	return 0;
}

// ArriveBridge() - Executed by OneVehicle(). Does not return until it is safe for this "vehicle" to cross the bridge.
void ArriveBridge(int id, int direc)
{
	bool waiting = true;
	int index = 0;

	//cout << "Vehicle " << id << " has arrived at the bridge, traveling in direction " << direc << endl; ///////////////////////////////////////////////// ???

	while (waiting){
		//cout << "Vehicle " << id << " is waiting to cross the bridge in direction " << direc << endl;

		if (bridge.nextTraveler == id)
			waiting = false;
	}
}

// CrossBridge() - Executed by OneVehicle(). Used for debugging/checking if vehicles correctly cross bridge.
void CrossBridge(int id, int direc)
{
	vehicle thisCar(id, direc);

	pthread_mutex_trylock(&LOCK);	// lock the carCounter
	bridge.bridge_queue.push(thisCar);
	pthread_mutex_unlock(&LOCK);	// unclock the carCounter

	//if (DEBUG)
	//	cout << "DEBUG >> Vehicle " << id << " is crossing the bridge in direction " << direc << endl; ///////////////////////////////////////////////////// ???
}

// ExitBridge() - Called when vechicle crosses bridge and performs operations to maintain flow of traffic.
void ExitBridge(int id, int direc)
{
	pthread_mutex_trylock(&LOCK);	// lock the current nextTraveler & carCounter
	bridge.bridge_queue.pop();
	pthread_mutex_unlock(&LOCK);	// unclock the current nextTraveler & carCounter

	if (DEBUG)
		cout << "DEBUG >> Vehicle " << id << " has finished crossing the bridge in direction " << direc << endl;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTION OF BRIDGE CONTROL ////////////////////////////////////////////////////////////////////////////////////
int main(int argc, const char * argv[])
{
	bool bridgePause = false;
	currentDirection = LEFT_SIDE;

	bridge.bridgeScenario();

	while (!(bridge.left_queue.empty() && bridge.right_queue.empty() && bridge.bridge_queue.empty())) {
		// Check if only one side has exhausted all vehicles or capacity reached.
		if ((currentDirection == LEFT_SIDE && bridge.left_queue.empty()) || bridge.bridge_queue.size() == bridge.BRIDGE_CAPACITY) { //carCounter == bridge.BRIDGE_CAPACITY)
			currentDirection = RIGHT_SIDE;
			bridgePause = true;
		}
		else if (currentDirection == RIGHT_SIDE && bridge.right_queue.empty() || bridge.bridge_queue.size() == bridge.BRIDGE_CAPACITY) {
			currentDirection = LEFT_SIDE;
			bridgePause = true;
		}

		// If switching direction, wait until bridge empties before letting opposite cars on.
		while (bridgePause) {
			if (bridge.bridge_queue.size() == 0)
				bridgePause = false;
		}

		// If bridge has room in current direction.
		if (bridge.bridge_queue.size() < bridge.BRIDGE_CAPACITY) {
			// Get carID of next available nextTraveler.
			if (currentDirection == LEFT_SIDE) {
				bridge.nextTraveler = bridge.left_queue.front().carID;
				bridge.left_queue.pop();
			}
			else if (currentDirection == RIGHT_SIDE) {
				bridge.nextTraveler = bridge.right_queue.front().carID;
				bridge.right_queue.pop();
			}
		}
	}

	system("pause");

	// Join up all completed threads
	while (!bridge.car_threads.empty()) {
		(void) pthread_join(bridge.car_threads.back(), NULL);
		bridge.car_threads.pop_back();
	}
	cout << "Goodbye!\n";

	system("pause");

	return 0;
}