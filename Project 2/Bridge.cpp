/****************************************************************
*	TITLE: ICS 53 Project 2 "The Bridge"						*
*	AUTHORS:	Kaveh Ghodstinat	(83025710)					*
*				Goris Haghverdian	(48207671)					*
*				Pablo Kang			(58842064)					*
*****************************************************************/

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


struct car_thread
{
	bool waiting;
	pthread_t car_function;

	car_thread(vehicle* carData)
	{
		waiting = true;
		pthread_create(&car_function, NULL, OneVehicle, (void*)carData);
	}
};


// BRIDGE CLASS ////////////////////////////////////////////////////////////////////////////////////////////////
class Bridge
{
public:
	int trafficDirection;						// Current direction of traffic flow.
	int carCount;								// Number of cars in the input file.
	unsigned int BRIDGE_CAPACITY = 1;			// Max cars the bridge can support at any given time.

	queue<vehicle> left_queue;					// Queue of vehicles waiting to cross bridge to the left side.
	queue<vehicle> right_queue;					// Queue of vehicles waiting to cross bridge to the right side.
	queue<vehicle> bridge_queue;				// Queue of vehicles currently crossing the bridge.
	vector<car_thread> car_threads;				// Vector of all running car threads (each thread represents one vehicle.


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
					cout << "\nERROR >> Invalid direction of travel across bridge. Ignoring car.";
					continue;
				}

				vehicle* carData = new vehicle(carID, carDir);
				car_thread carThread(carData);

				car_threads.insert(car_threads.begin() + (carID - 1), carThread);
			}
			inFile.close();
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

	pthread_mutex_lock(&LOCK);
	outFile << "| o-> }= ={     | ARRIVAL  : Vehicle " << id << ", traveling in direction " << direc << ".\n";
	pthread_mutex_unlock(&LOCK);

	while (bridge.car_threads[id - 1].waiting){
		//pthread_mutex_lock(&LOCK);
		//outFile << "|   o }= ={     | WAITING  : Vehicle " << id << ", traveling in direction " << direc << ".\n";
		//pthread_mutex_unlock(&LOCK);
	}
}

// CrossBridge() - Executed by OneVehicle(). Used for debugging/checking if vehicles correctly cross bridge.
void CrossBridge(int id, int direc)
{
	vehicle thisCar(id, direc);

	pthread_mutex_lock(&LOCK);
	bridge.bridge_queue.push(thisCar);
	if (DEBUG)
		outFile << "|     }=o={     | CROSSING : Vehicle " << id << ", traveling in direction " << direc << ".\n";
	pthread_mutex_unlock(&LOCK);
}

// ExitBridge() - Called when vechicle crosses bridge and performs operations to maintain flow of traffic.
void ExitBridge(int id, int direc)
{
	pthread_mutex_lock(&LOCK);
	bridge.bridge_queue.pop();
	outFile << "|     }= ={ o-> | DEPATRUE : Vehicle " << id << ", traveling in direction " << direc << ".\n";
	pthread_mutex_unlock(&LOCK);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTION OF BRIDGE CONTROL ////////////////////////////////////////////////////////////////////////////////////
int main(int argc, const char * argv[])
{
	bool bridgePause = false;


	bridge.bridgeScenario();
	currentDirection = LEFT_SIDE;

	while (!(bridge.left_queue.empty() && bridge.right_queue.empty() && bridge.bridge_queue.empty())) {
		// Check if only one side has exhausted all vehicles or capacity reached.
		if ((currentDirection == LEFT_SIDE && bridge.left_queue.empty()) || bridge.bridge_queue.size() == bridge.BRIDGE_CAPACITY) {
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
			int nextTraveler = 0;
			
			// Get carID of next available nextTraveler.
			if (currentDirection == LEFT_SIDE) {
				nextTraveler = bridge.left_queue.front().carID;
				bridge.left_queue.pop();
			}
			else if (currentDirection == RIGHT_SIDE) {
				nextTraveler = bridge.right_queue.front().carID;
				bridge.right_queue.pop();
			}
			bridge.car_threads[nextTraveler - 1].waiting = false;
		}
	}

	// Join up any running threads
	while (!bridge.car_threads.empty()) {
		(void) pthread_join(bridge.car_threads.back().car_function, NULL);
		bridge.car_threads.pop_back();
	}

	return 0;
}