//FastNV.cpp
//Class file for nonvolatile memory dimm system wrapper

#include "FastNV.h"
#include "Init.h"

using namespace std;

//uint64_t BLOCKS_PER_PLANE;

namespace NVDSim
{
    NVDIMM::NVDIMM(uint id, string deviceFile, string sysFile, string pwd, string trc) :
	dev(deviceFile),
	sys(sysFile),
	cDirectory(pwd)
	{
	//uint i, j;
	systemID = id;
	
	 if (cDirectory.length() > 0)
	 {
		 //ignore the pwd argument if the argument is an absolute path
		 if (dev[0] != '/')
		 {
		 dev = pwd + "/" + dev;
		 }
		 
		if (sys[0] != '/')
		 {
		 sys = pwd + "/" + sys;
		 }
	}
	Init::ReadIniFile(dev, false);
	//Init::ReadIniFile(sys, true);

	 if (!Init::CheckIfAllSet())
	 {
		 exit(-1);
	 }
	
	BLOCKS_PER_PLANE = (uint64_t) VIRTUAL_BLOCKS_PER_PLANE * PBLOCKS_PER_VBLOCK;
	if(LOGGING == 1)
	{
	    PRINT("Logs are being generated");
	}else{
	    PRINT("Logs are not being generated");
	}
	PRINT("\nDevice Information:\n");
	PRINT("Device Type: "<<DEVICE_TYPE);
	PRINT("Size (GB): "<<TOTAL_SIZE/(1024*1024));
	PRINT("Block Size: "<<BLOCK_SIZE);
	PRINT("Plane Size: "<<PLANE_SIZE);
	PRINT("Die Size: "<<DIE_SIZE);
	PRINT("Package Size: "<<PACKAGE_SIZE);
	PRINT("Total Size: "<<TOTAL_SIZE);
	PRINT("Packages/Channels: "<<NUM_PACKAGES);
	PRINT("Page size (KB): "<<NV_PAGE_SIZE);
	PRINT("\nTiming Info:\n");
	PRINT("Read time: "<<READ_TIME);
	PRINT("Write Time: "<<WRITE_TIME);
	PRINT("Erase time: "<<ERASE_TIME);
	PRINT("Channel latency for data: "<<CHANNEL_CYCLE);
	PRINT("Channel width for data: "<<CHANNEL_WIDTH);
	PRINT("Device latency for data: "<<DEVICE_CYCLE);
	PRINT("Device width for data: "<<DEVICE_WIDTH)
	if(USE_EPOCHS == 1)
	{
	    PRINT("Device is using epoch data logging");
	}
	PRINT("Epoch Time: "<<EPOCH_TIME);
	PRINT("");

	  controller= new Controller(this);

	// sanity checks
	

	ReturnReadData= NULL;
	WriteDataDone= NULL;

	epoch_count = 0;
	epoch_cycles = 0;

	numReads= 0;
	numWrites= 0;
	numErases= 0;
	currentClockCycle= 0;

	}

// static allocator for the library interface
NVDIMM *getNVDIMMInstance(uint id, string deviceFile, string sysFile, string pwd, string trc)
{
    return new NVDIMM(id, deviceFile, sysFile, pwd, trc);
}

bool NVDIMM::add(FlashTransaction &trans){
    bool isWrite;
    uint64_t addr;
    if(trans.transactionType == DATA_WRITE)
	isWrite = true;
    else
	isWrite = false;
    addr = trans.address; 
    return addTransaction(isWrite,addr);	
}

bool NVDIMM::addTransaction(bool isWrite, uint64_t addr){
	TransactionType type = isWrite ? DATA_WRITE : DATA_READ;
	FlashTransaction trans = FlashTransaction(type, addr, NULL);
	return controller->addTransaction(trans);
}

string NVDIMM::SetOutputFileName(string tracefilename){
	return "";
}

void NVDIMM::RegisterCallbacks(Callback_t *readCB, Callback_t *writeCB, Callback_v *Power){
	ReturnReadData = readCB;
	CriticalLineDone = NULL;
	WriteDataDone = writeCB;
	ReturnPowerData = Power;
}

void NVDIMM::RegisterCallbacks(Callback_t *readCB,  Callback_t *critLineCB, Callback_t *writeCB, Callback_v *Power)
{
    	ReturnReadData = readCB;
	CriticalLineDone = critLineCB;
	WriteDataDone = writeCB;
	ReturnPowerData = Power;
}

void NVDIMM::update(void){
	//uint i, j;
	
	controller->update();
	controller->step();

	step();

	epoch_cycles++;

//	cout << "NVDIMM successfully updated" << endl;
}

void NVDIMM::powerCallback(void){
}
void NVDIMM::printStats(void) {
    controller->loadNVState();
}
void NVDIMM::saveStats(void) {
    controller->saveNVState();
}
void NVDIMM::saveNVState(string filename) {
    ENABLE_NV_SAVE = 1;
    NV_SAVE_FILE = filename;
    cout << "got to save state in nvdimm \n";
    cout << "save file was" << NV_SAVE_FILE << "\n";
    controller->saveNVState();
}
void NVDIMM::loadNVState(string filename) {
    ENABLE_NV_RESTORE = 1;
    NV_RESTORE_FILE = filename;
    controller->loadNVState();
}

}
