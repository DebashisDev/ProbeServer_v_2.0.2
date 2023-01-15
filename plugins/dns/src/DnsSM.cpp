/*
 * DnsSM.cpp
 *
 *  Created on: 29 Sep 2021
 *      Author: Debashis
 */

#include "DnsSM.h"

DnsSM::DnsSM(uint16_t id)
{
	this->_name = "DnsSM";
	this->setLogLevel(Log::theLog().level());

	this->instanceId 		= id;
	this->initStats 		= false;
	this->lastTidx			= 0;
	this->curTidx			= 0;
	this->curIndexClnUp		= 0;
	this->lastIndexClnUp	= 0;
	this->pDnsSMInterface	= new DnsSMInterface(this->instanceId);
}

DnsSM::~DnsSM()
{
	delete (pDnsSMInterface);
}

bool DnsSM::isInitialized()
{ return initStats; }

void DnsSM::run()
{
	curIndexClnUp = lastIndexClnUp 	= Global::CURRENT_SEC / SMGlobal::SESSION_SCAN_FREQ_SEC;
	lastTidx = curTidx = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

	initStats = true;

	while(SMGlobal::DNS_SM_RUNNING_STATUS[instanceId])
	{
		usleep(SMGlobal::THREAD_SLEEP_TIME);

		curIndexClnUp = Global::CURRENT_SEC / SMGlobal::SESSION_SCAN_FREQ_SEC;
		curTidx = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

		while(lastTidx != curTidx)
		{
			processQueue(lastTidx);
			lastTidx = READ_NEXT_TIME_INDEX(lastTidx, Global::TIME_INDEX);
		}

		if(curIndexClnUp != lastIndexClnUp) /* Every Second jobs */
		{
			pDnsSMInterface->sessionTimeOutClean();
			pDnsSMInterface->dnsIpV4LookUpCount();
			lastIndexClnUp = curIndexClnUp;
		}

	} /* End Of (Main) While Loop */

	printf("  Dns SM [%02d] Stopped...\n", instanceId);
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void DnsSM::processQueue(uint16_t t_index)
{
	for(uint16_t i = 0; i < SMGlobal::NO_OF_INTERFACE_SUPPORT; i++)
		for(uint16_t k = 0; k < SMGlobal::ROUTER_PER_INTERFACE[i]; k++)
			processPackets(smStore::dnsSmBusy[instanceId][i][k][t_index], smStore::dnsSmCnt[instanceId][i][k][t_index],smStore::dnsSmStore[instanceId][i][k][t_index]);
}

void DnsSM::processPackets(bool &busy, uint32_t &smCnt, std::unordered_map<uint32_t, DnsPacket> &pkt)
{
	uint32_t recCnt = smCnt;

	SMGlobal::DNS_SM_PKT_RECEIVED[instanceId] += recCnt;

	if(recCnt > 0)
	{
		busy = true;

		for (auto it = pkt.cbegin(), next_it = it; it != pkt.cend(); it = next_it)
		{
			callInterface(it->second);
			++next_it;
			pkt.erase(it);
			smCnt --;
		}

		smCnt = 0;
		busy = false;
	}
	recCnt 	= 0;
}

void DnsSM::callInterface(DnsPacket dnsObj)
{
	pDnsSMInterface->DnsPacketEntry(&dnsObj);
}

//void DnsSM::processPackets(bool &busy, uint32_t &cnt, std::unordered_map<uint32_t, UdpPacket> &store)
//{
//	uint32_t recCnt = cnt;
//
//	SMGlobal::DNS_SM_PKT_RECEIVED[instanceId] += recCnt;
//
//	if(recCnt > 0)
//	{
//		busy = true;
//
//		for(uint32_t i = 0; i < recCnt; i++)
//		{
//			callInterface(&store[i]);
//			store.erase(i);
//		}
//		store.clear();
//		cnt = 0;
//		busy = false;
//	}
//	recCnt 	= 0;
//}
