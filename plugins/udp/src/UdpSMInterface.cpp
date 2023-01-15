/*
 * UdpSMInterface.cpp
 *
 *  Created on: 18 Mar 2021
 *      Author: Debashis
 */

#include "UdpSMInterface.h"

UdpSMInterface::UdpSMInterface(uint16_t id)
{
	_thisLogLevel = 0;
	this->_name = "UdpSMInterface";
	this->setLogLevel(Log::theLog().level());
	this->instanceId = id;

	this->pFUtility = new FUtility();
	udpInitializeSessionPool();

	sessionCleanCnt = 0;

	ipV4Key = 0;
	ipV6Key = "";
}

UdpSMInterface::~UdpSMInterface()
{ delete(this->pFUtility); }

uint32_t UdpSMInterface::udpGetFreeIndex()
{
	udpFreeBitPos++;
	if(udpFreeBitPos >= udpFreeBitPosMax) udpFreeBitPos = 0;
	int arrId = udpFreeBitPos / UDP_SESSION_POOL_ARRAY_ELEMENTS;
	int bitId = udpFreeBitPos % UDP_SESSION_POOL_ARRAY_ELEMENTS;

	while(udpBitFlagsSession[arrId].test(bitId)){
		udpFreeBitPos++;
		if(udpFreeBitPos >= udpFreeBitPosMax) udpFreeBitPos = 0;
		arrId = udpFreeBitPos / UDP_SESSION_POOL_ARRAY_ELEMENTS;
		bitId = udpFreeBitPos % UDP_SESSION_POOL_ARRAY_ELEMENTS;
	}
	if(udpFreeBitPos >= udpFreeBitPosMax){
		printf("[%d] getFreeIndexIp freeBitPosIp [%u] >= freeBitPosIpMax [%u]\n",instanceId, udpFreeBitPos, udpFreeBitPosMax);
	}
	udpBitFlagsSession[arrId].set(bitId);
	return udpFreeBitPos;
}

void UdpSMInterface::udpReleaseIndex(uint32_t idx)
{
	uint32_t arrId = idx / UDP_SESSION_POOL_ARRAY_ELEMENTS;
	uint32_t bitId = idx % UDP_SESSION_POOL_ARRAY_ELEMENTS;

	udpSessionPoolMap[arrId][bitId]->reset();
	udpSessionPoolMap[arrId][bitId]->poolIndex = idx;
	udpBitFlagsSession[arrId].reset(bitId);
}

void UdpSMInterface::udpInitializeSessionPool()
{
	udpFreeBitPosMax = UDP_SESSION_POOL_ARRAY_ELEMENTS * UDP_SESSION_POOL_ARRAY_SIZE;

	printf("UdpSMInterface [%02d]	Initializing [%u]  UDP Session Pool... ", instanceId, udpFreeBitPosMax);
	TheLog_nc_v2(Log::Info, name(),"     [%02d] Initializing [%u] UDP Session Pool...", instanceId, udpFreeBitPosMax);

	for(uint16_t i = 0; i < UDP_SESSION_POOL_ARRAY_SIZE; i++)
	{
		udpBitFlagsSession[i].reset();
		for(uint16_t j = 0; j < UDP_SESSION_POOL_ARRAY_ELEMENTS; j++)
		{
			udpSessionPoolMap[i][j] = new udpSession();
			udpSessionPoolMap[i][j]->poolIndex = (i*UDP_SESSION_POOL_ARRAY_ELEMENTS) + j;
		}
	}
	printf("Completed.\n");
	TheLog_nc_v2(Log::Info, name(),"     [%d] Initializing [%u] UDP Session Pool Completed.", instanceId, udpFreeBitPosMax);
}

udpSession* UdpSMInterface::udpGetSessionFromPool(uint32_t idx)
{
	uint32_t arrId = idx / UDP_SESSION_POOL_ARRAY_ELEMENTS;
	uint32_t bitId = idx % UDP_SESSION_POOL_ARRAY_ELEMENTS;
	return udpSessionPoolMap[arrId][bitId];
}

uint32_t UdpSMInterface::getMapIndexAndSessionKey(UdpPacket *udpMsg)
{
	uint32_t mapIndex;

    switch(udpMsg->ipVer)
    {
    	case IPVersion4:
    			ipV4Key = udpMsg->ipv4FlowId;

    			switch(udpMsg->direction)
    			{
					case UP:
							mapIndex = udpMsg->sIp % UDP_SESSION_POOL_ARRAY_ELEMENTS;
							break;

					case DOWN:
							mapIndex = udpMsg->dIp % UDP_SESSION_POOL_ARRAY_ELEMENTS;
							break;
    			}
				break;
    	default:
    			break;
	}
	return mapIndex;
}

void UdpSMInterface::UDPPacketEntry(UdpPacket *udpMsg)
{
	if(udpMsg == NULL)
		return;

	bool found = false;

	udpSession *pUdpSession = udpGetSession(udpMsg, &found, true);

	/* Couldn't Create Session */
	if(pUdpSession == NULL) return;

	/*
	 * Update Packet arrival & Last Packet time
	 */
	timeStampArrivalPacket(pUdpSession, udpMsg->frTimeEpochSec, udpMsg->frTimeEpochNanoSec);

	/* Create New Session */
	if(!found)
	{
		initializeUdpSession(pUdpSession, udpMsg);
		uint32_t ip = pFUtility->getV4UserId(pUdpSession->sIpv4, pUdpSession->userId);
		updateUdpSession(pUdpSession, udpMsg);
	}
	else
	{ updateUdpSession(pUdpSession, udpMsg); }

	pUdpSession = NULL;
}

udpSession* UdpSMInterface::udpGetSession(UdpPacket *udpMsg, bool *found, bool create)
{
	uint32_t sessionCnt = 0;
	udpSession *pUdpSession = NULL;
	uint32_t mapIndex, poolIndex;

	mapIndex = getMapIndexAndSessionKey(udpMsg);

	switch(udpMsg->ipVer)
	{
		case IPVersion4:
		{
			std::map<uint64_t, uint32_t>::iterator it = udpV4SessionMap[mapIndex].find(ipV4Key);

			if(it != udpV4SessionMap[mapIndex].end())
			{
				pUdpSession = udpGetSessionFromPool(it->second);
				*found = true;
			}
			else
			{
				if(create) {
					for(int i = 0; i < UDP_SESSION_POOL_ARRAY_ELEMENTS; i++)
						sessionCnt += (udpV4SessionMap[i].size() + udpV6SessionMap[i].size());

					if(sessionCnt < udpFreeBitPosMax) {
						poolIndex = udpGetFreeIndex();
						pUdpSession = udpGetSessionFromPool(poolIndex);
						pUdpSession->reset();

						pUdpSession->sessionIpV4Key = ipV4Key;
						pUdpSession->smInstanceId = this->instanceId;
						pUdpSession->mapIndex = mapIndex;
						pUdpSession->poolIndex = poolIndex;
						udpV4SessionMap[pUdpSession->mapIndex][pUdpSession->sessionIpV4Key] = poolIndex;
					}
				}
				*found = false;
			}
		}
		break;
	}
	return pUdpSession;
}

void UdpSMInterface::initializeUdpSession(udpSession *pUdpSession, UdpPacket *udpMsg)
{
	pUdpSession->ipVer = udpMsg->ipVer;
	pUdpSession->pType = PACKET_IPPROTO_UDP;
	pUdpSession->state = UD_UDP_DATA;
	pUdpSession->sliceCounter = 0;

	pUdpSession->isUpDir = udpMsg->direction;
	pUdpSession->vLanId = udpMsg->vLanId;

	pUdpSession->startTimeEpochSec 		= pUdpSession->pckArivalTimeEpochSec;
	pUdpSession->startTimeEpochNanoSec 	= pUdpSession->pckLastTimeEpochNanoSec;

	switch(udpMsg->direction)
	{
		case UP:
				pUdpSession->sPort = udpMsg->sPort;
				pUdpSession->dPort = udpMsg->dPort;

				switch(udpMsg->ipVer)
				{
					case IPVersion4:
								pUdpSession->sIpv4 = udpMsg->sIp;
								pUdpSession->dIpv4 = udpMsg->dIp;
								break;
				}
				break;

		case DOWN:
				pUdpSession->sPort = udpMsg->dPort;
				pUdpSession->dPort = udpMsg->sPort;

				switch(udpMsg->ipVer)
				{
					case IPVersion4:
								pUdpSession->sIpv4 = udpMsg->dIp;
								pUdpSession->dIpv4 = udpMsg->sIp;
								break;
				}
				break;
	}
}

void UdpSMInterface::updateUdpSession(udpSession *pUdpSession, UdpPacket *udpMsg)
{
	uint64_t timeDiff = 0;

	if(udpMsg->pLoad > 0)
		vpsFlag = updateVPS(pUdpSession, udpMsg);

	if(!vpsFlag) return;

	pUdpSession->frCount += 1;
	pUdpSession->frSize += udpMsg->frSize;

	switch(udpMsg->direction)
	{
		case UP:
				pUdpSession->upFrSize += udpMsg->frSize;
				pUdpSession->upFrCount += 1;

				if(udpMsg->pLoad > 0) {
					pUdpSession->pLoadPkt += 1;
					pUdpSession->pLoadSize +=  udpMsg->pLoad;
					pUdpSession->upPLoadPkt += 1;
					pUdpSession->upPLoadSize += udpMsg->pLoad;
				}
				break;

		case DOWN:
				pUdpSession->dnFrSize += udpMsg->frSize;
				pUdpSession->dnFrCount += 1;

				if(udpMsg->pLoad > 0) {
					pUdpSession->pLoadPkt += 1;
					pUdpSession->pLoadSize +=  udpMsg->pLoad;
					pUdpSession->dnPLoadPkt += 1;
					pUdpSession->dnPLoadSize += udpMsg->pLoad;
				}
				break;
	}

	/** Check the Data Slicing **/
	if(pUdpSession->frCount >= SMGlobal::SESSION_PKT_LIMIT)
	{
		pUdpSession->causeCode = SYSTEM_PKTLIMIT_UDP_DATA;

		udpFlushSession(5, pUdpSession, false);
		pUdpSession->reuse();
		pUdpSession->startTimeEpochSec = udpMsg->frTimeEpochSec;
		pUdpSession->startTimeEpochNanoSec = udpMsg->frTimeEpochNanoSec;
	}
	else
	{
		if(pUdpSession->pckLastTimeEpochSec > pUdpSession->startTimeEpochSec){
			timeDiff = pUdpSession->pckLastTimeEpochSec - pUdpSession->startTimeEpochSec;

			if (timeDiff >= SMGlobal::SESSION_TIME_LIMIT) {
				pUdpSession->causeCode = SYSTEM_TIMEOUT_UDP_DATA;

				udpFlushSession(9, pUdpSession, false);
				pUdpSession->reuse();
			}
		}
	}
}

bool UdpSMInterface::updateVPS(udpSession *pUdpSession, UdpPacket *udpMsg)
{
	fData *pFData = &pUdpSession->packTimeMap[pUdpSession->pckArivalTimeEpochSec];

	pFData->totalVolume += udpMsg->pLoad;

	switch(udpMsg->direction)
	{
		case UP:
				pFData->upPackets += 1;
				pFData->upVolume += udpMsg->pLoad;
				break;

		case DOWN:
				if(pFData->dnPackets > SMGlobal::VPS_PACKET_PER_SEC)
					return false;

				pFData->dnPackets += 1;
				pFData->dnVolume += udpMsg->pLoad;
				break;
	}
	return true;
}

void UdpSMInterface::timeStampArrivalPacket(udpSession *pUdpSession, uint64_t epochSec, uint64_t epochNanoSec)
{
	pUdpSession->pckArivalTimeEpochSec 	= epochSec;
	pUdpSession->pckLastTimeEpochSec 	= epochSec;
	pUdpSession->pckLastTimeEpochNanoSec = epochNanoSec;
}

void UdpSMInterface::udpFlushSession(uint16_t flushOrgId, udpSession *pUdpSession, bool erase)
{
	uint64_t epochSec = Global::CURRENT_EPOCH_SEC;
	uint64_t epochSecNano = Global::CURRENT_EPOCH_NANO_SEC;

	pUdpSession->flushOrgId = flushOrgId;
	pUdpSession->flushTime = epochSec;
	pUdpSession->endTimeEpochNanoSec = epochSecNano;

	uint16_t idx = PKT_WRITE_TIME_INDEX(epochSec, Global::TIME_INDEX);

	pUdpSession->sliceCounter += 1;
	udpStoreSession(idx, pUdpSession);

	if(erase)
		udpEraseSession(pUdpSession);
}

void UdpSMInterface::udpStoreSession(uint16_t idx, udpSession *pUdpSession)
{
	uint16_t flusherNo = instanceId % SMGlobal::UDP_FLUSHER_INSTANCE;

	FlushRepo::udpFlStore[flusherNo][instanceId][idx][FlushRepo::udpFlCnt[flusherNo][instanceId][idx]].copy(pUdpSession);
	FlushRepo::udpFlCnt[flusherNo][instanceId][idx]++;
}

void UdpSMInterface::udpTimeOutClean(bool endOfDay)
{
	sessionCleanCnt = 0;

	IPStats::udpV4SessionTotalCnt[instanceId] = 0;
	IPStats::udpV4SessionScanned[instanceId] 	= 0;
	IPStats::udpV4SessionCleaned[instanceId] 	= 0;

	IPStats::udpV6SessionTotalCnt[instanceId] = 0;

	for(uint16_t i = 0; i < UDP_SESSION_POOL_ARRAY_ELEMENTS; i++)
	{
		IPStats::udpV4SessionTotalCnt[instanceId] += udpV4SessionMap[i].size();
		IPStats::udpV6SessionTotalCnt[instanceId] += udpV6SessionMap[i].size();
	}

	if(endOfDay)
	{
		for(uint16_t i = 0; i < UDP_SESSION_POOL_ARRAY_ELEMENTS; i++)
		{
			for(auto elem = udpV4SessionMap[i].begin(), next_elem = elem; elem != udpV4SessionMap[i].end(); elem = next_elem)
			{
				++next_elem;
					udpCleanSession(udpGetSessionFromPool(elem->second), endOfDay);
					IPStats::udpV4SessionScanned[instanceId]++ ;
			}
		}

		IPStats::udpV4SessionCleaned[instanceId] = sessionCleanCnt;
		sessionCleanCnt = 0;

		for(uint16_t i = 0; i < UDP_SESSION_POOL_ARRAY_ELEMENTS; i++)
		{
			for(auto elem = udpV6SessionMap[i].begin(), next_elem = elem; elem != udpV6SessionMap[i].end(); elem = next_elem)
			{
				++next_elem;
					udpCleanSession(udpGetSessionFromPool(elem->second), endOfDay);
					IPStats::udpV6SessionScanned[instanceId]++ ;
			}
		}

		IPStats::udpV6SessionCleaned[instanceId] = sessionCleanCnt;
		sessionCleanCnt = 0;
	}
	else
	{
		for(uint16_t i = 0; i < UDP_SESSION_POOL_ARRAY_ELEMENTS; i++)
		{
//			if(sessionCleanCnt >= SMGlobal::UDP_SESSION_CLEAN_UP_BATCH_LIMIT) break;		//Apply only for timeout
			for(auto elem = udpV4SessionMap[i].begin(), next_elem = elem; elem != udpV4SessionMap[i].end(); elem = next_elem)
			{
				++next_elem;
//				if(sessionCleanCnt >= SMGlobal::UDP_SESSION_CLEAN_UP_BATCH_LIMIT) break;	//Apply only for timeout
					udpCleanSession(udpGetSessionFromPool(elem->second), endOfDay);
					IPStats::udpV4SessionScanned[instanceId]++ ;
			}
		}

		IPStats::udpV4SessionCleaned[instanceId] = sessionCleanCnt;
		sessionCleanCnt = 0;

		for(uint16_t i = 0; i < UDP_SESSION_POOL_ARRAY_ELEMENTS; i++)
		{
//			if(sessionCleanCnt >= SMGlobal::UDP_SESSION_CLEAN_UP_BATCH_LIMIT) break;		//Apply only for timeout
			for(auto elem = udpV6SessionMap[i].begin(), next_elem = elem; elem != udpV6SessionMap[i].end(); elem = next_elem)
			{
				++next_elem;
//				if(sessionCleanCnt >= SMGlobal::UDP_SESSION_CLEAN_UP_BATCH_LIMIT) break;	//Apply only for timeout
					udpCleanSession(udpGetSessionFromPool(elem->second), endOfDay);
					IPStats::udpV6SessionScanned[instanceId]++ ;
			}
		}

		IPStats::udpV6SessionCleaned[instanceId] = sessionCleanCnt;
		sessionCleanCnt = 0;
	}
}

void UdpSMInterface::udpCleanSession(udpSession *pUdpSession, bool endOfDay)
{
	uint64_t curEpochSec = Global::CURRENT_EPOCH_SEC;

	if(endOfDay)
	{
		pUdpSession->causeCode = SYSTEM_CLEANUP_END_OF_DAY_UDP_DATA;
		udpFlushSession(7, pUdpSession, true);
		sessionCleanCnt++;
	}
	else
	{
		if((curEpochSec - pUdpSession->pckLastTimeEpochSec) > SMGlobal::UDP_CLEAN_UP_TIMEOUT_SEC)
		{
			if (pUdpSession->pType == PACKET_IPPROTO_UDP)
					pUdpSession->causeCode = SYSTEM_CLEANUP_UDP_DATA;

			sessionCleanCnt++;

			udpFlushSession(7, pUdpSession, true);
		}
	}
}

void UdpSMInterface::udpEraseSession(udpSession *pUdpSession)
{
	uint32_t idx, poolIndex;

	switch(pUdpSession->ipVer)
	{
		case IPVersion4:
		{
			uint64_t sKey4 = pUdpSession->sessionIpV4Key;
			idx = pUdpSession->mapIndex;
			poolIndex = pUdpSession->poolIndex;
			udpReleaseIndex(poolIndex);
			udpV4SessionMap[idx].erase(sKey4);
		}
		break;

//		case IPVersion6:
//		{
//			string sKey6 = pUdpSession->ipV6sessionKey;
//			idx = pUdpSession->mapIndex;
//			poolIndex = pUdpSession->poolIndex;
//			udpReleaseIndex(poolIndex);
//			udpV6SessionMap[idx].erase(sKey6);
//		}
//		break;
	}
}

void UdpSMInterface::udpV4SessionCount()
{
	IPStats::tcpV4SessionTotalCnt[instanceId] = 0;
	for(uint16_t i = 0; i < UDP_SESSION_POOL_ARRAY_ELEMENTS; i++)
		IPStats::tcpV4SessionTotalCnt[instanceId] += udpV4SessionMap[i].size();
}

void UdpSMInterface::udpV6SessionCount()
{
	IPStats::tcpV6SessionTotalCnt[instanceId] = 0;
	for(uint16_t i = 0; i < UDP_SESSION_POOL_ARRAY_ELEMENTS; i++)
		IPStats::tcpV6SessionTotalCnt[instanceId] += udpV6SessionMap[i].size();
}

