#ifndef FNVCONTROLLER_H
#define FNVCONTROLLER_H
//Controller.h
//header file for controller
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <list>

#include "SimObj.h"
#include "FlashConfiguration.h"
//#include "Die.h"
//#include "Buffer.h"
//#include "Ftl.h"
//#include "Channel.h"
#include "FlashTransaction.h"
//#include "Logger.h"
#include "Util.h"

namespace NVDSim{

	class NVDIMM;
	class Controller : public SimObj{
		public:
	                Controller(NVDIMM* parent);

		//	void attachPackages(vector<Package> *packages);
			void returnReadData(const FlashTransaction &trans);
			void returnUnmappedData(const FlashTransaction &trans);
			void returnCritLine(const FlashTransaction &trans);
			void returnPowerData(vector<double> idle_energy,
					 vector<double> access_energy,
					 vector<double> erase_energy,
					 vector<double> vpp_idle_energy,
					 vector<double> vpp_access_energy,
					 vector<double> vpp_erase_energy);
			void returnPowerData(vector<double> idle_energy,
					 vector<double> access_energy,
					 vector<double> vpp_idle_energy,
					 vector<double> vpp_access_energy);
			void returnPowerData(vector<double> idle_energy,
					 vector<double> access_energy,
					 vector<double> erase_energy);
			void returnPowerData(vector<double> idle_energy,
					 vector<double> access_energy);
			void update(void);
  
			virtual bool addTransaction(FlashTransaction &t);
			virtual void saveNVState(void);
			virtual void loadNVState(void);


			NVDIMM *parentNVDIMM;

		private:
			std::list<FlashTransaction> returnTransaction;
			std::vector<uint> channelXferCyclesLeft; //cycles per channel beat
			FlashTransaction currentTransaction;
			std::list<std::pair <FlashTransaction, uint64_t>> readQueue;
			std::list<std::pair <FlashTransaction, uint64_t>> writeQueue;
			std::unordered_map<uint64_t,bool> addressMap;
			std::ofstream savefile;
			uint64_t CHANNEL_TIME;
			uint64_t CRIT_LINE_TIME;
			uint64_t TOTAL_RD_LAT;
			uint64_t TOTAL_WR_LAT;
			bool loaded;
	};
}
#endif
