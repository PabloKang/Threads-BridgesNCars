#include <queue>
#include <vector>
#include <thread>


#define LEFT_SIDE	0
#define RIGHT_SIDE	1


class Bridge
{
	std::queue<std::thread> leftCarQueue;
	std::queue<std::thread> rightCarQueue;

public:
	// Constructor
	Bridge();
	// Deconstructor
	~Bridge();

	// OneVehicle() - Represents one vehicle, and executes the three functions below it.
	void OneVehicle(int id, int direc);

	// ArriveBridge() - Executed by OneVehicle(). Does not return until it is safe for this "vehicle" to cross the bridge.
	void ArriveBridge(int id, int direc);
	// CrossBridge() - Executed by OneVehicle(). Used for debugging/checking if vehicles correctly cross bridge.
	void CrossBridge(int id, int direc);
	// ExitBridge() - Called when vechicle crosses bridge and performs operations to maintain flow of traffic.
	void ExitBridge(int id, int direc);
};

