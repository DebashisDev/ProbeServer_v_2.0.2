/*
 * UdpFlusher.cpp
 *
 *  Created on: 18 Mar 2021
 *      Author: Debashis
 */

#include "UdpFlusher.h"

UdpFlusher::UdpFlusher(uint16_t id)
{
	_thisLogLevel = 0;
	this->_name = "UdpFlusher";
	this->setLogLevel(Log::theLog().level());

	this->readyFlag  	= false;
	this->instanceId 	= id;
	this->lastIndex  	= 0;
	this->curIndex 	 	= 0;
	this->curMin		= 0;
	this->prevMin		= 0;
	this->totalCnt		= 0;
	this->pFlUtility  	= new FUtility();
}

UdpFlusher::~UdpFlusher()
{ delete(pFlUtility); }

bool UdpFlusher::isUdpFlusherReady()
{ return readyFlag; }

void UdpFlusher::run()
{
	readyFlag = true;
	lastIndex = curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

	curMin = prevMin = Global::CURRENT_MIN;
	openUdpXdrFile(Global::CURRENT_MIN, Global::CURRENT_HOUR, Global::CURRENT_DAY, Global::CURRENT_MONTH, Global::CURRENT_YEAR);

	while(SMGlobal::UDP_FLUSHER_RUNNING_STATUS[this->instanceId])
	{
		usleep(SMGlobal::THREAD_SLEEP_TIME);

		curMin = Global::CURRENT_MIN;

		if(curMin != prevMin)
		{
			closeUdpXdrFile();
			prevMin = curMin;
			openUdpXdrFile(Global::CURRENT_MIN, Global::CURRENT_HOUR, Global::CURRENT_DAY, Global::CURRENT_MONTH, Global::CURRENT_YEAR);
		}

		curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

		while(lastIndex != curIndex)
		{
			strcpy(udpXdr, "");
			processUdpData(lastIndex);
			lastIndex = READ_NEXT_TIME_INDEX(lastIndex, Global::TIME_INDEX);
		}
	}
	printf("  Udp [%02d] Flusher Stopped...\n", instanceId);
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void UdpFlusher::processUdpData(uint16_t idx)
{
	for(uint16_t sm = 0; sm < SMGlobal::UDP_SM_INSTANCES; sm++)
		flushUdpData(FlushRepo::udpFlCnt[instanceId][sm][idx], FlushRepo::udpFlStore[instanceId][sm][idx]);
}

void UdpFlusher::flushUdpData(uint32_t &flCnt, std::unordered_map<uint32_t, udpSession> &pkt)
{
	totalCnt = flCnt;

	if(totalCnt > 0)
	{
		for (auto it = pkt.cbegin(), next_it = it; it != pkt.cend(); it = next_it)
		{
			if(createUdpXdrData(it->second))
				xdrUdpHandler << std::string(udpXdr) << endl;

			++next_it;
			pkt.erase(it);
			flCnt --;
		}
		flCnt = 0;
	}
}

bool UdpFlusher::createUdpXdrData(udpSession pUdpSession)
{
	if(&pUdpSession == NULL)
		return false;

	udpXdr[0] = 0;
	pFlUtility->buildUdpXdr(&pUdpSession, udpXdr);

	if(strlen(udpXdr) <= 0)
		return false;
	else
		return true;
}

void UdpFlusher::openUdpXdrFile(uint16_t min, uint16_t hour, uint16_t day, uint16_t month, uint16_t year)
{
	char filePath[300];
	filePath[0] = 0;

	sprintf(filePath, "%s%s/%s_%d-%02d-%02d-%02d-%02d_%d.csv",
					SMGlobal::XDR_DIR.c_str(),
					"ip",
					SMGlobal::UDP_FILE_PREFIX.c_str(),
					year,
					month,
					day,
					hour,
					min,
					this->instanceId);
	xdrUdpHandler.open((char *)filePath, ios :: out | ios :: app);

	filePath[0] = 0;
}

void UdpFlusher::closeUdpXdrFile()
{ xdrUdpHandler.close(); }

//void UdpFlusher::flushUdpData(uint32_t &flCnt, std::unordered_map<uint32_t, udpSession> &pkt)
//{
//	udpSession fLocal;
//	totalCnt = flCnt;
//
//	if(totalCnt > 0)
//	{
//		std::list<std::unordered_map<uint32_t, udpSession>::const_iterator> iterators;
//
//		for (auto it = pkt.cbegin(); it != pkt.cend(); it++)
//		{
//			fLocal.reset();
//			fLocal = it->second;
//
//			if(createUdpXdrData(&fLocal))
//			{
//				xdrUdpHandler << std::string(udpXdr) << endl;
//				iterators.push_back(it);
//			}
//		}
//
//		flCnt = 0;
//
//		for (auto it: iterators)
//			pkt.erase(it);
//	}
//}
//
//void UdpFlusher::flushUdpData(uint32_t &flCnt, std::unordered_map<uint32_t, udpSession> &pkt)
//{
//	totalCnt = flCnt;
//
//	if(totalCnt > 0)
//	{
//		for(uint32_t cnt = 0; cnt < totalCnt; cnt++)
//		{
//			if(createUdpXdrData(&pkt[cnt]))
//			{
//				xdrUdpHandler << std::string(udpXdr) << endl;
//			}
//			pkt.erase(cnt);
//			flCnt--;
//		}
//		pkt.clear();
//	}
//	flCnt = 0;
//}

