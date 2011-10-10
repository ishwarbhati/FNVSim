#ifndef FNVSIMTRANSACTION_H
#define FNVSIMTRANSACTION_H

//FlashTransaction.h
//
//Header file for transaction object

#include "FlashConfiguration.h"

using namespace std;

namespace NVDSim
{
	enum TransactionType
	{
		DATA_READ, 
		DATA_WRITE,
		GC_DATA,
		GC_DATA_READ,
		GC_DATA_WRITE,
		RETURN_DATA,
		BLOCK_ERASE
	};

	class FlashTransaction
	{
	public:
		//fields
		TransactionType transactionType;
		uint64_t address;
		void *data;
		uint64_t timeAdded;
		uint64_t timeReturned;

		
		//functions
		FlashTransaction(TransactionType transType, uint64_t addr, void *data);
		FlashTransaction();
		
		void print();
	};
}

#endif

