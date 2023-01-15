/*
 * IPFlusher.cpp
 *
 *  Created on: Apr 24, 2017
 *      Author: Debashis
 */

#include "TcpFlusher.h"

TcpFlusher::TcpFlusher(uint16_t id)
{
	_thisLogLevel = 0;
	this->_name = "TcpFlusher";
	this->setLogLevel(Log::theLog().level());

	this->repoInitStatus  = false;
	this->instanceId 	  = id;
	this->lastIndex 	  = 0;
	this->curIndex 		  = 0;
	this->curMin		  = 0;
	this->prevMin		  = 0;
	this->totalCnt		  = 0;
	this->pFlUtility 	  = new FUtility();
}

TcpFlusher::~TcpFlusher()
{ delete(pFlUtility); }

bool TcpFlusher::isTcpFlusherReady()
{ return repoInitStatus; }

void TcpFlusher::run()
{
	repoInitStatus = true;
	lastIndex = curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

	curMin = prevMin = Global::CURRENT_MIN;
	openTcpXdrFile(Global::CURRENT_MIN, Global::CURRENT_HOUR, Global::CURRENT_DAY, Global::CURRENT_MONTH, Global::CURRENT_YEAR);

	while(SMGlobal::TCP_FLUSHER_RUNNING_STATUS[instanceId])
	{
		usleep(SMGlobal::THREAD_SLEEP_TIME);

		curMin = Global::CURRENT_MIN;

		if(curMin != prevMin)
		{
			closeTcpXdrFile();
			prevMin = curMin;
			openTcpXdrFile(Global::CURRENT_MIN, Global::CURRENT_HOUR, Global::CURRENT_DAY, Global::CURRENT_MONTH, Global::CURRENT_YEAR);
		}

		curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

		while(lastIndex != curIndex)
		{
			strcpy(tcpXdr, "");
			processTcpData(lastIndex);
			lastIndex = READ_NEXT_TIME_INDEX(lastIndex, Global::TIME_INDEX);
		}
	}
	printf("  Tcp [%02d] Flusher Stopped...\n", instanceId);
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void TcpFlusher::processTcpData(uint16_t idx)
{
	for(uint16_t sm = 0; sm < SMGlobal::TCP_SM_INSTANCES; sm++)
		flushTcpData(FlushRepo::tcpFlCnt[instanceId][sm][idx], FlushRepo::tcpFlStore[instanceId][sm][idx]);
}

void TcpFlusher::flushTcpData(uint32_t &flCnt, std::unordered_map<uint32_t, tcpSession> &pkt)
{
	totalCnt = flCnt;

	if(totalCnt > 0)
	{
		for (auto it = pkt.cbegin(), next_it = it; it != pkt.cend(); it = next_it)
		{
			if(createTcpXdrData(it->second))
				xdrTcpHandler << std::string(tcpXdr) << endl;

			++next_it;
			pkt.erase(it);
			flCnt --;
		}
		flCnt = 0;
	}
}

bool TcpFlusher::createTcpXdrData(tcpSession pTcpSession)
{
	if(&pTcpSession == NULL)
			return false;

	tcpXdr[0] = 0;
	pFlUtility->buildTcpXdr(&pTcpSession, tcpXdr);

	if(strlen(tcpXdr) <= 0)
		return false;
	else
		return true;
}

void TcpFlusher::openTcpXdrFile(uint16_t min, uint16_t hour, uint16_t day, uint16_t month, uint16_t year)
{
	char filePath[300];
	filePath[0] = 0;

	sprintf(filePath, "%s%s/%s_%d-%02d-%02d-%02d-%02d_%d.csv",
					SMGlobal::XDR_DIR.c_str(),
					"ip",
					SMGlobal::TCP_FILE_PREFIX.c_str(),
					year,
					month,
					day,
					hour,
					min,
					instanceId);
	xdrTcpHandler.open((char *)filePath, ios :: out | ios :: app);

	filePath[0] = 0;
}

void TcpFlusher::closeTcpXdrFile()
{ xdrTcpHandler.close(); }

//void TcpFlusher::flushTcpData(uint32_t &flCnt, std::unordered_map<uint32_t, tcpSession> &pkt)
//{
//	totalCnt = flCnt;
//
//	if(totalCnt > 0)
//	{
//		for(uint32_t cnt = 0; cnt < totalCnt; cnt++)
//		{
//			if(createTcpXdrData(&pkt[cnt]))
//			{
//				xdrTcpHandler << std::string(tcpXdr) << endl;
//			}
//			pkt.erase(cnt);
//			flCnt--;
//		}
//		pkt.clear();
//	}
//	flCnt = 0;
//}
//void TcpFlusher::flushTcpData(uint32_t &flCnt, std::unordered_map<uint32_t, tcpSession> &pkt)
//{
//	tcpSession fLocal;
//	totalCnt = flCnt;
//
//	if(totalCnt > 0)
//	{
//		std::list<std::unordered_map<uint32_t, tcpSession>::const_iterator> iterators;
//
//		for (auto it = pkt.cbegin(); it != pkt.cend(); it++)
//		{
//			fLocal.reset();
//			fLocal = it->second;
//
//			if(createTcpXdrData(&fLocal))
//			{
//				xdrTcpHandler << std::string(tcpXdr) << endl;
//				iterators.push_back(it);
//			}
//		}
//
//		flCnt = 0;
//		for (auto it: iterators)
//			pkt.erase(it);
//	}
//}

