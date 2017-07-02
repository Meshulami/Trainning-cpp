#include "Simulation.h"

int main()
{
	createMap(MyEdge::TTL);
	createSimulation(50,30,0.75);
	return 0;
}