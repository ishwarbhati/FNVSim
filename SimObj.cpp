//SimObj.cpp
//
//Base class for all classes in the simulator
//	
//Code copied from DRAMSim

#include <cstdlib>
#include "SimObj.h"

using namespace NVDSim;
using namespace std;

void SimObj::step()
{
	currentClockCycle++;
}


