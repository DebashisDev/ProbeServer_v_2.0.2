/*
 * IPSessionManager.cpp
 *
 *  Created on: Apr 24, 2017
 *      Author: Debashis
 */

#include "TcpSM.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

TcpSM::TcpSM(uint16_t id)
{
	this->_name = "TcpSM";
	this->setLogLevel(Log::theLog().level());

	this->instanceId 		= id;
	this->tcpSMReadyState 	= false;
	this->lastIndex 		= 0;
	this->curIndex 			= 0;
	this->curIndexClnUp 	= 0;
	this->lastIndexClnUp 	= 0;
	this->tcpSMInterface	= new TcpSMInterface(id);
}

TcpSM::~TcpSM()
{ delete(tcpSMInterface); }

bool TcpSM::isTcpSMReady()
{ return tcpSMReadyState; }

void TcpSM::run()
{
	curIndexClnUp = lastIndexClnUp = Global::CURRENT_SEC / SMGlobal::SESSION_SCAN_FREQ_SEC;
	lastIndex = curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);
	tcpSMReadyState = true;

	while(SMGlobal::TCP_SM_RUNNING_STATUS[instanceId])
	{
		usleep(SMGlobal::THREAD_SLEEP_TIME);		// 100ms

		curIndexClnUp = Global::CURRENT_SEC / SMGlobal::SESSION_SCAN_FREQ_SEC;

		curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

		while(lastIndex != curIndex)
		{
			tcpProcessQueue(lastIndex);
			lastIndex = READ_NEXT_TIME_INDEX(lastIndex, Global::TIME_INDEX);
		}

		if(curIndexClnUp != lastIndexClnUp)
		{
			tcpSMInterface->tcpTimeOutClean(false);	// IP Cleanup
			lastIndexClnUp = curIndexClnUp;
		}

		if(Global::CURRENT_HOUR == SMGlobal::DAY_CLEAN_HOUR && Global::CURRENT_MIN == SMGlobal::DAY_CLEAN_MIN && Global::CURRENT_SEC == SMGlobal::DAY_CLEAN_SEC)
		{
			uint16_t i = 0;

			while(i <= 10)
			{
				curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);
				if(lastIndex != curIndex)
				{
					tcpProcessQueue(lastIndex);
					lastIndex = READ_NEXT_TIME_INDEX(lastIndex, Global::TIME_INDEX);
					i++;
				}
			}
			tcpSMInterface->tcpTimeOutClean(true);
			sleep(60 - SMGlobal::DAY_CLEAN_SEC);
		}
	} /* End Of (Main) While Loop */

	printf("  Tcp SM [%02d] Stopped...\n", instanceId);
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void TcpSM::tcpProcessQueue(uint16_t t_index)
{
	for(uint16_t i = 0; i < SMGlobal::NO_OF_INTERFACE_SUPPORT; i++)
		for(uint16_t r = 0; r < SMGlobal::ROUTER_PER_INTERFACE[i]; r++)
			processQueue_sm(smStore::tcpSmBusy[instanceId][i][r][t_index], smStore::tcpSmCnt[instanceId][i][r][t_index], smStore::tcpSmStore[instanceId][i][r][t_index]);
}

void TcpSM::processQueue_sm(bool &smBusy, uint32_t &smCnt, std::unordered_map<uint32_t, TcpPacket> &pkt)
{
	uint32_t recCnt = smCnt;
	SMGlobal::TCP_SM_PKT_RECEIVED[instanceId] += recCnt;

	if(recCnt > 0)
	{
		smBusy = true;

		for (auto it = pkt.cbegin(), next_it = it; it != pkt.cend(); it = next_it)
		{
			tcpProcessPacket(it->second);
			++next_it;
			pkt.erase(it);
			smCnt --;

		}
		smCnt = 0;
		smBusy = false;
	}
}

void TcpSM::tcpProcessPacket(TcpPacket pkt)
{
	if(&pkt == NULL)
		return;

	tcpSMInterface->TCPPacketEntry(&pkt);
}


//void TcpSM::processQueue_sm(bool &smBusy, uint32_t &smCnt, std::unordered_map<uint32_t, TcpPacket> &pkt)
//{
//	uint32_t recCnt = smCnt;
//	SMGlobal::TCP_SM_PKT_RECEIVED[instanceId] += recCnt;
//
//	if(recCnt > 0)
//	{
//		smBusy = true;
//
//		for(uint32_t i = 0; i < recCnt; i++)
//		{
//			tcpProcessPacket(&pkt[i]);
//			pkt.erase(i);
//		}
//		pkt.clear();
//		smCnt = 0;
//		smBusy = false;
//	}
//}
