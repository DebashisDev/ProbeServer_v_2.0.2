/*
 * DNSFlusher.cpp
 *
 *  Created on: 17-Sep-2019
 *      Author: Debashis
 */

#include "DnsFlusher.h"

DnsFlusher::DnsFlusher(uint16_t id)
{
	_thisLogLevel = 0;
	this->_name = "DNSFlusher";
	this->setLogLevel(Log::theLog().level());

	this->readyFlag 		= false;
	this->instanceId 		= id;
	this->lastIndex 		= 0;
	this->curIndex 			= 0;
	this->curMin		 	= 0;
	this->prevMin		  	= 0;
	this->totalCnt		  	= 0;
	this->pFlUtility 		= new FUtility();
}

DnsFlusher::~DnsFlusher()
{ delete(this->pFlUtility); }

bool DnsFlusher::isDnsFlusherReady()
{ return readyFlag; }

void DnsFlusher::run()
{
	readyFlag = true;
	lastIndex = curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC ,Global::TIME_INDEX);

	curMin = prevMin = Global::CURRENT_MIN;
	openDnsXdrFile(Global::CURRENT_MIN, Global::CURRENT_HOUR, Global::CURRENT_DAY, Global::CURRENT_MONTH, Global::CURRENT_YEAR);

	while(SMGlobal::DNS_FLUSHER_RUNNING_STATUS[this->instanceId])
	{
		usleep(SMGlobal::THREAD_SLEEP_TIME);

		curMin = Global::CURRENT_MIN;

		if(curMin != prevMin)
		{
			closeDnsXdrFile();
			prevMin = curMin;
			openDnsXdrFile(Global::CURRENT_MIN, Global::CURRENT_HOUR, Global::CURRENT_DAY, Global::CURRENT_MONTH, Global::CURRENT_YEAR);
		}

		curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

		while(lastIndex != curIndex)
		{
			strcpy(dnsXdr, "");
			processDnsData(lastIndex);
			lastIndex = READ_NEXT_TIME_INDEX(lastIndex, Global::TIME_INDEX);
		}
	}
	printf("  Dns Flusher [%02d] Stopped... \n", this->instanceId);
	pthread_detach(pthread_self());
	pthread_exit(NULL);

}

void DnsFlusher::processDnsData(uint16_t idx)
{
	for(uint16_t sm = 0; sm < SMGlobal::UDP_SM_INSTANCES; sm++)
		flushDnsData(FlushRepo::dnsFlCnt[instanceId][sm][idx], FlushRepo::dnsFlStore[instanceId][sm][idx]);
}

void DnsFlusher::flushDnsData(uint32_t &flCnt, std::unordered_map<uint32_t, dnsSession> &pkt)
{
	totalCnt = flCnt;

	if(totalCnt > 0)
	{
		for (auto it = pkt.cbegin(), next_it = it; it != pkt.cend(); it = next_it)
		{
			if(createDnsXdrData(it->second))
				xdrDnsHandler << std::string(dnsXdr) << endl;

			++next_it;
			pkt.erase(it);
			flCnt --;
		}
		flCnt = 0;
	}
}

bool DnsFlusher::createDnsXdrData(dnsSession pDnsSession)
{
	if(&pDnsSession == NULL)
		return false;

	dnsXdr[0] = 0;
	pFlUtility->buildDnsXdr(&pDnsSession, dnsXdr);

	if(strlen(dnsXdr) <= 0)
		return false;
	else
		return true;
}

void DnsFlusher::openDnsXdrFile(uint16_t min, uint16_t hour, uint16_t day, uint16_t month, uint16_t year)
{
	char filePath[300];
	filePath[0] = 0;

	sprintf(filePath, "%s%s/%s_%d-%02d-%02d-%02d-%02d_%d.csv",
					SMGlobal::XDR_DIR.c_str(),
					SMGlobal::DNS_FILE_PREFIX.c_str(),
					SMGlobal::DNS_FILE_PREFIX.c_str(),
					year,
					month,
					day,
					hour,
					min,
					this->instanceId);
	xdrDnsHandler.open((char *)filePath, ios :: out | ios :: app);
	filePath[0] = 0;
}

void DnsFlusher::closeDnsXdrFile()
{ xdrDnsHandler.close(); }

//void DnsFlusher::flushDnsData(uint32_t &flCnt, std::unordered_map<uint32_t, dnsSession> &pkt)
//{
//	dnsSession fLocal;
//	totalCnt = flCnt;
//
//	if(totalCnt > 0)
//	{
//		std::list<std::unordered_map<uint32_t, dnsSession>::const_iterator> iterators;
//
//		for (auto it = pkt.cbegin(); it != pkt.cend(); it++)
//		{
//			fLocal.reset();
//			fLocal = it->second;
//
//			if(createDnsXdrData(&fLocal))
//			{
//				xdrDnsHandler << std::string(dnsXdr) << endl;
//				iterators.push_back(it);
//			}
//		}
//
//		flCnt = 0;
//
//		for (auto it: iterators)
//			pkt.erase(it);
//
//	}
//}
//
//void DnsFlusher::flushDnsData(uint32_t &flCnt, std::unordered_map<uint32_t, dnsSession> &pkt)
//{
//	totalCnt = flCnt;
//
//	if(totalCnt > 0)
//	{
//		for(uint32_t cnt = 0; cnt < totalCnt; cnt++)
//		{
//			if(createDnsXdrData(&pkt[cnt]))
//			{
//				xdrDnsHandler << std::string(dnsXdr) << endl;
//			}
//			pkt.erase(cnt);
//			flCnt--;
//		}
//		pkt.clear();
//	}
//	flCnt = 0;
//}
