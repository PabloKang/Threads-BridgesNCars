#include "Bridge.h"


// GLOBAL BRIDGE OBJECT:
Bridge bridge;


Bridge::Bridge()
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

		// Fill both Queues with threads of OneVehicle, passed car information from input file.
		for (int i = 0; i < carCount; i++){
			int carID;
			int carDir;

			inFile >> carID;
			inFile >> carDir;

			pthread_t carThread;
			func_params* carData = new func_params(carID, carDir);

			pthread_create(&carThread, NULL, OneVehicle, (void*)carData);

			if (carDir == LEFT_SIDE) {
				travelingLeftQueue.push(carThread);
			}
			else if (carDir == RIGHT_SIDE) {
				travelingRightQueue.push(carThread);
			}
			else {
				cout << "ERROR >> Invalid direction of travel across bridge.\n";
			}
		}
		inFile.close();

		// Set current traveler on bridge to nobody.
		traveler = 0;
	}
}


Bridge::~Bridge()
{

}




// VEHICLE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////

// OneVehicle() - Represents one vehicle, and executes the three functions below it.
void* OneVehicle(void* params){
	func_params* p = (func_params*) params;
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
	bridge.traveler = 0;
}


// MAIN FUNCTION OF BRIDGE CONTROL ///////////////////////////////////////////
int main(int argc, const char * argv[])
{
	bridge = Bridge();
	
	int currentDirection = 0;

	while (!bridge.travelingLeftQueue.empty() || !bridge.travelingRightQueue.empty()) {
		if (currentDirection == 0 && bridge.travelingLeftQueue.empty())
			currentDirection = 1;
		else if (currentDirection == 1 && bridge.travelingRightQueue.empty())
			currentDirection = 0;

		
	}

	return 0;
}