#include "Bridge.h"


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

		for (int i = 0; i < carCount; i++){
			int carID;
			int carDir;

			inFile >> carID;
			inFile >> carDir;
			
			func_params carData = func_params(carID, carDir);

			if (carDir == LEFT_SIDE) {
				travelingLeftQueue.push(thread(OneVehicle, carData));
			}
			else if (carDir == RIGHT_SIDE) {
				travelingRightQueue.push(thread(OneVehicle, carData));
			}
			else {
				cout << "ERROR >> Invalid direction of travel across bridge.\n";
			}
		}
		inFile.close();
	}
}


Bridge::~Bridge()
{

}


void Bridge::OneVehicle(int id, int direc){
	ArriveBridge(id, direc);
	CrossBridge(id, direc);
	ExitBridge(id, direc);
}


void Bridge::ArriveBridge(int id, int direc)
{

}


void Bridge::CrossBridge(int id, int direc)
{

}


void Bridge::ExitBridge(int id, int direc)
{

}


// MAIN FUNCTION OF BRIDGE CONTROL ///////////////////////////////////////////
int main(int argc, const char * argv[])
{

	return 0;
}