/*
 * IPSMInterface.cpp
 *
 *  Created on: 20-Jul-2016
 *      Author: Debashis
 */

#include "TcpSMInterface.h"

#include "sys/time.h"
#include <locale.h>

TcpSMInterface::TcpSMInterface(uint16_t id)
{
	_thisLogLevel = 0;
	this->_name = "TcpSMInterface";
	this->setLogLevel(Log::theLog().level());

	this->instanceId 	= id;
	this->tIdx 	= 0;
	this->flusherId 	= 0;
	this->cleanUpCnt 	= 0;

	this->pFUtility 	= new FUtility();
	initSessionPool();

	ipV4Key = 0;
	ipV6Key = "";
}

TcpSMInterface::~TcpSMInterface()
{ delete (this->pFUtility); }

void TcpSMInterface::getMapIndexAndSessionKey(TcpPacket *pkt, uint32_t &idx)
{
    switch(pkt->ipVer)
    {
    	case IPVersion4:
    			switch(pkt->direction)
    			{
    				case UP:
							ipV4Key = pkt->ipv4FlowId;
							idx = pkt->sIp % TCP_SESSION_POOL_ARRAY_ELEMENTS;
    						break;

    				case DOWN:
							ipV4Key = pkt->ipv4FlowId;
							idx = pkt->dIp % TCP_SESSION_POOL_ARRAY_ELEMENTS;
    						break;
				}
				break;
    	default:
    			break;
	}
}

uint32_t TcpSMInterface::tcpGetFreeIndex()
{
	tcpFreeBitPos++;
	if(tcpFreeBitPos >= tcpFreeBitPosMax) tcpFreeBitPos = 0;
	int arrId = tcpFreeBitPos / TCP_SESSION_POOL_ARRAY_ELEMENTS;
	int bitId = tcpFreeBitPos % TCP_SESSION_POOL_ARRAY_ELEMENTS;

	while(tcpBitFlagsSession[arrId].test(bitId))
	{
		tcpFreeBitPos++;
		if(tcpFreeBitPos >= tcpFreeBitPosMax) tcpFreeBitPos = 0;
		arrId = tcpFreeBitPos / TCP_SESSION_POOL_ARRAY_ELEMENTS;
		bitId = tcpFreeBitPos % TCP_SESSION_POOL_ARRAY_ELEMENTS;
	}
	if(tcpFreeBitPos >= tcpFreeBitPosMax)
	{
		printf("[%d] getFreeIndexIp freeBitPosIp [%u] >= freeBitPosIpMax [%u]\n",instanceId, tcpFreeBitPos, tcpFreeBitPosMax);
	}
	tcpBitFlagsSession[arrId].set(bitId);
	return tcpFreeBitPos;
}

void TcpSMInterface::tcpReleaseIndex(uint32_t idx)
{
	uint32_t arrId = idx / TCP_SESSION_POOL_ARRAY_ELEMENTS;
	uint32_t bitId = idx % TCP_SESSION_POOL_ARRAY_ELEMENTS;

	tcpSessionPoolMap[arrId][bitId]->reset();
	tcpSessionPoolMap[arrId][bitId]->poolIndex = idx;
	tcpBitFlagsSession[arrId].reset(bitId);
}

void TcpSMInterface::initSessionPool()
{
	tcpFreeBitPosMax = TCP_SESSION_POOL_ARRAY_ELEMENTS * TCP_SESSION_POOL_ARRAY_SIZE;

	printf("IPSMInterface [%02d]	Initializing [%u] TCP Session Pool... ", instanceId, tcpFreeBitPosMax);
	TheLog_nc_v2(Log::Info, name(),"     [%02d] Initializing [%u] TCP Session Pool...", instanceId, tcpFreeBitPosMax);

	for(uint16_t i = 0; i < TCP_SESSION_POOL_ARRAY_SIZE; i++)
	{
		tcpBitFlagsSession[i].reset();
		for(uint16_t j = 0; j < TCP_SESSION_POOL_ARRAY_ELEMENTS; j++)
		{
			tcpSessionPoolMap[i][j] = new tcpSession();
			tcpSessionPoolMap[i][j]->poolIndex = (i*TCP_SESSION_POOL_ARRAY_ELEMENTS) + j;
		}
	}
	printf("Completed.\n");
	TheLog_nc_v2(Log::Info, name(),"     [%d] Initializing [%u] TCP Session Pool Completed.", instanceId, tcpFreeBitPosMax);
}

tcpSession* TcpSMInterface::tcpGetSessionFromPool(uint32_t idx)
{
	uint32_t arrId = idx / TCP_SESSION_POOL_ARRAY_ELEMENTS;
	uint32_t bitId = idx % TCP_SESSION_POOL_ARRAY_ELEMENTS;
	return tcpSessionPoolMap[arrId][bitId];
}

tcpSession* TcpSMInterface::tcpGetSession(TcpPacket *pkt, bool *found, bool create)
{
	uint64_t sessionCnt = 0;
	tcpSession *pTcpSession = NULL;
	uint32_t idx = 0;
	uint32_t poolIndex = 0;

	/* Get Index */
	getMapIndexAndSessionKey(pkt, idx);

	switch(pkt->ipVer)
	{
		case IPVersion4:
		{
			std::map<uint64_t, uint32_t>::iterator it = tcpV4SessionMap[idx].find(ipV4Key);

			if(it != tcpV4SessionMap[idx].end())
			{
				pTcpSession = tcpGetSessionFromPool(it->second);
				*found = true;
			}
			else
			{
				if(create)
				{
					for(uint16_t i = 0; i < TCP_SESSION_POOL_ARRAY_ELEMENTS; i++)
						sessionCnt += (tcpV4SessionMap[i].size() + tcpV6SessionMap[i].size());

					if(sessionCnt < tcpFreeBitPosMax)
					{
						poolIndex = tcpGetFreeIndex();
						pTcpSession = tcpGetSessionFromPool(poolIndex);
						pTcpSession->reset();

						pTcpSession->sessionIpV4Key = ipV4Key;
						pTcpSession->smInstanceId = this->instanceId;
						pTcpSession->mapIndex = idx;
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

void TcpSMInterface::updateTime(tcpSession *pTcpSession, uint16_t id)
{
	switch(id)
	{
		case SYN_RCV:
				pTcpSession->synRcv = true;
				pTcpSession->synTimeEpochNanoSec = pTcpSession->pckLastTimeEpochNanoSec;
				break;

		case SYN_ACK_RCV:
				pTcpSession->synAckRcv = true;
				pTcpSession->synAckTimeEpochNanoSec = pTcpSession->pckLastTimeEpochNanoSec;
				break;

		case ACK_RCV:
				pTcpSession->ackRcv = true;
				pTcpSession->ackTimeEpochNanoSec = pTcpSession->pckLastTimeEpochNanoSec;
				break;

		case DATA_RCV:
				pTcpSession->dataRcv = true;
				pTcpSession->firstDataTimeEpochNanoSec = pTcpSession->pckLastTimeEpochNanoSec;
				pTcpSession->firstDataFlag = true;
				break;

		case FIN_RCV:
				pTcpSession->finRcv = true;
				pTcpSession->finTimeEpochNanoSec = pTcpSession->pckLastTimeEpochNanoSec;
				pTcpSession->causeCode = SESSION_TERM_TCP_FIN_RECEIVED;
				break;
	}

	if(pTcpSession->synRcv && pTcpSession->synAckRcv && pTcpSession->ackRcv)
		pTcpSession->state = CONNECTED;
}

void TcpSMInterface::TCPPacketEntry(TcpPacket *tcpMsg)
{
	bool found 	= false;
	uint32_t ip = 0;
	tcpSession *pTcpSession;

	if(tcpMsg == NULL) return;

	switch(tcpMsg->tcpFlags)
	{
		case SYN_RCV:
		case SYN_ACK_RCV:
		case ACK_RCV:
		case DATA_RCV:
					if((pTcpSession = tcpGetSession(tcpMsg, &found, true)) == NULL)
						return;

					timeStampArrivalPacket(pTcpSession, tcpMsg->frTimeEpochSec, tcpMsg->frTimeEpochNanoSec);

					/*
					 * Update the time of packets based on TCP Flag
					 */
					updateTime(pTcpSession, tcpMsg->tcpFlags);

					/* Create a New Session */
					if(!found)
					{
						tcpInitializeSession(pTcpSession, tcpMsg);	/* Initialize TCP Packet */
						ip = pFUtility->getV4UserId(pTcpSession->sIpv4, pTcpSession->userId);
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

					timeStampArrivalPacket(pTcpSession, tcpMsg->frTimeEpochSec, tcpMsg->frTimeEpochNanoSec);
					updateTime(pTcpSession, FIN_RCV);						/* Update Syn Ack Time */
					tcpUpdateSession(pTcpSession, tcpMsg);
					tcpFlushSession(TCP_FIN, pTcpSession, true);

					break;

		default:
					break;
	}
	pTcpSession = NULL;
}

void TcpSMInterface::tcpInitializeSession(tcpSession *pTcpSession, TcpPacket *tcpMsg)
{
	pTcpSession->ipVer = tcpMsg->ipVer;
	pTcpSession->pType = PACKET_IPPROTO_TCP;
	pTcpSession->TTL = tcpMsg->ipTtl;
	pTcpSession->vLanId = tcpMsg->vLanId;

	switch(tcpMsg->ipVer)
	{
		case IPVersion4:
				switch(tcpMsg->direction)
				{
					case UP:
						pTcpSession->sIpv4 = tcpMsg->sIp;
						pTcpSession->dIpv4 = tcpMsg->dIp;
						pTcpSession->sPort = tcpMsg->sPort;
						pTcpSession->dPort = tcpMsg->dPort;
						break;

					case DOWN:
						pTcpSession->sIpv4 = tcpMsg->dIp;
						pTcpSession->dIpv4 = tcpMsg->sIp;
						pTcpSession->sPort = tcpMsg->dPort;
						pTcpSession->dPort = tcpMsg->sPort;
						break;
				}
				break;
	}

	pTcpSession->isUpDir = tcpMsg->direction;
	pTcpSession->sliceCounter = 0;

	if(strlen(pTcpSession->httpAgent) == 0 && strlen(tcpMsg->httpAgent) > 0)
	{
		pTcpSession->httpAgent[0] = 0;
		strcpy(pTcpSession->httpAgent, tcpMsg->httpAgent);
	}

	pTcpSession->startTimeEpochSec = pTcpSession->pckArivalTimeEpochSec;
	pTcpSession->startTimeEpochNanoSec = pTcpSession->pckLastTimeEpochNanoSec;
	pTcpSession->pckLastTimeEpochSec = pTcpSession->pckArivalTimeEpochSec;
}

bool TcpSMInterface::checkDuplicate(tcpSession *pTcpSession, TcpPacket *tcpMsg)
{
	uint16_t 	ipId	= 0;
	uint8_t 	ttl 	= 0;
	uint8_t		dir 	= 0;
	bool 		got 	= false;

	std::map<uint32_t, dupInfo>::iterator it = pTcpSession->dupMap.find(tcpMsg->tcpSeqNo);

	pTcpSession->totalFrCount ++;

	if(it != pTcpSession->dupMap.end())
	{
		ipId = it->second.ipId;
		ttl = it->second.ttl;
		dir = it->second.direction;
		got = true;
	}
	else
	{
		pTcpSession->dupMap[tcpMsg->tcpSeqNo].ipId = tcpMsg->ipIdentification;
		pTcpSession->dupMap[tcpMsg->tcpSeqNo].ttl = tcpMsg->ipTtl;
		pTcpSession->dupMap[tcpMsg->tcpSeqNo].direction = tcpMsg->direction;
	}

	if(got)
	{
		if(dir == tcpMsg->direction)
		{
			if(ipId != tcpMsg->ipIdentification)
			{ pTcpSession->reTransmissionCnt++; }
			else
			{
				if(ttl != tcpMsg->ipTtl )
				{ pTcpSession->layer3LoopCnt++; }
				else
				{ pTcpSession->duplicateCnt++; }
			}
		}
	}
	return got;
}

bool TcpSMInterface::updateVPS(tcpSession *pTcpSession, TcpPacket *tcpMsg)
{
	fData *pFData = &pTcpSession->packTimeMap[pTcpSession->pckArivalTimeEpochSec];

	pFData->totalVolume += tcpMsg->pLoad;

	switch(tcpMsg->direction)
	{
		case UP:
				pFData->upPackets += 1;
				pFData->upVolume += tcpMsg->pLoad;
				break;

		case DOWN:
				if(pFData->dnPackets > SMGlobal::VPS_PACKET_PER_SEC)
					return false;

				pFData->dnPackets += 1;
				pFData->dnVolume += tcpMsg->pLoad;
				break;
	}
	return true;
}

void TcpSMInterface::timeStampArrivalPacket(tcpSession *pTcpSession, uint64_t epochSec, uint64_t epochNanoSec)
{
	pTcpSession->pckArivalTimeEpochSec 	= epochSec;
	pTcpSession->pckLastTimeEpochSec 	= epochSec;
	pTcpSession->pckLastTimeEpochNanoSec = epochNanoSec;
}

void TcpSMInterface::tcpUpdateSession(tcpSession *pTcpSession, TcpPacket *tcpMsg)
{
	uint16_t timeDiff = 0;

//	if(tcpMsg->tcpFlags != ACK_RCV)
//	{
		if(SMGlobal::CHECK_DUPLICATE_PKT)
			if(checkDuplicate(pTcpSession, tcpMsg)) return;

		if(tcpMsg->pLoad > 0)
			vpsFlag = updateVPS(pTcpSession, tcpMsg);

		if(!vpsFlag)
			tcpMsg->pLoad = 0;
//	}

//	pTcpSession->endTimeEpochNanoSec = pTcpSession->pckLastTimeEpochNanoSec;

	pTcpSession->frCount += 1;
	pTcpSession->frSize += tcpMsg->frSize;

	pTcpSession->pLoadPkt += 1;
	pTcpSession->pLoadSize +=  tcpMsg->pLoad;

	switch(tcpMsg->direction)
	{
		case UP:
				pTcpSession->upFrSize += tcpMsg->frSize;
				pTcpSession->upFrCount += 1;

				if(tcpMsg->pLoad > 0)
				{
					pTcpSession->upPLoadPkt += 1;
					pTcpSession->upPLoadSize += tcpMsg->pLoad;
				}
				break;

		case DOWN:
				pTcpSession->dnFrSize += tcpMsg->frSize;
				pTcpSession->dnFrCount += 1;

				if(tcpMsg->pLoad > 0)
				{
					pTcpSession->dnPLoadPkt += 1;
					pTcpSession->dnPLoadSize += tcpMsg->pLoad;
				}
				break;
	}

	/*
	 * Don't flush the TCP Session which don't have connection
	 */
	if((pTcpSession->frCount >= SMGlobal::SESSION_PKT_LIMIT))
	{
		if(pTcpSession->state == CONNECTED)
			pTcpSession->causeCode = SYSTEM_PKTLIMIT_TCP_CONN_DATA;
		else
			pTcpSession->causeCode = SYSTEM_PKTLIMIT_TCP_NOCONN_DATA;

		tcpFlushSession(TCP_LIMIT, pTcpSession, false);
		pTcpSession->reuse();
	}
	else
	{
		if(pTcpSession->pckLastTimeEpochSec > pTcpSession->startTimeEpochSec)
		{
			timeDiff = pTcpSession->pckLastTimeEpochSec - pTcpSession->startTimeEpochSec;

			if(timeDiff >= SMGlobal::SESSION_TIME_LIMIT)
			{
				if(pTcpSession->state == CONNECTED)
					pTcpSession->causeCode = SYSTEM_TIMEOUT_TCP_CONN_DATA;
				else
					pTcpSession->causeCode = SYSTEM_TIMEOUT_TCP_NOCONN_DATA;

				tcpFlushSession(4, pTcpSession, false);
				pTcpSession->reuse();
			}
		}
	}
}

void TcpSMInterface::tcpFlushSession(uint16_t flushOrgId, tcpSession *pTcpSession, bool erase)
{
	uint64_t epochSec = Global::CURRENT_EPOCH_SEC;
	uint64_t epochNanoSec = Global::CURRENT_EPOCH_NANO_SEC;

	pTcpSession->flushOrgId = flushOrgId;
	pTcpSession->flushTime = epochSec;
	pTcpSession->endTimeEpochNanoSec = epochNanoSec;

	tIdx = PKT_WRITE_TIME_INDEX(epochSec, Global::TIME_INDEX);

	/* If TCP Session Frame Count < 5 during clean up No XDR required */
	if(pTcpSession->pType == PACKET_IPPROTO_TCP)
	{
		if(pTcpSession->frCount < 5 && flushOrgId == TCP_CLEAN)
		{
			tcpEraseSession(pTcpSession);
			return;
		}
	}

	pTcpSession->sliceCounter += 1;
	tcpStoreSession(tIdx, pTcpSession);

	if(erase)
		tcpEraseSession(pTcpSession);
}

void TcpSMInterface::tcpStoreSession(uint16_t timeIndex, tcpSession *pTcpSession)
{
	flusherId = instanceId % SMGlobal::TCP_FLUSHER_INSTANCE;

	FlushRepo::tcpFlStore[flusherId][instanceId][timeIndex][FlushRepo::tcpFlCnt[flusherId][instanceId][timeIndex]].copy(pTcpSession);
	FlushRepo::tcpFlCnt[flusherId][instanceId][timeIndex]++;
}

void TcpSMInterface::tcpTimeOutClean(bool endOfDay)
{
	cleanUpCnt = 0;

	IPStats::tcpV4SessionTotalCnt[instanceId] = 0;
	IPStats::tcpV4SessionScanned[instanceId] 	= 0;
	IPStats::tcpV4SessionCleaned[instanceId] 	= 0;

	IPStats::tcpV6SessionTotalCnt[instanceId] = 0;
	IPStats::tcpV6SessionScanned[instanceId] 	= 0;
	IPStats::tcpV6SessionCleaned[instanceId] 	= 0;

	for(uint16_t i = 0; i < TCP_SESSION_POOL_ARRAY_ELEMENTS; i++)
	{
		IPStats::tcpV4SessionTotalCnt[instanceId] += tcpV4SessionMap[i].size();
		IPStats::tcpV6SessionTotalCnt[instanceId] += tcpV6SessionMap[i].size();
	}

	if(endOfDay)
	{
		for(uint16_t i = 0; i < TCP_SESSION_POOL_ARRAY_ELEMENTS; i++)
		{
			for(auto elem = tcpV4SessionMap[i].begin(), next_elem = elem; elem != tcpV4SessionMap[i].end(); elem = next_elem)
			{
				++next_elem;
				tcpCleanSession(tcpGetSessionFromPool(elem->second), endOfDay);
				IPStats::tcpV4SessionScanned[instanceId]++;
			}
		}

		IPStats::tcpV4SessionCleaned[instanceId] = cleanUpCnt;
		cleanUpCnt = 0;

		for(uint16_t i = 0; i < TCP_SESSION_POOL_ARRAY_ELEMENTS; i++)
		{
			for(auto elem = tcpV6SessionMap[i].begin(), next_elem = elem; elem != tcpV6SessionMap[i].end(); elem = next_elem)
			{
				++next_elem;
				tcpCleanSession(tcpGetSessionFromPool(elem->second), endOfDay);
				IPStats::tcpV6SessionScanned[instanceId]++;
			}
		}
		IPStats::tcpV6SessionCleaned[instanceId] = cleanUpCnt;
		cleanUpCnt = 0;
	}
	else
	{
		for(uint16_t i = 0; i < TCP_SESSION_POOL_ARRAY_ELEMENTS; i++)
		{
//			if(cleanUpCnt >= SMGlobal::SESSION_CLEAN_UP_BATCH_LIMIT) break;
			for(auto elem = tcpV4SessionMap[i].begin(), next_elem = elem; elem != tcpV4SessionMap[i].end(); elem = next_elem)
			{
				++next_elem;
//				if(cleanUpCnt >= SMGlobal::SESSION_CLEAN_UP_BATCH_LIMIT) break;
				tcpCleanSession(tcpGetSessionFromPool(elem->second), endOfDay);
				IPStats::tcpV4SessionScanned[instanceId]++ ;
			}
		}
		IPStats::tcpV4SessionCleaned[instanceId] = cleanUpCnt;
		cleanUpCnt = 0;

		for(uint16_t i = 0; i < TCP_SESSION_POOL_ARRAY_ELEMENTS; i++)
		{
//			if(cleanUpCnt >= SMGlobal::SESSION_CLEAN_UP_BATCH_LIMIT) break;
			for(auto elem = tcpV6SessionMap[i].begin(), next_elem = elem; elem != tcpV6SessionMap[i].end(); elem = next_elem)
			{
				++next_elem;
//				if(cleanUpCnt >= SMGlobal::SESSION_CLEAN_UP_BATCH_LIMIT) break;
				tcpCleanSession(tcpGetSessionFromPool(elem->second), endOfDay);
				IPStats::tcpV6SessionScanned[instanceId]++ ;
			}
		}
		IPStats::tcpV6SessionCleaned[instanceId] = cleanUpCnt;
		cleanUpCnt = 0;
	}
}

void TcpSMInterface::tcpCleanSession(tcpSession *pTcpSession, bool endOfDay)
{
	uint64_t curEpochSec = Global::CURRENT_EPOCH_SEC;

	if(endOfDay)
	{
		pTcpSession->causeCode = SYSTEM_CLEANUP_END_OF_DAY_TCP_DATA;
		tcpFlushSession(TCP_CLEAN, pTcpSession, true);
		cleanUpCnt++;

	}
	else
	{
		if((curEpochSec - pTcpSession->pckLastTimeEpochSec) > SMGlobal::TCP_CLEAN_UP_TIMEOUT_SEC)
		{
			if(pTcpSession->pType == PACKET_IPPROTO_TCP)
				pTcpSession->causeCode = SYSTEM_CLEANUP_TCP_DATA;

			cleanUpCnt++;

			tcpFlushSession(TCP_CLEAN, pTcpSession, true);
		}
	}
}

void TcpSMInterface::tcpEraseSession(tcpSession *pTcpSession)
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
	}
}

//void TcpSMInterface::TCPPacketEntry(TcpPacket *tcpMsg)
//{
//	bool found = false;
//	tcpSession *pTcpSession;
//
//	if(tcpMsg == NULL) return;
//
//	switch(tcpMsg->tcpFlags)
//	{
//		case  SYN_RCV:
//				pTcpSession = tcpGetSession(tcpMsg, &found, true);
//
//				/* Couldn't Create Session */
//				if(pTcpSession == NULL) return;
//
//				timeStampArrivalPacket(pTcpSession, tcpMsg->frTimeEpochSec, tcpMsg->frTimeEpochNanoSec);
//
//				/* Create New Session */
//				if(!found)
//				{
//					updateTime(pTcpSession, SYN_RCV);			/* Update Syn Time */
//					tcpInitializeSession(pTcpSession, tcpMsg);	/* Initialize TCP Packet */
//					uint32_t ip = pFUtility->getV4UserId(pTcpSession->sIpv4, pTcpSession->userId);
//					tcpUpdateSession(pTcpSession, tcpMsg);			/* Update TCP Packet */
//					return;
//				}
//
//				if(pTcpSession->synRcv)
//				{
//					tcpUpdateSession(pTcpSession, tcpMsg);		/* Update TCP Packet */
//					break;
//				}
//				else if(pTcpSession->synAckRcv && pTcpSession->ackRcv)
//				{
//					pTcpSession->state = CONNECTED;
//					updateTime(pTcpSession, SYN_RCV);			/* Update Syn Time */
//					tcpUpdateSession(pTcpSession, tcpMsg);		/* Update TCP Packet */
//				}
//				else if(pTcpSession->synAckRcv || pTcpSession->ackRcv)
//				{
//					updateTime(pTcpSession, SYN_RCV);			/* Update Syn Time */
//					tcpUpdateSession(pTcpSession, tcpMsg);			/* Update TCP Packet */
//				}
//				else
//				{
//					updateTime(pTcpSession, SYN_RCV);
//					tcpUpdateSession(pTcpSession, tcpMsg);
//				}
//
//				break;
//
//		case SYN_ACK_RCV:
//				pTcpSession = tcpGetSession(tcpMsg, &found, true);
//
//				/* Couldn't Create Session */
//				if(pTcpSession == NULL) return;
//
//				timeStampArrivalPacket(pTcpSession, tcpMsg->frTimeEpochSec, tcpMsg->frTimeEpochNanoSec);
//
//				/* Create New Session */
//				if(!found)
//				{
//					updateTime(pTcpSession, SYN_ACK_RCV);		/* Update Syn Ack Time */
//					tcpInitializeSession(pTcpSession, tcpMsg);
//					tcpUpdateSession(pTcpSession, tcpMsg);
//					return;
//				}
//
//				if(pTcpSession->synAckRcv)
//				{
//					tcpUpdateSession(pTcpSession, tcpMsg);
//					break;
//				}
//				else if (pTcpSession->synRcv && pTcpSession->ackRcv)
//				{
//					pTcpSession->state = CONNECTED;
//					updateTime(pTcpSession, SYN_ACK_RCV);			/* Update Syn Ack Time */
//					tcpUpdateSession(pTcpSession, tcpMsg);
//				}
//				else if (pTcpSession->synRcv || pTcpSession->ackRcv)
//				{
//					updateTime(pTcpSession, SYN_ACK_RCV);			/* Update Syn Ack Time */
//					tcpUpdateSession(pTcpSession, tcpMsg);
//				}
//				else
//				{
//					updateTime(pTcpSession, SYN_ACK_RCV);
//					tcpUpdateSession(pTcpSession, tcpMsg);
//				}
//				break;
//
//		case ACK_RCV:
//				/* Don't Process ACK Payload is Zero */
//				if(tcpMsg->pLoad == 0)
//					return;
//				else if(!SMGlobal::PROCESS_ACK)
//					return;
//
//				if(SMGlobal::ACK_CREATE_SESSION)
//					pTcpSession = tcpGetSession(tcpMsg, &found, true);
//				else
//					pTcpSession = tcpGetSession(tcpMsg, &found, false);
//
//				/* Couldn't Create Session */
//				if(pTcpSession == NULL) return;
//
//				timeStampArrivalPacket(pTcpSession, tcpMsg->frTimeEpochSec, tcpMsg->frTimeEpochNanoSec);
//
//				/* Create New Session */
//				if(!found)
//				{
//					updateTime(pTcpSession, ACK_RCV);						/* Update Syn Ack Time */
//					tcpInitializeSession(pTcpSession, tcpMsg);
//					tcpUpdateSession(pTcpSession, tcpMsg);
//					return;
//				}
//
//				if(pTcpSession->ackRcv)
//				{
//					tcpUpdateSession(pTcpSession, tcpMsg);
//					break;
//				}
//
//				else if (pTcpSession->synRcv && pTcpSession->synAckRcv)
//				{
//					pTcpSession->state = CONNECTED;
//					updateTime(pTcpSession, ACK_RCV);			/* Update Syn Ack Time */
//					tcpUpdateSession(pTcpSession, tcpMsg);
//				}
//				else if(pTcpSession->synRcv || pTcpSession->synAckRcv)
//				{
//					updateTime(pTcpSession, ACK_RCV);			/* Update Syn Ack Time */
//					tcpUpdateSession(pTcpSession, tcpMsg);
//				}
//				else
//				{
//					updateTime(pTcpSession, ACK_RCV);
//					tcpUpdateSession(pTcpSession, tcpMsg);
//				}
//				break;
//
//		case DATA_RCV:
//				pTcpSession = tcpGetSession(tcpMsg, &found, true);
//
//				/* Couldn't Create Session */
//				if(pTcpSession == NULL) return;
//
//				timeStampArrivalPacket(pTcpSession, tcpMsg->frTimeEpochSec, tcpMsg->frTimeEpochNanoSec);
//
//				if(!found)
//				{
//					updateTime(pTcpSession, DATA_RCV);						/* Update Syn Ack Time */
//					tcpInitializeSession(pTcpSession, tcpMsg);
//					tcpUpdateSession(pTcpSession, tcpMsg);
//					return;
//				}
//
//				if(pTcpSession->firstDataFlag == false)
//					updateTime(pTcpSession, DATA_RCV);						/* Update Syn Ack Time */
//
//				tcpUpdateSession(pTcpSession, tcpMsg);
//
//				break;
//
//		case FIN_RCV:
//				pTcpSession = tcpGetSession(tcpMsg, &found, false);
//
//				/* Couldn't Create Session */
//				if(pTcpSession == NULL) return;
//
//				timeStampArrivalPacket(pTcpSession, tcpMsg->frTimeEpochSec, tcpMsg->frTimeEpochNanoSec);
//				updateTime(pTcpSession, FIN_RCV);						/* Update Syn Ack Time */
//				tcpUpdateSession(pTcpSession, tcpMsg);
//				tcpFlushSession(3, pTcpSession, true);
//
//				break;
//
//		default:
//				break;
//
//	}
//	pTcpSession = NULL;
//}

