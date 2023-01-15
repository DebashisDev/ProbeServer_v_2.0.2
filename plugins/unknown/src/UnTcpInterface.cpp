/*
 * UnTcpInetrface.cpp
 *
 *  Created on: 16-Aug-2021
 *      Author: Debashis
 */

#include "UnTcpInterface.h"

UnTcpInterface::UnTcpInterface(uint16_t id)
{
	_thisLogLevel = 0;
	this->_name = "UnTcpInetrface";
	this->setLogLevel(Log::theLog().level());
	this->instanceId = id;
	this->timeindex = 0;
	tcpInitializeSessionPool();
	cleanUpCnt = 0;

	ipV4Key = 0;
	ipV6Key = "";
}

UnTcpInterface::~UnTcpInterface()
{ }

uint32_t UnTcpInterface::getMapIndexAndSessionKey(TcpPacket *tcpMsg)
{
    uint32_t mapIndex = 0;

    switch(tcpMsg->ipVer)
    {
    	case IPVersion4:
    	{
			ipV4Key = tcpMsg->ipv4FlowId;
			mapIndex = tcpMsg->sIp % UNM_SESSION_POOL_ARRAY_ELEMENTS;
    	}
    	break;

    	default:
    		break;
	}
	return mapIndex;
}

uint32_t UnTcpInterface::tcpGetFreeIndex()
{
	tcpFreeBitPos++;
	if(tcpFreeBitPos >= tcpFreeBitPosMax) tcpFreeBitPos = 0;
	int arrId = tcpFreeBitPos / UNM_SESSION_POOL_ARRAY_ELEMENTS;
	int bitId = tcpFreeBitPos % UNM_SESSION_POOL_ARRAY_ELEMENTS;

	while(tcpBitFlagsSession[arrId].test(bitId)){
		tcpFreeBitPos++;
		if(tcpFreeBitPos >= tcpFreeBitPosMax) tcpFreeBitPos = 0;
		arrId = tcpFreeBitPos / UNM_SESSION_POOL_ARRAY_ELEMENTS;
		bitId = tcpFreeBitPos % UNM_SESSION_POOL_ARRAY_ELEMENTS;
	}
	if(tcpFreeBitPos >= tcpFreeBitPosMax){
		printf("[%d] getFreeIndexIp freeBitPosIp [%u] >= freeBitPosIpMax [%u]\n",instanceId, tcpFreeBitPos, tcpFreeBitPosMax);
	}
	tcpBitFlagsSession[arrId].set(bitId);
	return tcpFreeBitPos;
}

void UnTcpInterface::tcpReleaseIndex(uint32_t idx)
{
	uint32_t arrId = idx / UNM_SESSION_POOL_ARRAY_ELEMENTS;
	uint32_t bitId = idx % UNM_SESSION_POOL_ARRAY_ELEMENTS;

	tcpSessionPoolMap[arrId][bitId]->reset();
	tcpSessionPoolMap[arrId][bitId]->poolIndex = idx;
	tcpBitFlagsSession[arrId].reset(bitId);
}

void UnTcpInterface::tcpInitializeSessionPool()
{
	tcpFreeBitPosMax = UNM_SESSION_POOL_ARRAY_ELEMENTS * UNM_SESSION_POOL_ARRAY_SIZE;

	printf("IPSMInterface [%02d]	Initializing [%u] UNKNOWN TCP Session Pool... ", instanceId, tcpFreeBitPosMax);
	TheLog_nc_v2(Log::Info, name(),"     [%02d] Initializing [%u] UNKNOWN TCP Session Pool...", instanceId, tcpFreeBitPosMax);
	for(int i=0; i<UNM_SESSION_POOL_ARRAY_SIZE; i++)
	{
		tcpBitFlagsSession[i].reset();
		for(int j=0; j<UNM_SESSION_POOL_ARRAY_ELEMENTS; j++)
		{
			tcpSessionPoolMap[i][j] = new tcpSession();
			tcpSessionPoolMap[i][j]->poolIndex = (i*UNM_SESSION_POOL_ARRAY_ELEMENTS) + j;
		}
	}
	printf("Completed.\n");
	TheLog_nc_v2(Log::Info, name(),"     [%02d] Initializing [%u] UNKNOWN TCP Session Pool Completed.", instanceId, tcpFreeBitPosMax);
}

tcpSession* UnTcpInterface::tcpGetSessionFromPool(uint32_t idx)
{
	uint32_t arrId = idx / UNM_SESSION_POOL_ARRAY_ELEMENTS;
	uint32_t bitId = idx % UNM_SESSION_POOL_ARRAY_ELEMENTS;
	return tcpSessionPoolMap[arrId][bitId];
}

tcpSession* UnTcpInterface::tcpGetSession(TcpPacket *tcpMsg, bool *found, bool create)
{
	uint32_t sessionCnt = 0;
	tcpSession *pTcpSession = NULL;
	uint32_t mapIndex, poolIndex;

	mapIndex = getMapIndexAndSessionKey(tcpMsg);

	switch(tcpMsg->ipVer)
	{
		case IPVersion4:
		{
			std::map<uint64_t, uint32_t>::iterator it = tcpV4SessionMap[mapIndex].find(ipV4Key);

			if(it != tcpV4SessionMap[mapIndex].end())
			{
				pTcpSession = tcpGetSessionFromPool(it->second);
				*found = true;
			}
			else
			{
				if(create)
				{
					for(uint16_t i = 0; i < UNM_SESSION_POOL_ARRAY_ELEMENTS; i++)
						sessionCnt += (tcpV4SessionMap[i].size() + tcpV6SessionMap[i].size());

					if(sessionCnt < tcpFreeBitPosMax)
					{
						poolIndex = tcpGetFreeIndex();
						pTcpSession = tcpGetSessionFromPool(poolIndex);
						pTcpSession->reset();

						pTcpSession->sessionIpV4Key = ipV4Key;
						pTcpSession->smInstanceId = this->instanceId;
						pTcpSession->mapIndex = mapIndex;
						pTcpSession->poolIndex = poolIndex;
						tcpV4SessionMap[pTcpSession->mapIndex][pTcpSession->sessionIpV4Key] = poolIndex;
					}
				}
				*found = false;
			}
		}
		break;
	}
	return pTcpSession;
}

void UnTcpInterface::TCPPacketEntry(TcpPacket *tcpMsg)
{
	bool found = false;

	tcpSession *pTcpSession;

	switch(tcpMsg->tcpFlags)
	{
		case SYN_RCV:
		case SYN_ACK_RCV:
		case ACK_RCV:
		case DATA_RCV:
					if((pTcpSession = tcpGetSession(tcpMsg, &found, true)) == NULL)
						return;

					timeStampArrivalPacket(pTcpSession, tcpMsg->frTimeEpochSec, tcpMsg->frTimeEpochNanoSec);

					if(!found)
					{
						tcpInitializeSession(pTcpSession, tcpMsg);	/* Initialize TCP Packet */
						tcpUpdateSession(pTcpSession, tcpMsg);			/* Update TCP Packet */
					}
					else
					{
						tcpUpdateSession(pTcpSession, tcpMsg);
					}
					break;

		case FIN_RCV:
					if((pTcpSession = tcpGetSession(tcpMsg, &found, false)) == NULL)
						return;

					tcpFlushSession(TCP_FIN, pTcpSession, true);
					break;

		default:
					break;
	}
	pTcpSession = NULL;
}

void UnTcpInterface::tcpInitializeSession(tcpSession *pTcpSession, TcpPacket *tcpMsg)
{
	pTcpSession->ipVer = tcpMsg->ipVer;
	pTcpSession->pType = PACKET_IPPROTO_TCP;
	pTcpSession->TTL = tcpMsg->ipTtl;
	pTcpSession->vLanId = tcpMsg->vLanId;

	switch(tcpMsg->ipVer)
	{
		case IPVersion4:
					pTcpSession->sIpv4 = tcpMsg->sIp;
					pTcpSession->dIpv4 = tcpMsg->dIp;
					break;
	}

	pTcpSession->sPort = tcpMsg->sPort;
	pTcpSession->dPort = tcpMsg->dPort;

	pTcpSession->sliceCounter = 0;

	pTcpSession->startTimeEpochSec = pTcpSession->pckArivalTimeEpochSec;
	pTcpSession->startTimeEpochNanoSec = pTcpSession->pckLastTimeEpochNanoSec;
}

void UnTcpInterface::timeStampArrivalPacket(tcpSession *pTcpSession, uint64_t epochSec, uint64_t epochNanoSec)
{
	pTcpSession->pckArivalTimeEpochSec 	= epochSec;
	pTcpSession->pckLastTimeEpochSec 	= epochSec;
	pTcpSession->pckLastTimeEpochNanoSec = epochNanoSec;
}

void UnTcpInterface::tcpUpdateSession(tcpSession *pTcpSession, TcpPacket *tcpMsg)
{
	uint64_t timeDiff = 0;

	pTcpSession->frCount += 1;
	pTcpSession->frSize += tcpMsg->frSize;

		if(tcpMsg->pLoad > 0)
		{
			pTcpSession->pLoadPkt += 1;
			pTcpSession->pLoadSize +=  tcpMsg->pLoad;
		}

	/*
	 * Don't flush the TCP Session which don't have connection
	 */
	if((pTcpSession->frCount >= SMGlobal::SESSION_PKT_LIMIT))
	{
		tcpFlushSession(TCP_LIMIT, pTcpSession, false);
		pTcpSession->reuse();
	}
}

void UnTcpInterface::tcpFlushSession(uint16_t flushOrgId, tcpSession *pTcpSession, bool erase)
{
	uint64_t epochSec = Global::CURRENT_EPOCH_SEC;
	uint64_t epochNanoSec = Global::CURRENT_EPOCH_NANO_SEC;

	pTcpSession->flushOrgId = flushOrgId;
	pTcpSession->flushTime = epochSec;
	pTcpSession->endTimeEpochNanoSec = epochNanoSec;

	timeindex = PKT_WRITE_TIME_INDEX(epochSec, Global::TIME_INDEX);

	pTcpSession->sliceCounter += 1;
	tcpStoreSession(timeindex, pTcpSession);

	if(erase)
		tcpEraseSession(pTcpSession);
}

void UnTcpInterface::tcpStoreSession(uint16_t timeIndex, tcpSession *pTcpSession)
{
	UnFlushRepo::uTcpFlStore[instanceId][timeIndex][UnFlushRepo::uTcpFlCnt[instanceId][timeIndex]].copy(pTcpSession);
	UnFlushRepo::uTcpFlCnt[instanceId][timeIndex]++;
}

void UnTcpInterface::tcpTimeOutClean(bool endOfDay)
{
	cleanUpCnt = 0;
	uint32_t totalCount = 0;

	IPStats::unTcpSessionCnt[instanceId] 		= 0;
	IPStats::unTcpSessionScanned[instanceId] 	= 0;
	IPStats::unTcpSessionCleaned[instanceId] 	= 0;

	for(uint16_t i = 0; i < UNM_SESSION_POOL_ARRAY_ELEMENTS; i++)
		totalCount += (tcpV4SessionMap[i].size() + tcpV6SessionMap[i].size());

	IPStats::unTcpSessionCnt[instanceId] = totalCount;

	if(endOfDay)
	{
		for(uint16_t i = 0; i < UNM_SESSION_POOL_ARRAY_ELEMENTS; i++)
		{
			for(auto elem = tcpV4SessionMap[i].begin(), next_elem = elem; elem != tcpV4SessionMap[i].end(); elem = next_elem)
			{
				++next_elem;
				tcpCleanSession(tcpGetSessionFromPool(elem->second), endOfDay);
				IPStats::unTcpSessionScanned[instanceId]++;
			}
		}

		IPStats::unTcpSessionCleaned[instanceId] = cleanUpCnt;
		cleanUpCnt = 0;

		for(uint16_t i = 0; i < UNM_SESSION_POOL_ARRAY_ELEMENTS; i++)
		{
			for(auto elem = tcpV6SessionMap[i].begin(), next_elem = elem; elem != tcpV6SessionMap[i].end(); elem = next_elem)
			{
				++next_elem;
					tcpCleanSession(tcpGetSessionFromPool(elem->second), endOfDay);
					IPStats::unTcpSessionScanned[instanceId]++;
			}
		}

		IPStats::unTcpSessionCleaned[instanceId] += cleanUpCnt;
		cleanUpCnt = 0;
	}
	else
	{
		for(uint16_t i = 0; i < UNM_SESSION_POOL_ARRAY_ELEMENTS; i++)
		{
			for(auto elem = tcpV4SessionMap[i].begin(), next_elem = elem; elem != tcpV4SessionMap[i].end(); elem = next_elem)
			{
				++next_elem;
					tcpCleanSession(tcpGetSessionFromPool(elem->second), endOfDay);
					IPStats::unTcpSessionScanned[instanceId]++ ;
			}
		}

		IPStats::unTcpSessionCleaned[instanceId] = cleanUpCnt;
		cleanUpCnt = 0;

		for(uint16_t i = 0; i < UNM_SESSION_POOL_ARRAY_ELEMENTS; i++)
		{
			for(auto elem = tcpV6SessionMap[i].begin(), next_elem = elem; elem != tcpV6SessionMap[i].end(); elem = next_elem)
			{
				++next_elem;
					tcpCleanSession(tcpGetSessionFromPool(elem->second), endOfDay);
					IPStats::unTcpSessionScanned[instanceId]++ ;
			}
		}

		IPStats::unTcpSessionCleaned[instanceId] += cleanUpCnt;
		cleanUpCnt = 0;
	}
}

void UnTcpInterface::tcpCleanSession(tcpSession *pTcpSession, bool endOfDay)
{
	uint64_t curEpochSec = Global::CURRENT_EPOCH_SEC;

	if(endOfDay)
	{
		pTcpSession->causeCode = SYSTEM_CLEANUP_END_OF_DAY_TCP_DATA;
		IPStats::unTcpSessionCleaned[instanceId]++;
		cleanUpCnt++;
		tcpFlushSession(TCP_CLEAN, pTcpSession, true);
	}
	else
	{
		if((curEpochSec - pTcpSession->pckLastTimeEpochSec) > SMGlobal::TCP_CLEAN_UP_TIMEOUT_SEC)
		{
			pTcpSession->causeCode = SYSTEM_CLEANUP_TCP_DATA;
			IPStats::unTcpSessionCleaned[instanceId]++;
			cleanUpCnt++;
			tcpFlushSession(TCP_CLEAN, pTcpSession, true);
		}
	}
}

void UnTcpInterface::tcpEraseSession(tcpSession *pTcpSession)
{
	uint32_t index, poolIndex;

	switch(pTcpSession->ipVer)
	{
		case IPVersion4:
		{
			uint64_t sKey4 = pTcpSession->sessionIpV4Key;
			index = pTcpSession->mapIndex;
			poolIndex = pTcpSession->poolIndex;
			tcpReleaseIndex(poolIndex);
			tcpV4SessionMap[index].erase(sKey4);
		}
		break;

		default:
			break;
	}
}
