/*
 * UnUdpInterface.cpp
 *
 *  Created on: 16-Aug-2021
 *      Author: Debashis
 */

#include "UnUdpInterface.h"

UnUdpInterface::UnUdpInterface(uint16_t id)
{
	_thisLogLevel = 0;
	this->_name = "UnUnUdpInterface";
	this->setLogLevel(Log::theLog().level());
	this->instanceId = id;
	cleanUpCnt = 0;

	udpInitializeSessionPool();
	dnsinitializeSessionPool();

	ipV4Key = 0;
	ipV6Key = "";
}

UnUdpInterface::~UnUdpInterface()
{ }

uint32_t UnUdpInterface::udpGetFreeIndex()
{
	udpFreeBitPos++;
	if(udpFreeBitPos >= udpFreeBitPosMax) udpFreeBitPos = 0;
	int arrId = udpFreeBitPos / UNM_SESSION_POOL_ARRAY_ELEMENTS;
	int bitId = udpFreeBitPos % UNM_SESSION_POOL_ARRAY_ELEMENTS;

	while(udpBitFlagsSession[arrId].test(bitId)){
		udpFreeBitPos++;
		if(udpFreeBitPos >= udpFreeBitPosMax) udpFreeBitPos = 0;
		arrId = udpFreeBitPos / UNM_SESSION_POOL_ARRAY_ELEMENTS;
		bitId = udpFreeBitPos % UNM_SESSION_POOL_ARRAY_ELEMENTS;
	}
	if(udpFreeBitPos >= udpFreeBitPosMax){
		printf("[%02d] getFreeIndexIp freeBitPosIp [%u] >= freeBitPosIpMax [%u]\n",instanceId, udpFreeBitPos, udpFreeBitPosMax);
	}
	udpBitFlagsSession[arrId].set(bitId);
	return udpFreeBitPos;
}

void UnUdpInterface::udpReleaseIndex(uint32_t idx)
{
	uint32_t arrId = idx / UNM_SESSION_POOL_ARRAY_ELEMENTS;
	uint32_t bitId = idx % UNM_SESSION_POOL_ARRAY_ELEMENTS;

	udpSessionPoolMap[arrId][bitId]->reset();
	udpSessionPoolMap[arrId][bitId]->poolIndex = idx;
	udpBitFlagsSession[arrId].reset(bitId);
}

void UnUdpInterface::udpInitializeSessionPool()
{
	udpFreeBitPosMax = UNM_SESSION_POOL_ARRAY_ELEMENTS * UNM_SESSION_POOL_ARRAY_SIZE;

	printf("UnUdpInterface [%02d]	Initializing [%u] UNKNOWN UDP Session Pool... ", instanceId, udpFreeBitPosMax);
	TheLog_nc_v2(Log::Info, name(),"     [%02d] Initializing [%u] UDP Session Pool...", instanceId, udpFreeBitPosMax);

	for(uint16_t i = 0; i < UNM_SESSION_POOL_ARRAY_SIZE; i++)
	{
		udpBitFlagsSession[i].reset();
		for(uint16_t j = 0; j < UNM_SESSION_POOL_ARRAY_ELEMENTS; j++)
		{
			udpSessionPoolMap[i][j] = new udpSession();
			udpSessionPoolMap[i][j]->poolIndex = (i*UNM_SESSION_POOL_ARRAY_ELEMENTS) + j;
		}
	}
	printf("Completed.\n");
	TheLog_nc_v2(Log::Info, name(),"     [%02d] Initializing [%u] UNKNOWN UDP Session Pool Completed.", instanceId, udpFreeBitPosMax);
}

udpSession* UnUdpInterface::udpGetSessionFromPool(uint32_t idx)
{
	uint32_t arrId = idx / UNM_SESSION_POOL_ARRAY_ELEMENTS;
	uint32_t bitId = idx % UNM_SESSION_POOL_ARRAY_ELEMENTS;
	return udpSessionPoolMap[arrId][bitId];
}

uint32_t UnUdpInterface::getMapIndexAndSessionKey(UdpPacket *udpMsg)
{
	uint32_t mapIndex = 0;

    switch(udpMsg->ipVer)
    {
    	case IPVersion4:
				{
					ipV4Key = udpMsg->ipv4FlowId;
					mapIndex = udpMsg->sIp % UNM_SESSION_POOL_ARRAY_ELEMENTS;
				}
				break;

    	default:
    			break;
	}
	return mapIndex;
}

void UnUdpInterface::UDPPacketEntry(UdpPacket *udpMsg)
{
	bool found = false;

	udpSession *pUdpSession = udpGetSession(udpMsg, &found, true);

	/* Couldn't Create Session */
	if(pUdpSession == NULL) return;

	timeStampArrivalPacket(pUdpSession, udpMsg->frTimeEpochSec, udpMsg->frTimeEpochNanoSec);

	/* Create New Session */
	if(!found)
	{
		initializeUdpSession(pUdpSession, udpMsg);
		updateUdpSession(pUdpSession, udpMsg);
	}
	else
	{ updateUdpSession(pUdpSession, udpMsg); }

	pUdpSession = NULL;
}

udpSession* UnUdpInterface::udpGetSession(UdpPacket *udpMsg, bool *found, bool create)
{
	uint32_t sessionCnt = 0;
	udpSession *pUdpSession = NULL;
	uint32_t mapIndex = 0, poolIndex = 0;

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
				if(create)
				{
					for(uint16_t i = 0; i < UNM_SESSION_POOL_ARRAY_ELEMENTS; i++)
						sessionCnt += (udpV4SessionMap[i].size() + udpV6SessionMap[i].size());

					if(sessionCnt < udpFreeBitPosMax)
					{
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

void UnUdpInterface::initializeUdpSession(udpSession *pUdpSession, UdpPacket *udpMsg)
{
	pUdpSession->ipVer = udpMsg->ipVer;
	pUdpSession->pType = PACKET_IPPROTO_UDP;
	pUdpSession->state = UD_UDP_DATA;
	pUdpSession->sliceCounter = 0;

	switch(udpMsg->ipVer)
	{
	  case IPVersion4:
		  pUdpSession->sIpv4 = udpMsg->sIp;
		  pUdpSession->dIpv4 = udpMsg->dIp;
		  break;

	  default:
		  break;
	}

	pUdpSession->sPort = udpMsg->sPort;
	pUdpSession->dPort = udpMsg->dPort;

	pUdpSession->startTimeEpochSec 		= pUdpSession->pckArivalTimeEpochSec;
	pUdpSession->startTimeEpochNanoSec 	= pUdpSession->pckLastTimeEpochNanoSec;
}

void UnUdpInterface::updateUdpSession(udpSession *pUdpSession, UdpPacket *udpMsg)
{
	uint64_t timeDiff = 0;

	pUdpSession->frCount += 1;
	pUdpSession->frSize += udpMsg->frSize;

	if(udpMsg->pLoad > 0) {
		pUdpSession->pLoadPkt += 1;
		pUdpSession->pLoadSize +=  udpMsg->pLoad;
	}

	/** Check the Data Slicing **/
	if(pUdpSession->frCount >= SMGlobal::SESSION_PKT_LIMIT)
	{
		pUdpSession->causeCode = SYSTEM_PKTLIMIT_UDP_DATA;

		udpFlushSession(TCP_LIMIT, pUdpSession, false);
		pUdpSession->reuse();
	}
}

void UnUdpInterface::timeStampArrivalPacket(udpSession *pUdpSession, uint64_t epochSec, uint64_t epochNanoSec)
{
	pUdpSession->pckArivalTimeEpochSec 	= epochSec;
	pUdpSession->pckLastTimeEpochSec 	= epochSec;
	pUdpSession->pckLastTimeEpochNanoSec = epochNanoSec;
}

void UnUdpInterface::udpFlushSession(uint16_t flushOrgId, udpSession *pUdpSession, bool erase)
{
	uint64_t epochSec = Global::CURRENT_EPOCH_SEC;
	uint64_t epochNanoSec = Global::CURRENT_EPOCH_NANO_SEC;

	pUdpSession->flushOrgId = flushOrgId;
	pUdpSession->flushTime = epochSec;
	pUdpSession->endTimeEpochNanoSec = epochNanoSec;

	uint16_t idx = PKT_WRITE_TIME_INDEX(epochSec, Global::TIME_INDEX);

	pUdpSession->sliceCounter += 1;
	udpStoreSession(idx, pUdpSession);

	if(erase)
		udpEraseSession(pUdpSession);
}

void UnUdpInterface::udpStoreSession(uint16_t idx, udpSession *pUdpSession)
{
	UnFlushRepo::uUdpFlStore[instanceId][idx][UnFlushRepo::uUdpFlCnt[instanceId][idx]].copy(pUdpSession);
	UnFlushRepo::uUdpFlCnt[instanceId][idx]++;
}

void UnUdpInterface::udpTimeOutClean(bool endOfDay)
{
	uint32_t totalCount = 0;
	cleanUpCnt = 0;

	IPStats::unUdpSessionCnt[instanceId]		= 0;
	IPStats::unUdpSessionScanned[instanceId] 	= 0;
	IPStats::unUdpSessionCleaned[instanceId] 	= 0;

	for(uint16_t i = 0; i < UNM_SESSION_POOL_ARRAY_ELEMENTS; i++)
		totalCount += (udpV4SessionMap[i].size() + udpV6SessionMap[i].size());

	IPStats::unUdpSessionCnt[instanceId] = totalCount;

	if(endOfDay)
	{
		for(uint16_t i = 0; i < UNM_SESSION_POOL_ARRAY_ELEMENTS; i++)
		{
			for(auto elem = udpV4SessionMap[i].begin(), next_elem = elem; elem != udpV4SessionMap[i].end(); elem = next_elem)
			{
				++next_elem;
				udpCleanSession(udpGetSessionFromPool(elem->second), endOfDay);
				IPStats::unUdpSessionScanned[instanceId]++;
			}
		}

		IPStats::unUdpSessionCleaned[instanceId] = cleanUpCnt;
		cleanUpCnt = 0;

		for(uint16_t i = 0; i < UNM_SESSION_POOL_ARRAY_ELEMENTS; i++)
		{
			for(auto elem = udpV6SessionMap[i].begin(), next_elem = elem; elem != udpV6SessionMap[i].end(); elem = next_elem)
			{
				++next_elem;
				udpCleanSession(udpGetSessionFromPool(elem->second), endOfDay);
				IPStats::unUdpSessionScanned[instanceId]++;
			}
		}

		IPStats::unUdpSessionCleaned[instanceId] += cleanUpCnt;
		cleanUpCnt = 0;
	}
	else
	{
		for(uint16_t i = 0; i < UNM_SESSION_POOL_ARRAY_ELEMENTS; i++)
		{
//			if(cleanUpCnt >= SMGlobal::UDP_SESSION_CLEAN_UP_BATCH_LIMIT) break;		//Apply only for timeout

			for(auto elem = udpV4SessionMap[i].begin(), next_elem = elem; elem != udpV4SessionMap[i].end(); elem = next_elem)
			{
				++next_elem;
//				if(cleanUpCnt >= SMGlobal::UDP_SESSION_CLEAN_UP_BATCH_LIMIT) break;	//Apply only for timeout
				udpCleanSession(udpGetSessionFromPool(elem->second), endOfDay);
				IPStats::unUdpSessionScanned[instanceId]++;
			}
		}

		IPStats::unUdpSessionCleaned[instanceId] = cleanUpCnt;
		cleanUpCnt = 0;

		for(uint16_t i = 0; i < UNM_SESSION_POOL_ARRAY_ELEMENTS; i++)
		{
//			if(cleanUpCnt >= SMGlobal::UDP_SESSION_CLEAN_UP_BATCH_LIMIT) break;		//Apply only for timeout

			for(auto elem = udpV6SessionMap[i].begin(), next_elem = elem; elem != udpV6SessionMap[i].end(); elem = next_elem)
			{
				++next_elem;
//				if(cleanUpCnt >= SMGlobal::UDP_SESSION_CLEAN_UP_BATCH_LIMIT) break;	//Apply only for timeout
				udpCleanSession(udpGetSessionFromPool(elem->second), endOfDay);
				IPStats::unUdpSessionScanned[instanceId]++;
			}
		}

		IPStats::unUdpSessionCleaned[instanceId] += cleanUpCnt;
		cleanUpCnt = 0;
	}
}

void UnUdpInterface::udpCleanSession(udpSession *pUdpSession, bool endOfDay)
{
	uint64_t curEpochSec = Global::CURRENT_EPOCH_SEC;

	if(endOfDay)
	{
		pUdpSession->causeCode = SYSTEM_CLEANUP_END_OF_DAY_UDP_DATA;
		IPStats::unUdpSessionCleaned[instanceId]++;
		cleanUpCnt++;
		udpFlushSession(TCP_CLEAN, pUdpSession, true);
	}
	else
	{
		if((curEpochSec - pUdpSession->pckLastTimeEpochSec) > SMGlobal::UDP_CLEAN_UP_TIMEOUT_SEC)
		{
			pUdpSession->causeCode = SYSTEM_CLEANUP_UDP_DATA;
			IPStats::unUdpSessionCleaned[instanceId]++;
			cleanUpCnt++;
			udpFlushSession(TCP_CLEAN, pUdpSession, true);
		}
	}
}

void UnUdpInterface::udpEraseSession(udpSession *pUdpSession)
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

		default:
			break;
	}
}

/*
*  DNS Function Session
*/

uint32_t UnUdpInterface::getFreeIndexDns()
{
	freeBitPosDns++;
	if(freeBitPosDns >= freeBitPosDnsMax) freeBitPosDns = 0;
	int arrId = freeBitPosDns / DNS_SESSION_POOL_ARRAY_ELEMENTS;
	int bitId = freeBitPosDns % DNS_SESSION_POOL_ARRAY_ELEMENTS;

	while(bitFlagsDnsSession[arrId].test(bitId)){
		freeBitPosDns++;
		if(freeBitPosDns >= freeBitPosDnsMax) freeBitPosDns = 0;
		arrId = freeBitPosDns / DNS_SESSION_POOL_ARRAY_ELEMENTS;
		bitId = freeBitPosDns % DNS_SESSION_POOL_ARRAY_ELEMENTS;
	}
	if(freeBitPosDns >= freeBitPosDnsMax){
		printf("[%d] getFreeIndexDns freeBitPosDns [%d] >= freeBitPosDnsMax [%d]\n",instanceId, freeBitPosDns, freeBitPosDnsMax);
	}
	bitFlagsDnsSession[arrId].set(bitId);
	return freeBitPosDns;
}

void UnUdpInterface::releaseIndexDns(uint32_t idx)
{
	uint32_t arrId = idx / DNS_SESSION_POOL_ARRAY_ELEMENTS;
	uint32_t bitId = idx % DNS_SESSION_POOL_ARRAY_ELEMENTS;

	dnsSessionPoolMap[arrId][bitId]->reset();
	dnsSessionPoolMap[arrId][bitId]->poolIndex = idx;
	bitFlagsDnsSession[arrId].reset(bitId);
}

void UnUdpInterface::dnsinitializeSessionPool()
{
	freeBitPosDnsMax = DNS_SESSION_POOL_ARRAY_ELEMENTS * DNS_SESSION_POOL_ARRAY_SIZE;

	printf("UnUdpInterface [%02d]	Initializing [%d] DNS Session Pool... ", instanceId, freeBitPosDnsMax);
	TheLog_nc_v2(Log::Info, name(),"     [%02d] Initializing [%u] DNS Session Pool...", instanceId, freeBitPosDnsMax);

	for(uint16_t i = 0; i < DNS_SESSION_POOL_ARRAY_SIZE; i++)
	{
		bitFlagsDnsSession[i].reset();
		for(uint16_t j = 0; j < DNS_SESSION_POOL_ARRAY_ELEMENTS; j++)
		{
			dnsSessionPoolMap[i][j] = new dnsSession();
			dnsSessionPoolMap[i][j]->poolIndex = (i*DNS_SESSION_POOL_ARRAY_ELEMENTS) + j;
		}
	}
	printf("Completed.\n");
	TheLog_nc_v2(Log::Info, name(),"     [%02d] Initializing [%u] DNS Session Pool Completed.", instanceId, freeBitPosDnsMax);
}

dnsSession* UnUdpInterface::getDnsSessionFromPool(uint32_t idx)
{
	uint32_t arrId = idx / DNS_SESSION_POOL_ARRAY_ELEMENTS;
	uint32_t bitId = idx % DNS_SESSION_POOL_ARRAY_ELEMENTS;
	return dnsSessionPoolMap[arrId][bitId];
}

void UnUdpInterface::getIpv4DNSSessionKey(uint64_t &key, uint32_t userAddrLong, uint16_t port, uint32_t destAddrLong, uint32_t dnsTransactionId)
{ key = (userAddrLong^4) + (port^3) + (destAddrLong^2) + (dnsTransactionId^1); }

void UnUdpInterface::getIpv6DNSSessionKey(std::string &key, char* userAddrLong, uint32_t dnsTransactionId, uint16_t port)
{ key = userAddrLong + to_string(port) + to_string(dnsTransactionId); }

void UnUdpInterface::flushDnsSession(dnsSession *pDnsSession, uint16_t type)
{
	uint64_t epochSecNow = Global::CURRENT_EPOCH_SEC;

	uint16_t idx = PKT_WRITE_TIME_INDEX(epochSecNow, Global::TIME_INDEX);

	pDnsSession->flushType = type;

	storeDnsSession(idx, pDnsSession);
}

void UnUdpInterface::storeDnsSession(uint16_t idx, dnsSession *pDnsSession)
{
	UnFlushRepo::uDnsFlStore[instanceId][idx][UnFlushRepo::uDnsFlCnt[instanceId][idx]].copy(pDnsSession);
	UnFlushRepo::uDnsFlCnt[instanceId][idx]++;
}

void UnUdpInterface::dnsTimeOutClean()
{
	uint16_t IdleTimeSec = SMGlobal::DNS_CLEAN_UP_TIMEOUT_SEC;

	uint64_t curEpochSec = Global::CURRENT_EPOCH_SEC;
	uint16_t flushType = 0;

	dnsSessionCleanUpMap_cnt = 0;

	for(auto elem : ipV4dnsSessionMap)
	{
		dnsSession *pDnsSession = getDnsSessionFromPool(elem.second);

		if(((pDnsSession->queryStartEpochSec > 0) && ((curEpochSec - pDnsSession->queryStartEpochSec) > IdleTimeSec)) ||
				((pDnsSession->queryEndEpochSec > 0) && ((curEpochSec - pDnsSession->queryEndEpochSec) > IdleTimeSec)))
		{
			if(pDnsSession->queryStartEpochSec == 0 && pDnsSession->queryEndEpochSec != 0)
			{
				pDnsSession->queryStartEpochSec = pDnsSession->queryEndEpochSec;
				pDnsSession->queryStartEpochNanoSec = pDnsSession->queryEndEpochNanoSec;
				flushType = DNS_FLUSH_CLEANUP_RSP_NOREQ;
			}
			else if(pDnsSession->queryEndEpochSec == 0 && pDnsSession->queryStartEpochSec != 0)
			{
				pDnsSession->queryEndEpochSec = pDnsSession->queryStartEpochSec;
				pDnsSession->queryEndEpochNanoSec = pDnsSession->queryStartEpochNanoSec;
				flushType = DNS_FLUSH_CLEANUP_REQ_NORSP;
			}
			else
			{ flushType = DNS_FLUSH_CLEANUP_REQ_RSP; }

			pDnsSession->causeCode = SYSTEM_CLEANUP_DNS_QUERY;

			if(pDnsSession->state == QUERY)
			{
				pDnsSession->errorCode = SYSTEM_CLEANUP_DNS_QUERY;
				strcpy(pDnsSession->errorDesc, "No Response");
			}
			/* Flush only DNS Request Message (As No Response is only for Request) */
			if(strlen(pDnsSession->url) != 0 && pDnsSession->state != RESPONSE) flushDnsSession(pDnsSession, flushType);
			dnsSessionCleanUpMap[dnsSessionCleanUpMap_cnt].ipv4key = elem.first;
			dnsSessionCleanUpMap[dnsSessionCleanUpMap_cnt].poolIndex = elem.second;
			dnsSessionCleanUpMap_cnt++;
		}
	}

	for(uint32_t i = 0; i < dnsSessionCleanUpMap_cnt; i++)
	{
		releaseIndexDns(dnsSessionCleanUpMap[i].poolIndex);
		ipV4dnsSessionMap.erase(dnsSessionCleanUpMap[i].ipv4key);
	}

	dnsSessionCleanUpMap.clear();
	dnsSessionCleanUpMap_cnt = 0;

	/*** IPV6 ***/

	for(auto elem : ipV6dnsSessionMap)
	{
		dnsSession *pDnsSession = getDnsSessionFromPool(elem.second);

		if(((pDnsSession->queryStartEpochSec > 0) && ((curEpochSec - pDnsSession->queryStartEpochSec) > IdleTimeSec)) ||
				((pDnsSession->queryEndEpochSec > 0) && ((curEpochSec - pDnsSession->queryEndEpochSec) > IdleTimeSec)))
		{
			if(pDnsSession->queryStartEpochSec == 0 && pDnsSession->queryEndEpochSec != 0)
			{
				pDnsSession->queryStartEpochSec = pDnsSession->queryEndEpochSec;
				pDnsSession->queryStartEpochNanoSec = pDnsSession->queryEndEpochNanoSec;
				flushType = DNS_FLUSH_CLEANUP_RSP_NOREQ;
			}
			else if(pDnsSession->queryEndEpochSec == 0 && pDnsSession->queryStartEpochSec != 0)
			{
				pDnsSession->queryEndEpochSec = pDnsSession->queryStartEpochSec;
				pDnsSession->queryEndEpochNanoSec = pDnsSession->queryStartEpochNanoSec;
				flushType = DNS_FLUSH_CLEANUP_REQ_NORSP;
			}
			else
			{ flushType = DNS_FLUSH_CLEANUP_REQ_RSP; }

			pDnsSession->causeCode = SYSTEM_CLEANUP_DNS_QUERY;

			if(pDnsSession->state == QUERY)
			{
				pDnsSession->errorCode = SYSTEM_CLEANUP_DNS_QUERY;
				strcpy(pDnsSession->errorDesc, "No Response");
			}
			/* Flush only DNS Request Message (As No Response is only for Request) */
			if(strlen(pDnsSession->url) != 0 && pDnsSession->state != RESPONSE) flushDnsSession(pDnsSession, flushType);
			dnsSessionCleanUpMap[dnsSessionCleanUpMap_cnt].ipv6key = elem.first;
			dnsSessionCleanUpMap[dnsSessionCleanUpMap_cnt].poolIndex = elem.second;
			dnsSessionCleanUpMap_cnt++;
		}
	}

	for(uint32_t i = 0; i < dnsSessionCleanUpMap_cnt; i++)
	{
		releaseIndexDns(dnsSessionCleanUpMap[i].poolIndex);
		ipV6dnsSessionMap.erase(dnsSessionCleanUpMap[i].ipv6key);
	}

	dnsSessionCleanUpMap.clear();
	dnsSessionCleanUpMap_cnt = 0;
}

void UnUdpInterface::initializeResolvedIpv4()
{
	string filePath = SMGlobal::DATA_DIR[0] + "dnsIpv4data.csv";

	std::string line;
	std::string url;
	uint32_t ip;
	int loadedCnt = 0;
	char* pch;

	ifstream fp;

	fp.open(filePath.c_str());

	if(fp.fail())
	{
		TheLog_nc_v1(Log::Warn, name(),"SpectaProbe Error in Loading IPv4 DNS Lookup file [%s]", filePath.c_str());
	}
	else
	{
		while(!fp.eof())
		{
			getline(fp, line);
			if(!line.empty() && line.length() > 0)
			{
				pch = strtok ((char *)line.c_str(), ",");
				ip = atol(pch);

				pch = strtok (NULL, ",");
				url = std::string(pch);

				if(ip > 0)
				{
					loadedCnt++;
					DNSGlobal::dnsLookUpMap[ip % 10][ip] = url;
				}
			}
		}
		fp.close();
	}

	TheLog_nc_v2(Log::Debug, name(),"     Loaded [ %06d ] Records in Dns IPv4 Lookup Map from file [%s]", loadedCnt, filePath.c_str());
	printf("     Loaded [ %06d ] Records in Dns IPv4 Lookup Map from file [%s].\n", loadedCnt, filePath.c_str());
}

void UnUdpInterface::initializeResolvedIpv6()
{
	string filePath = SMGlobal::DATA_DIR[0] + "dnsIpv6data.csv";

	std::string line;
	std::string url;
	std::string ip;

	int loadedCnt = 0;
	char* pch;

	ifstream fp;

	fp.open(filePath.c_str());

	if(fp.fail())
	{
		TheLog_nc_v1(Log::Warn, name(),"     Error in Loading IPv6 DNS Lookup file [%s]", filePath.c_str());
	}
	else
	{
		while(!fp.eof())
		{
			getline(fp, line);

			if(!line.empty() && line.length() > 0)
			{
				pch = strtok ((char *)line.c_str(), ",");
				ip = std::string(pch);

				pch = strtok (NULL, ",");
				url = std::string(pch);

				DNSGlobal::dnsV6LookUpMap[ip] = url;
				loadedCnt ++;
			}
		}
		fp.close();
	}

	TheLog_nc_v2(Log::Debug, name(),"     Loaded [ %06d ] Records in Dns IPv6 Lookup Map from file [%s]", loadedCnt, filePath.c_str());
	printf("     Loaded [ %06d ] Records in Dns IPv6 Lookup Map from file [%s].\n", loadedCnt, filePath.c_str());
}

void UnUdpInterface::dnsIpV4LookUpCount()
{
	IPStats::dnsLookupMapSize = 0;

	for(int i = 0; i < 10; i++)
		IPStats::dnsLookupMapSize += DNSGlobal::dnsLookUpMap[i].size();

	TheLog_nc_v1(Log::Info, name(),"   DnsLookUp Size       IPv4: %08u", IPStats::dnsLookupMapSize);
}
