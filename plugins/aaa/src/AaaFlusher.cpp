/*
 * RadiusFlusher.cpp
 *
 *  Created on: 17-Sep-2019
 *      Author: Debashis
 */

#include "AaaFlusher.h"

AaaFlusher::AaaFlusher(uint16_t id)
{
	_thisLogLevel = 0;
	this->_name = "RadiusFlusher";
	this->setLogLevel(Log::theLog().level());

	this->flusherReadyFlag 	= false;
	this->instance 		 	= id;
	this->lastIndex 		= 0;
	this->curIndex 		 	= 0;
	this->totalCnt		  	= 0;
	this->curMin			= 0;
	this->prevMin			= 0;
	this->pFlUtility 	    = new FUtility();
	this->errorXdr			= true;
}

AaaFlusher::~AaaFlusher()
{ delete(pFlUtility); }

bool AaaFlusher::isAaaFlusherReady()
{ return flusherReadyFlag; }

void AaaFlusher::run()
{
	flusherReadyFlag = true;
	lastIndex = curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

	curMin = prevMin = Global::CURRENT_MIN;
	openAaaXdrFile(Global::CURRENT_MIN, Global::CURRENT_HOUR, Global::CURRENT_DAY, Global::CURRENT_MONTH, Global::CURRENT_YEAR);

	while(SMGlobal::AAA_FLUSHER_RUNNING_STATUS[instance])
	{
		usleep(SMGlobal::THREAD_SLEEP_TIME);

		curMin = Global::CURRENT_MIN;

		if(curMin != prevMin)
		{
			closeAaaXdrFile();
			prevMin = curMin;
			openAaaXdrFile(Global::CURRENT_MIN, Global::CURRENT_HOUR, Global::CURRENT_DAY, Global::CURRENT_MONTH, Global::CURRENT_YEAR);
		}

		curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

		while(lastIndex != curIndex)
		{
			strcpy(aaaXdr, "");
			processAaaData(lastIndex);
			lastIndex = READ_NEXT_TIME_INDEX(lastIndex, Global::TIME_INDEX);
		}
	}
	printf("  Aaa Flusher Stopped...\n");
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void AaaFlusher::processAaaData(uint16_t idx)
{
	for(uint16_t sm = 0; sm < SMGlobal::AAA_SM_INSTANCES; sm++)
		flushAaaData(FlushRepo::aaaFlCnt[instance][sm][idx], FlushRepo::aaaFlStore[instance][sm][idx]);
}

void AaaFlusher::flushAaaData(uint32_t &flCnt, std::unordered_map<uint32_t, aaaSession> &pkt)
{
	totalCnt = flCnt;

	if(totalCnt > 0)
	{
		for (auto it = pkt.cbegin(), next_it = it; it != pkt.cend(); it = next_it)
		{
			if(createAaaXdrData(it->second) && !errorXdr)
				aaaXdrHandler << std::string(aaaXdr) << endl;
			else if(errorXdr)
				aaaErrorXdrHandler << std::string(aaaXdr) << endl;

			++next_it;
			pkt.erase(it);
			flCnt --;
		}
		flCnt = 0;
	}
}

bool AaaFlusher::createAaaXdrData(aaaSession pAaaSession)
{
	if(&pAaaSession == NULL)
			return false;

	errorXdr = false;
	aaaXdr[0] = 0;

	pFlUtility->buildAaaXdr(&pAaaSession, aaaXdr, &errorXdr);

	if(strlen(aaaXdr) <= 0)
		return false;
	else
		return true;
}

void AaaFlusher::openAaaXdrFile(uint16_t min, uint16_t hour, uint16_t day, uint16_t month, uint16_t year)
{
	char filePath[300];
	filePath[0] = 0;

	sprintf(filePath, "%s%s/%s_%d-%02d-%02d-%02d-%02d_%d.csv",
					SMGlobal::XDR_DIR.c_str(),
					SMGlobal::AAA_FILE_PREFIX.c_str(),
					SMGlobal::AAA_FILE_PREFIX.c_str(),
					year,
					month,
					day,
					hour,
					min,
					instance);
	aaaXdrHandler.open((char *)filePath, ios :: out | ios :: app);

	filePath[0] = 0;

	sprintf(filePath, "%sError/%s_%d-%02d-%02d-%02d-%02d_%d.csv",
					SMGlobal::XDR_DIR.c_str(),
					SMGlobal::AAA_FILE_PREFIX.c_str(),
					year,
					month,
					day,
					hour,
					min,
					instance);
	aaaErrorXdrHandler.open((char *)filePath, ios :: out | ios :: app);

	filePath[0] = 0;
}

void AaaFlusher::closeAaaXdrFile()
{
	aaaXdrHandler.close();
	aaaErrorXdrHandler.close();
}

//void AaaFlusher::flushAaaData(uint32_t &flCnt, std::unordered_map<uint32_t, aaaSession> &pkt)
//{
//	aaaSession fLocal;
//	totalCnt = flCnt;
//
//	if(totalCnt > 0)
//	{
//		std::list<std::unordered_map<uint32_t, aaaSession>::const_iterator> iterators;
//
//		for (auto it = pkt.cbegin(); it != pkt.cend(); it++)
//		{
//			fLocal.reset();
//			fLocal = it->second;
//
//			if(createAaaXdrData(&fLocal))
//			{
//				aaaXdrHandler << std::string(aaaXdr) << endl;
//				iterators.push_back(it);
//			}
//		}
//		flCnt = 0;
//
//		for (auto it: iterators)
//			pkt.erase(it);
//	}
//}
//void AaaFlusher::flushAaaData(uint32_t &flCnt, std::unordered_map<uint32_t, aaaSession> &pkt)
//{
//	totalCnt = flCnt;
//
//	if(totalCnt > 0)
//	{
//		for(uint32_t cnt = 0; cnt < totalCnt; cnt++)
//		{
//			if(createAaaXdrData(&pkt[cnt]))
//			{
//				aaaXdrHandler << std::string(aaaXdr) << endl;
//			}
//			pkt.erase(cnt);
//			flCnt--;
//		}
//		pkt.clear();
//	}
//	flCnt = 0;
//}
