//Controller.cpp
//Class files for controller

#include "Controller.h"
#include "FastNV.h"
#include <cmath>

using namespace NVDSim;

Controller::Controller(NVDIMM* parent){
	parentNVDIMM = parent;
	readQueue = list<std::pair <FlashTransaction,uint64_t>>();
	writeQueue = list<std::pair <FlashTransaction,uint64_t>>();

	currentClockCycle = 0;
	savefile.open("FastNV.log", ios_base::out | ios_base::trunc);
	if (!savefile)
	{
	    ERROR("Cannot open FastNV.log");
	    exit(-1);
	}
	CHANNEL_TIME = (divide_params((NV_PAGE_SIZE*8192),DEVICE_WIDTH) * DEVICE_CYCLE) / CYCLE_TIME;
	savefile << "CHANNEL_TIME: " << CHANNEL_TIME << endl; 
	TOTAL_RD_LAT = CHANNEL_TIME + READ_TIME;
	TOTAL_WR_LAT = CHANNEL_TIME + WRITE_TIME;
	addressMap = std::unordered_map<uint64_t, bool>();
	loaded = false;


}


void Controller::returnReadData(const FlashTransaction  &trans){
	if(parentNVDIMM->ReturnReadData!=NULL){
	    (*parentNVDIMM->ReturnReadData)(parentNVDIMM->systemID, trans.address, currentClockCycle, true);
	}
	parentNVDIMM->numReads++;
}

void Controller::returnUnmappedData(const FlashTransaction  &trans){
	if(parentNVDIMM->ReturnReadData!=NULL){
	    (*parentNVDIMM->ReturnReadData)(parentNVDIMM->systemID, trans.address, currentClockCycle, false);
	}
	parentNVDIMM->numReads++;
}

void Controller::returnPowerData(vector<double> idle_energy, vector<double> access_energy, vector<double> erase_energy,
		vector<double> vpp_idle_energy, vector<double> vpp_access_energy, vector<double> vpp_erase_energy) {
	if(parentNVDIMM->ReturnPowerData!=NULL){
		vector<vector<double>> power_data = vector<vector<double>>(6, vector<double>(NUM_PACKAGES, 0.0));
		for(uint i = 0; i < NUM_PACKAGES; i++)
		{
			power_data[0][i] = idle_energy[i] * VCC;
			power_data[1][i] = access_energy[i] * VCC;
			power_data[2][i] = erase_energy[i] * VCC;
			power_data[3][i] = vpp_idle_energy[i] * VPP;
			power_data[4][i] = vpp_access_energy[i] * VPP;
			power_data[5][i] = vpp_erase_energy[i] * VPP;
		}
		(*parentNVDIMM->ReturnPowerData)(parentNVDIMM->systemID, power_data, currentClockCycle, false);
	}
}

void Controller::returnPowerData(vector<double> idle_energy, vector<double> access_energy, vector<double> vpp_idle_energy,
		vector<double> vpp_access_energy) {
	if(parentNVDIMM->ReturnPowerData!=NULL){
		vector<vector<double>> power_data = vector<vector<double>>(4, vector<double>(NUM_PACKAGES, 0.0));
		for(uint i = 0; i < NUM_PACKAGES; i++)
		{
			power_data[0][i] = idle_energy[i] * VCC;
			power_data[1][i] = access_energy[i] * VCC;
			power_data[2][i] = vpp_idle_energy[i] * VPP;
			power_data[3][i] = vpp_access_energy[i] * VPP;
		}
		(*parentNVDIMM->ReturnPowerData)(parentNVDIMM->systemID, power_data, currentClockCycle, false);
	}
}

void Controller::returnPowerData(vector<double> idle_energy, vector<double> access_energy, vector<double> erase_energy) {
	if(parentNVDIMM->ReturnPowerData!=NULL){
		vector<vector<double>> power_data = vector<vector<double>>(3, vector<double>(NUM_PACKAGES, 0.0));
		for(uint i = 0; i < NUM_PACKAGES; i++)
		{
			power_data[0][i] = idle_energy[i] * VCC;
			power_data[1][i] = access_energy[i] * VCC;
			power_data[2][i] = erase_energy[i] * VCC;
		}
		(*parentNVDIMM->ReturnPowerData)(parentNVDIMM->systemID, power_data, currentClockCycle, false);
	}
}

void Controller::returnPowerData(vector<double> idle_energy, vector<double> access_energy) {
	if(parentNVDIMM->ReturnPowerData!=NULL){
		vector<vector<double>> power_data = vector<vector<double>>(2, vector<double>(NUM_PACKAGES, 0.0));
		for(uint i = 0; i < NUM_PACKAGES; i++)
		{
			power_data[0][i] = idle_energy[i] * VCC;
			power_data[1][i] = access_energy[i] * VCC;
		}
		(*parentNVDIMM->ReturnPowerData)(parentNVDIMM->systemID, power_data, currentClockCycle, false);
	}
}

void Controller::update(void){
    FlashTransaction trans;
    list<pair <FlashTransaction, uint64_t>>::iterator wr_it,wr_it_tmp;
    list<pair <FlashTransaction, uint64_t>>::iterator rd_it,rd_it_tmp;
    for (wr_it = writeQueue.begin(); wr_it != writeQueue.end(); )
    {
	if((*wr_it).second == TOTAL_WR_LAT) {
	    trans.address = ((*wr_it).first).address;
	    trans.transactionType = ((*wr_it).first).transactionType;
	    if (parentNVDIMM->WriteDataDone != NULL){
		(*parentNVDIMM->WriteDataDone)(parentNVDIMM->systemID, trans.address, currentClockCycle,true);
		parentNVDIMM->numWrites++;
		
	    }
	    wr_it_tmp = wr_it;
	    wr_it++;
	    writeQueue.erase(wr_it_tmp);
	    wr_it = writeQueue.begin();
	    if(LOGGING == true)
	    {
		savefile << "WRITE Completed, Address: "<< trans.address << " Cycle No: " << currentClockCycle << endl;
	    }

	}
	else {
	(*wr_it).second++;
	wr_it++;
	}
    }	

    for (rd_it = readQueue.begin(); rd_it != readQueue.end(); )
    {
	if((*rd_it).second == TOTAL_RD_LAT) {
	    trans.address = ((*rd_it).first).address;
	    trans.transactionType = ((*rd_it).first).transactionType;
	    returnReadData(trans);
	    rd_it_tmp = rd_it;
	    rd_it++;
	    readQueue.erase(rd_it_tmp);
	    if(LOGGING == true)
	    {
		savefile << "READ Completed, Address: "<< trans.address << " Cycle No: " << currentClockCycle << endl;
	    }
	}
	else {
	(*rd_it).second++;
	rd_it++;
	}
    }
}
bool Controller::addTransaction(FlashTransaction &t){
    if(t.transactionType == DATA_READ) {
	if (addressMap.find(t.address) == addressMap.end()) {
	    readQueue.push_back(pair <FlashTransaction, uint64_t>(t, (TOTAL_RD_LAT - LOOKUP_TIME)));
	    savefile << "Unmapped Read, address: " << t.address << endl;
	}
	else {
	    readQueue.push_back(pair <FlashTransaction, uint64_t>(t, 0));
	}
    }
    else if(t.transactionType == DATA_WRITE)
    {
	addressMap[t.address] = true;
	writeQueue.push_back(pair <FlashTransaction, uint64_t>(t, 0));
    }
    if(LOGGING == true)
    {
	savefile << "Transaction added, Type: " << t.transactionType << " Address: "<< t.address << 
	    " Cycle No: " << currentClockCycle << endl;
    }
    return true;
}

void Controller::saveNVState(void)
{

	if(ENABLE_NV_SAVE)
	{
		ofstream save_file;
		save_file.open(NV_SAVE_FILE, ios_base::out | ios_base::trunc);
		if(!save_file)
		{
			cout << "ERROR: Could not open NVDIMM state save file: " << NV_SAVE_FILE << "\n";
			abort();
		}

		cout << "FastNV is saving the used address map \n";
		cout << "save file is" << NV_SAVE_FILE << "\n";

		// save the address map
		save_file << "AddressMap \n";
		std::unordered_map<uint64_t, bool>::iterator it;
		for (it = addressMap.begin(); it != addressMap.end(); it++)
		{
			save_file << (*it).first << " " << (*it).second << " \n";
		}
	}

}
void Controller::loadNVState(void)
{

	if(ENABLE_NV_RESTORE && !loaded)
	{
		ifstream restore_file;
		restore_file.open(NV_RESTORE_FILE);
		if(!restore_file)
		{
			cout << "ERROR: Could not open FastNV restore file: " << NV_RESTORE_FILE << "\n";
			abort();
		}

		cout << "FastNV is restoring the system from file " << NV_RESTORE_FILE <<"\n";

		// restore the data
		uint doing_used = 0;
		uint doing_addresses = 0;
		uint first = 0;
		uint64_t key;

		std::string temp;

		while(!restore_file.eof())
		{ 
			restore_file >> temp;

			// restore used data
			// have the row check cause eof sux

			if(temp.compare("Used") == 0 || temp.compare("Dirty") == 0)
			{
				doing_used = 1;
				doing_addresses = 0;
			}

			// restore address map data
			if(doing_addresses == 1)
			{
				if(first == 0)
				{
					first = 1;
					//savefile << "mapped Address: " << temp << endl;
					key = convert_uint64_t(temp);
				}
				else
				{
					addressMap[key] = true;
					first = 0;
				}
			}

			if(temp.compare("AddressMap") == 0)
			{
				doing_addresses = 1;
			}
		}

		restore_file.close();
		loaded = true;
	}
	//std::unordered_map<uint64_t, bool>::iterator it;
	//for (it = addressMap.begin(); it != addressMap.end(); it++)
	//{
	//	//savefile << "address map:  " (*it).first << " " << (*it).second << endl;
	//	savefile << (*it).first << " " << (*it).second << " \n";
	//}

}
