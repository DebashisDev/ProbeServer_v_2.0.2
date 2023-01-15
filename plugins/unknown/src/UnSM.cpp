/*
 * UnSessionManager.cpp
 *
 *  Created on: 15-Aug-2021
 *      Author: Debashis
 */

#include "UnSM.h"

UnSM::UnSM(uint16_t id)
{
	this->_name = "UnSM";
	this->setLogLevel(Log::theLog().level());

	this->instanceId 		= id;
	this->unSMReadyState 	= false;

	this->lastIndex 		= 0;
	this->curIndex 			= 0;
	this->curMin 			= 0;
	this->prevMin 			= 0;
	this->curIndexClnUp 	= 0;
	this->lastIndexClnUp 	= 0;

	unTcpSM					= new UnTcpInterface(this->instanceId);
	unUdpSM					= new UnUdpInterface(this->instanceId);
}

UnSM::~UnSM()
{
	delete(unTcpSM);
	delete(unUdpSM);
}

bool UnSM::isUnknownSMReady()
{ return unSMReadyState; }

void UnSM::run()
{
	curMin = prevMin = Global::CURRENT_MIN;
	curIndexClnUp = lastIndexClnUp = Global::CURRENT_SEC / SMGlobal::SESSION_SCAN_FREQ_SEC;
	lastIndex = curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);
	unSMReadyState = true;

	while(SMGlobal::UNM_SM_RUNNING_STATUS[instanceId])
	{
		usleep(SMGlobal::THREAD_SLEEP_TIME);		// 100ms

		curMin = Global::CURRENT_MIN;
		curIndexClnUp = Global::CURRENT_SEC / SMGlobal::SESSION_SCAN_FREQ_SEC;
		curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

		while(lastIndex != curIndex)
		{
			ProcessQueue(lastIndex);
			lastIndex = READ_NEXT_TIME_INDEX(lastIndex, Global::TIME_INDEX);
		}

		if(curIndexClnUp != lastIndexClnUp)
		{
			unTcpSM->tcpTimeOutClean(false);	// IP Cleanup
			unUdpSM->udpTimeOutClean(false);
			lastIndexClnUp = curIndexClnUp;
		}

		if(curMin != prevMin)
		{
			unUdpSM->dnsTimeOutClean();	// DNS Session Cleanup
			prevMin = curMin;
		}
	} /* End Of (Main) While Loop */

	printf("  UnMapped SM [%02d] Stopped...\n", instanceId);
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void UnSM::ProcessQueue(uint16_t t_index)
{
	for(uint16_t i = 0; i < SMGlobal::NO_OF_INTERFACE_SUPPORT; i++)
		for(uint16_t r = 0; r < SMGlobal::ROUTER_PER_INTERFACE[i]; r++)
		{
			tcpProcessQueue_sm(smStore::unTcpSmBusy[instanceId][i][r][t_index], smStore::unTcpSmCnt[instanceId][i][r][t_index],smStore::unTcpSmStore[instanceId][i][r][t_index]);
			udpProcessQueue_sm(smStore::unUdpSmBusy[instanceId][i][r][t_index], smStore::unUdpSmCnt[instanceId][i][r][t_index],smStore::unUdpSmStore[instanceId][i][r][t_index]);
		}
}

void UnSM::tcpProcessQueue_sm(bool &smBusy, uint32_t &smCnt, std::unordered_map<uint32_t, TcpPacket> &pkt)
{
	uint32_t recCnt = smCnt;

	SMGlobal::UNM_SM_PKT_RECEIVED[instanceId] += recCnt;

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

void UnSM::tcpProcessPacket(TcpPacket pkt)
{
	if(&pkt == NULL)
		return;

	unTcpSM->TCPPacketEntry(&pkt);
}

void UnSM::udpProcessQueue_sm(bool &smBusy, uint32_t &smCnt, std::unordered_map<uint32_t, UdpPacket> &pkt)
{
	uint32_t recCnt = smCnt;

	SMGlobal::UNM_SM_PKT_RECEIVED[instanceId] += smCnt;

	if(recCnt > 0)
	{
		smBusy = true;
		for (auto it = pkt.cbegin(), next_it = it; it != pkt.cend(); it = next_it)
		{
			udpProcessPacket(it->second);
			++next_it;
			pkt.erase(it);
			smCnt --;
		}
		smCnt = 0;
		smBusy = false;
	}
}


void UnSM::udpProcessPacket(UdpPacket pkt)
{
	if(&pkt == NULL)
		return;

	unUdpSM->UDPPacketEntry(&pkt);
}


//void UnSM::tcpProcessQueue_sm(bool &smBusy, uint32_t &smCnt, std::unordered_map<uint32_t, TcpPacket> &pkt)
//{
//	uint32_t recCnt = smCnt;
//
//	SMGlobal::UNM_SM_PKT_RECEIVED[instanceId] += recCnt;
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


//void UnSM::udpProcessQueue_sm(bool &smBusy, uint32_t &smCnt, std::unordered_map<uint32_t, UdpPacket> &pkt)
//{
//	uint32_t recCnt = smCnt;
//
//	SMGlobal::UNM_SM_PKT_RECEIVED[instanceId] += smCnt;
//
//	if(recCnt > 0)
//	{
//		smBusy = true;
//		for(uint32_t i = 0; i < recCnt; i++)
//		{
//			udpProcessPacket(&pkt[i]);
//			pkt.erase(i);
//		}
//		pkt.clear();
//		smCnt = 0;
//		smBusy = false;
//	}
//}
