/*
 * UdpSessionManager.cpp
 *
 *  Created on: 18 Mar 2021
 *      Author: Debashis
 */

#include "UdpSM.h"

UdpSM::UdpSM(uint16_t id)
{
	this->_name = "UdpSM";
	this->setLogLevel(Log::theLog().level());

	this->instanceId 		= id;
	this->udpSMReadyState 	= false;
	this->lastIndex			= 0;
	this->curIndex			= 0;
	this->curIndexClnUp		= 0;
	this->lastIndexClnUp	= 0;

	this->udpSMInterface	= new UdpSMInterface(id);
}

UdpSM::~UdpSM()
{ delete(udpSMInterface); }

bool UdpSM::isUdpSMReady()
{ return udpSMReadyState; }


void UdpSM::run()
{
	curIndexClnUp = lastIndexClnUp = Global::CURRENT_SEC / SMGlobal::SESSION_SCAN_FREQ_SEC;
	lastIndex = curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);
	udpSMReadyState = true;

	while(SMGlobal::UDP_SM_RUNNING_STATUS[instanceId])
	{
		usleep(SMGlobal::THREAD_SLEEP_TIME);		// 100ms

		curIndexClnUp = Global::CURRENT_SEC / SMGlobal::SESSION_SCAN_FREQ_SEC;
		curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

		while(lastIndex != curIndex)
		{
			udpProcessQueue(lastIndex);
			lastIndex = READ_NEXT_TIME_INDEX(lastIndex, Global::TIME_INDEX);
		}

		if(curIndexClnUp != lastIndexClnUp)
		{
			udpSMInterface->udpTimeOutClean(false);	// UDP Session Cleanup
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
					udpProcessQueue(lastIndex);
					lastIndex = READ_NEXT_TIME_INDEX(lastIndex, Global::TIME_INDEX);
					i++;
				}
			}
			udpSMInterface->udpTimeOutClean(true);
			sleep(60 - SMGlobal::DAY_CLEAN_SEC);
		}
	} /* End Of (Main) While Loop */
	printf("  Udp SM [%02d] Stopped...\n", instanceId);
}

void UdpSM::udpProcessQueue(uint16_t t_index)
{
	for(uint16_t i = 0; i < SMGlobal::NO_OF_INTERFACE_SUPPORT; i++)
		for(uint16_t r = 0; r < SMGlobal::ROUTER_PER_INTERFACE[i]; r++)
			processQueue_sm(smStore::udpSmBusy[instanceId][i][r][t_index], smStore::udpSmCnt[instanceId][i][r][t_index],smStore::udpSmStore[instanceId][i][r][t_index]);
}

void UdpSM::processQueue_sm(bool &smBusy, uint32_t &smCnt, std::unordered_map<uint32_t, UdpPacket> &pkt)
{
	uint32_t recCnt = smCnt;

	SMGlobal::UDP_SM_PKT_RECEIVED[instanceId] += recCnt;

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

void UdpSM::udpProcessPacket(UdpPacket pkt)
{
	if(&pkt == NULL)
		return;

	udpSMInterface->UDPPacketEntry(&pkt);
}


//void UdpSM::processQueue_sm(bool &smBudy, uint32_t &smCnt, std::unordered_map<uint32_t, UdpPacket> &pkt)
//{
//	uint32_t recCnt = smCnt;
//
//	SMGlobal::UDP_SM_PKT_RECEIVED[instanceId] += recCnt;
//
//	if(recCnt > 0)
//	{
//		smBudy = true;
//		for(uint32_t i = 0; i < recCnt; i++)
//		{
//			udpProcessPacket(&pkt[i]);
//			pkt.erase(i);
//		}
//		pkt.clear();
//		smCnt = 0;
//		smBudy = false;
//	}
//}
