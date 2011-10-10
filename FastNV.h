#ifndef FASTNV_H
#define FASTNV_H
//FASTNV.h
//Header for fast nonvolatile memory system wrapper

#include "SimObj.h"
#include "FlashConfiguration.h"
#include "Controller.h"
#include "FlashTransaction.h"
#include "Callbacks.h"

using std::string;

namespace NVDSim{
    typedef CallbackBase<void,uint,uint64_t,uint64_t,bool> Callback_t;
    typedef CallbackBase<void,uint,vector<vector<double>>,uint64_t,bool> Callback_v;
	class NVDIMM : public SimObj{
		public:
			NVDIMM(uint id, string dev, string sys, string pwd, string trc);
			void update(void);
			bool add(FlashTransaction &trans);
			bool addTransaction(bool isWrite, uint64_t addr);
			string SetOutputFileName(string tracefilename);
			void RegisterCallbacks(Callback_t *readDone, Callback_t *writeDone, Callback_v *Power);
			void RegisterCallbacks(Callback_t *readDone, Callback_t *critLine, Callback_t *writeDone, Callback_v *Power); 

			void powerCallback(void);
			void printStats(void);
			void saveStats(void);
			void saveNVState(string filename);
			void loadNVState(string filename);

			Controller *controller;


			Callback_t* ReturnReadData;
			Callback_t* CriticalLineDone;
			Callback_t* WriteDataDone;
			Callback_v* ReturnPowerData;

			uint systemID, numReads, numWrites, numErases;
			uint epoch_count, epoch_cycles;

		private:
			string dev, sys, cDirectory;
	};

	NVDIMM *getNVDIMMInstance(uint id, string deviceFile, string sysFile, string pwd, string trc);
}
#endif
