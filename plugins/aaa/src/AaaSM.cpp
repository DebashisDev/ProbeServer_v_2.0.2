/*
 * AaaSessionManager.cpp
 *
 *  Created on: 19 Mar 2021
 *      Author: Debashis
 */

#include "AaaSM.h"

void AaaSM::lockAAAMap()
{
	pthread_mutex_lock(&mapAAALock::lockCount);
	while (mapAAALock::count == 0)
		pthread_cond_wait(&mapAAALock::nonzero, &mapAAALock::lockCount);
	mapAAALock::count = mapAAALock::count - 1;
	pthread_mutex_unlock(&mapAAALock::lockCount);
}

void AaaSM::unLockAAAMap()
{
    pthread_mutex_lock(&mapAAALock::lockCount);
    if (mapAAALock::count == 0)
        pthread_cond_signal(&mapAAALock::nonzero);
    mapAAALock::count = mapAAALock::count + 1;
    pthread_mutex_unlock(&mapAAALock::lockCount);
}

AaaSM::AaaSM(uint16_t id)
{
	this->_name = "AaaSM";
	this->setLogLevel(Log::theLog().level());

	this->instanceId 	  = id;
	this->lastIndex 	  = 0;
	this->curIndex 		  = 0;
	this->curIndexClnUp   = 0;
	this->lastIndexClnUp  = 0;
	this->minCount		  = 0;
	this->aaaSMReadyState = false;
	this->aaaSMInterface  = new AaaSMInterface(this->instanceId);

#if 0
	if(this->instanceId == 0)
	{ aaaSMInterface->aaaLoadUserInfo(); }
#endif
}

AaaSM::~AaaSM()
{ delete(aaaSMInterface); }

bool AaaSM::isAaaSMReady()
{ return aaaSMReadyState; }


void AaaSM::run()
{
	curIndexClnUp = lastIndexClnUp = Global::CURRENT_SEC / SMGlobal::SESSION_SCAN_FREQ_SEC;
	lastIndex = curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);
	aaaSMReadyState = true;

	uint16_t curHour = 0, prevHour = 0;
	uint16_t curSec = 0, prevSec = 0;

	curSec  = prevSec  = Global::CURRENT_SEC;
	curHour = prevHour = Global::CURRENT_HOUR;

	while(SMGlobal::AAA_SM_RUNNING_STATUS[instanceId])
	{
		usleep(SMGlobal::THREAD_SLEEP_TIME);		// 100ms

		curSec  = Global::CURRENT_SEC;
		curHour = Global::CURRENT_HOUR;

		curIndexClnUp = Global::CURRENT_SEC / SMGlobal::SESSION_SCAN_FREQ_SEC;
		curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

		while(lastIndex != curIndex)
		{
			aaaProcessQueue(lastIndex);
			lastIndex = READ_NEXT_TIME_INDEX(lastIndex, Global::TIME_INDEX);
		}

		if(curIndexClnUp != lastIndexClnUp)
		{
			aaaSMInterface->timeOutCleanSession();	// IP Cleanup
			lastIndexClnUp = curIndexClnUp;
		}

		/* Logging AAA Global and Local Session Count (Second wise Job)*/

		if(curSec != prevSec)
		{
			minCount++;
			if(minCount >= SMGlobal::LOG_STATS_FREQ_SEC)
			{
				if(instanceId == 0)
				{ aaaSMInterface->glbSessionCount(); }
				minCount = 0;
			}
			prevSec = curSec;
		}

#if 0
		// Every Hour jobs now
		if(prevHour != curHour)
		{
			if(instanceId == 0)
			{
				lockAAAMap();
				aaaSMInterface->aaaDumpUserInfo(SMGlobal::CURRENT_HOUR);	// User IPv4 Map Dump
				unLockAAAMap();
			}
			prevHour = curHour;
		} /* End of Hours Job */
#endif

	} /* End Of (Main) While Loop */

	printf("  Aaa SM [%02d] Stopped...\n", instanceId);
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void AaaSM::aaaProcessQueue(uint16_t timeIndex)
{
	for(uint16_t i = 0; i < SMGlobal::NO_OF_INTERFACE_SUPPORT; i++)
		for(uint16_t r = 0; r < SMGlobal::ROUTER_PER_INTERFACE[i]; r++)
			processQueue(smStore::aaaSmBusy[instanceId][i][r][timeIndex], smStore::aaaSmCnt[instanceId][i][r][timeIndex], smStore::aaaSmStore[instanceId][i][r][timeIndex]);
}

void AaaSM::processQueue(bool &smBusy, uint32_t &smCnt, std::unordered_map<uint32_t, AAAPacket> &pkt)
{
	uint32_t recCnt = smCnt;

	SMGlobal::AAA_SM_PKT_RECEIVED[instanceId] += recCnt;

	if(recCnt > 0)
	{
		smBusy = true;

		for (auto it = pkt.cbegin(), next_it = it; it != pkt.cend(); it = next_it)
		{
			aaaProcessPacket(it->second);
			++next_it;
			pkt.erase(it);
			smCnt --;
		}
		smCnt = 0;
		smBusy = false;
	}
}

void AaaSM::aaaProcessPacket(AAAPacket pkt)
{
	if(&pkt == NULL)
		return;

	aaaSMInterface->packetEntry(&pkt);
}

//void AaaSM::processQueue(bool &smBusy, uint32_t &smCnt, std::unordered_map<uint32_t, AAAPacket> &pkt)
//{
//	uint32_t recCnt = smCnt;
//
//	SMGlobal::AAA_SM_PKT_RECEIVED[instanceId] += recCnt;
//
//	if(recCnt > 0)
//	{
//		smBusy = true;
//
//		for(uint32_t i = 0; i < recCnt; i++)
//		{
//			aaaProcessPacket(&pkt[i]);
//			pkt.erase(i);
//		}
//		pkt.clear();
//		smCnt = 0;
//		smBusy = false;
//	}
//}
