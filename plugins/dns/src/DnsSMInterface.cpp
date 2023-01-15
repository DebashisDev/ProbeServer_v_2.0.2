/*
 * DnsInterface.cpp
 *
 *  Created on: 29 Sep 2021
 *      Author: Debashis
 */

#include "DnsSMInterface.h"

DnsSMInterface::DnsSMInterface(uint16_t id)
{
	_thisLogLevel = 0;
	this->_name = "DnsSMInterface";
	this->setLogLevel(Log::theLog().level());

	this->instanceId 		= id;
	this->freeBitPosDns  	= 0;
	this->freeBitPosDnsMax 	= 0;

	dnsinitializeSessionPool();
}

DnsSMInterface::~DnsSMInterface()
{}

uint32_t DnsSMInterface::getFreeIndexDns()
{
	freeBitPosDns++;
	if(freeBitPosDns >= freeBitPosDnsMax) freeBitPosDns = 0;
	uint32_t arrId = freeBitPosDns / DNS_SESSION_POOL_ARRAY_ELEMENTS;
	uint32_t bitId = freeBitPosDns % DNS_SESSION_POOL_ARRAY_ELEMENTS;

	while(bitFlagsDnsSession[arrId].test(bitId))
	{
		freeBitPosDns++;
		if(freeBitPosDns >= freeBitPosDnsMax) freeBitPosDns = 0;
		arrId = freeBitPosDns / DNS_SESSION_POOL_ARRAY_ELEMENTS;
		bitId = freeBitPosDns % DNS_SESSION_POOL_ARRAY_ELEMENTS;
	}
	if(freeBitPosDns >= freeBitPosDnsMax){
		printf("[%d] getFreeIndexDns freeBitPosDns [%u] >= freeBitPosDnsMax [%u]\n",instanceId, freeBitPosDns, freeBitPosDnsMax);
	}
	bitFlagsDnsSession[arrId].set(bitId);
	return freeBitPosDns;
}

void DnsSMInterface::releaseIndexDns(uint32_t idx)
{
	uint32_t arrId = idx / DNS_SESSION_POOL_ARRAY_ELEMENTS;
	uint32_t bitId = idx % DNS_SESSION_POOL_ARRAY_ELEMENTS;
	dnsSessionPoolMap[arrId][bitId]->reset();
	dnsSessionPoolMap[arrId][bitId]->poolIndex = idx;
	bitFlagsDnsSession[arrId].reset(bitId);
}

void DnsSMInterface::dnsinitializeSessionPool()
{
	freeBitPosDnsMax = DNS_SESSION_POOL_ARRAY_ELEMENTS * DNS_SESSION_POOL_ARRAY_SIZE;

	printf("DnsSMInterface [%02d]	Initializing [%u] DNS Session Pool... ", instanceId, freeBitPosDnsMax);
	TheLog_nc_v2(Log::Info, name(),"     [%02d] Initializing [%u] DNS Session Pool...", instanceId, freeBitPosDnsMax);
	for(uint16_t i = 0; i < DNS_SESSION_POOL_ARRAY_SIZE; i++)
	{
		bitFlagsDnsSession[i].reset();
		for(uint16_t j = 0; j < DNS_SESSION_POOL_ARRAY_ELEMENTS; j++)
		{
			dnsSessionPoolMap[i][j] = new dnsSession();
			dnsSessionPoolMap[i][j]->poolIndex = (i * DNS_SESSION_POOL_ARRAY_ELEMENTS) + j;
		}
	}
	printf("Completed.\n");
	TheLog_nc_v2(Log::Info, name(),"     [%02d] Initializing [%u] DNS Session Pool Completed.", instanceId, freeBitPosDnsMax);
}

dnsSession* DnsSMInterface::getDnsSessionFromPool(uint32_t idx)
{
	uint32_t arrId = idx / DNS_SESSION_POOL_ARRAY_ELEMENTS;
	uint32_t bitId = idx % DNS_SESSION_POOL_ARRAY_ELEMENTS;
	return dnsSessionPoolMap[arrId][bitId];
}

//void DnsSMInterface::DnsPacketEntry(DnsPacket *dnsMsg)
//{
//	dnsSession*	pDnsSession;
//	uint64_t 	ipV4key;
//	string		ipV6key;
//
//	if(dnsMsg == NULL)
//		return;
//
//	switch(dnsMsg->qrFlag)
//	{
//		case QUERY:
//		{
//			switch(dnsMsg->ipVer)
//			{
//				case IPVersion4:
//				{
//					getIpv4DNSSessionKey(ipV4key, dnsMsg->sIp, dnsMsg->sPort, dnsMsg->dIp, dnsMsg->transactionId);
//
//					std::map<uint64_t, uint32_t>::iterator it4 = ipV4dnsSessionMap.find(ipV4key);
//
//					if(it4 != ipV4dnsSessionMap.end())
//					{
//						pDnsSession = getDnsSessionFromPool(it4->second);
//
//						if(pDnsSession->state == RESPONSE)
//						{
//							requestUpdateDnsSession(pDnsSession, dnsMsg);
//							pDnsSession->state = SUCCESS;
//							flushDnsSession(pDnsSession, DNS_FLUSH_RSP_REQ);
//							releaseIndexDns(pDnsSession->poolIndex);
//							ipV4dnsSessionMap.erase(ipV4key);
//							return;
//						}
//						else
//						{
//							uint32_t poolIndex = pDnsSession->poolIndex;
//							pDnsSession->reset();
//							pDnsSession->poolIndex = poolIndex;
//							pDnsSession->sessionV4Key = ipV4key;
//							requestUpdateDnsSession(pDnsSession, dnsMsg);
//						}
//					}
//					else
//					{
//						if((ipV4dnsSessionMap.size() + ipV6dnsSessionMap.size()) < freeBitPosDnsMax)
//						{
//							uint32_t poolIndex = getFreeIndexDns();
//							pDnsSession = getDnsSessionFromPool(poolIndex);
//							pDnsSession->reset();
//							pDnsSession->poolIndex = poolIndex;
//							requestUpdateDnsSession(pDnsSession, dnsMsg);
//							pDnsSession->sessionV4Key = ipV4key;
//							ipV4dnsSessionMap[pDnsSession->sessionV4Key] = poolIndex;
//						}
//					}
//				}
//				break;
//
//			default:
//				break;
//			}
//		}
//		break;
//
//		case RESPONSE:
//		{
//			switch(dnsMsg->ipVer)
//			{
//				case IPVersion4:
//				{
//					getIpv4DNSSessionKey(ipV4key, dnsMsg->dIp, dnsMsg->dPort, dnsMsg->sIp, dnsMsg->transactionId);
//
//					std::map<uint64_t, uint32_t>::iterator it4 = ipV4dnsSessionMap.find(ipV4key);
//
//					if(it4 != ipV4dnsSessionMap.end())
//					{
//						pDnsSession = getDnsSessionFromPool(it4->second);
//
//						if(pDnsSession->state == QUERY)
//						{
//							responseUpdateDnsSession(pDnsSession, dnsMsg);
//							pDnsSession->state = SUCCESS;
//							flushDnsSession(pDnsSession, DNS_FLUSH_REQ_RSP);
//							releaseIndexDns(pDnsSession->poolIndex);
//							ipV4dnsSessionMap.erase(ipV4key);
//						}
//						else
//						{
//							uint32_t poolIndex = pDnsSession->poolIndex;
//							pDnsSession->reset();
//							pDnsSession->poolIndex = poolIndex;
//							pDnsSession->sessionV4Key = ipV4key;
//							responseUpdateDnsSession(pDnsSession, dnsMsg);
//						}
//					}
//					else
//					{
//						if((ipV4dnsSessionMap.size() + ipV6dnsSessionMap.size()) < freeBitPosDnsMax)
//						{
//							uint32_t poolIndex = getFreeIndexDns();
//							pDnsSession = getDnsSessionFromPool(poolIndex);
//							pDnsSession->reset();
//							pDnsSession->poolIndex = poolIndex;
//							responseUpdateDnsSession(pDnsSession, dnsMsg);
//							pDnsSession->sessionV4Key = ipV4key;
//							ipV4dnsSessionMap[pDnsSession->sessionV4Key] = poolIndex;
//						}
//					}
//				}
//				break;
//
//				default:
//					break;
//			}
//		}
//		break;
//
//		default:
//			break;
//	}
//}

void DnsSMInterface::DnsPacketEntry(DnsPacket *dnsMsg)
{
	dnsSession*	pDnsSession;
	uint64_t 	ipV4key;
	string		ipV6key;

	if(dnsMsg == NULL)
		return;

	switch(dnsMsg->ipVer)
	{
		case IPVersion4:
				switch(dnsMsg->qrFlag)
				{
					case QUERY:
					{
						getIpv4DNSSessionKey(ipV4key, dnsMsg->sIp, dnsMsg->sPort, dnsMsg->dIp, dnsMsg->transactionId);

						std::map<uint64_t, uint32_t>::iterator it4 = ipV4dnsSessionMap.find(ipV4key);

						if(it4 != ipV4dnsSessionMap.end())
						{
							pDnsSession = getDnsSessionFromPool(it4->second);

							if(pDnsSession->state == RESPONSE)
							{
								requestUpdateDnsSession(pDnsSession, dnsMsg);
								pDnsSession->state = SUCCESS;
								flushDnsSession(pDnsSession, DNS_FLUSH_RSP_REQ);
								releaseIndexDns(pDnsSession->poolIndex);
								ipV4dnsSessionMap.erase(ipV4key);
							}
						}
						else
						{
							if((ipV4dnsSessionMap.size()) < freeBitPosDnsMax)
							{
								uint32_t poolIndex = getFreeIndexDns();
								pDnsSession = getDnsSessionFromPool(poolIndex);
								pDnsSession->reset();
								pDnsSession->poolIndex = poolIndex;
								requestUpdateDnsSession(pDnsSession, dnsMsg);
								pDnsSession->sessionV4Key = ipV4key;
								ipV4dnsSessionMap[pDnsSession->sessionV4Key] = poolIndex;
							}
						}
					}
					break;

					case RESPONSE:
					{
						getIpv4DNSSessionKey(ipV4key, dnsMsg->dIp, dnsMsg->dPort, dnsMsg->sIp, dnsMsg->transactionId);

						std::map<uint64_t, uint32_t>::iterator it4 = ipV4dnsSessionMap.find(ipV4key);

						if(it4 != ipV4dnsSessionMap.end())
						{
							pDnsSession = getDnsSessionFromPool(it4->second);

							if(pDnsSession->state == QUERY)
							{
								responseUpdateDnsSession(pDnsSession, dnsMsg);
								pDnsSession->state = SUCCESS;
								flushDnsSession(pDnsSession, DNS_FLUSH_REQ_RSP);
								releaseIndexDns(pDnsSession->poolIndex);
								ipV4dnsSessionMap.erase(ipV4key);
							}
						}
						else
						{
							if((ipV4dnsSessionMap.size()) < freeBitPosDnsMax)
							{
								uint32_t poolIndex = getFreeIndexDns();
								pDnsSession = getDnsSessionFromPool(poolIndex);
								pDnsSession->reset();
								pDnsSession->poolIndex = poolIndex;
								responseUpdateDnsSession(pDnsSession, dnsMsg);
								pDnsSession->sessionV4Key = ipV4key;
								ipV4dnsSessionMap[pDnsSession->sessionV4Key] = poolIndex;
							}
						}
					}
					break;

					default:
						break;
				}	/* Flag End */
				break;

			default:	/* Of Version */
				break;
	}	/* Version */
}

void DnsSMInterface::requestUpdateDnsSession(dnsSession *pDnsSession, DnsPacket *msgObj)
{
	pDnsSession->ipVer					= msgObj->ipVer;
	pDnsSession->transactionId 			= msgObj->transactionId;
	pDnsSession->queryStartEpochSec 	= msgObj->frTimeEpochSec;
	pDnsSession->queryStartEpochNanoSec = msgObj->frTimeEpochNanoSec;

	switch(msgObj->ipVer)
	{
		case IPVersion4:
				pDnsSession->sIpv4 	= msgObj->sIp;
				pDnsSession->dIpv4 	= msgObj->dIp;
				break;

		default:
				return;
				break;
	}

	pDnsSession->sPort 			= msgObj->sPort;
	pDnsSession->dPort 			= msgObj->dPort;
	pDnsSession->state 			= QUERY;

	pDnsSession->url[0] = 0;
	strcpy(pDnsSession->url, msgObj->url);
}

void DnsSMInterface::responseUpdateDnsSession(dnsSession *pDnsSession, DnsPacket *msgObj)
{
	pDnsSession->transactionId 			= msgObj->transactionId;
	pDnsSession->queryEndEpochSec 		= msgObj->frTimeEpochSec;
	pDnsSession->queryEndEpochNanoSec 	= msgObj->frTimeEpochNanoSec;

	if(msgObj->responseCode <= 26)
	{
		pDnsSession->errorCode 			= msgObj->responseCode;
		strcpy(pDnsSession->errorDesc, initSection::dnsErrorCode[msgObj->responseCode].c_str());
	}
	else
	{
		pDnsSession->errorCode 			= 24;
		strcpy(pDnsSession->errorDesc, initSection::dnsErrorCode[msgObj->responseCode].c_str());
	}

	pDnsSession->state 					= RESPONSE;

	if(strlen(pDnsSession->url) == 0 && strlen(msgObj->url) != 0)
	{
		pDnsSession->url[0] = 0;
		strcpy(pDnsSession->url, msgObj->url);
	}
}

void DnsSMInterface::getIpv4DNSSessionKey(uint64_t &key, uint32_t userAddrLong, uint16_t port, uint32_t destAddrLong, uint32_t dnsTransactionId)
{ key = (userAddrLong^4) + (port^3) + (destAddrLong^2) + (dnsTransactionId^1); }

void DnsSMInterface::getIpv6DNSSessionKey(std::string &key, char* userAddrLong, uint32_t dnsTransactionId, uint16_t port)
{ key = userAddrLong + to_string(port) + to_string(dnsTransactionId); }

void DnsSMInterface::flushDnsSession(dnsSession *pDnsSession, int type)
{
	uint64_t epochSecNow = Global::CURRENT_EPOCH_SEC;

	uint16_t idx = PKT_WRITE_TIME_INDEX(epochSecNow, Global::TIME_INDEX);

	pDnsSession->flushType = type;

	storeDnsSession(idx, pDnsSession);
}

void DnsSMInterface::storeDnsSession(uint16_t idx, dnsSession *pDnsSession)
{
	uint16_t flusherNo = instanceId % SMGlobal::DNS_FLUSHER_INSTANCE;

	FlushRepo::dnsFlStore[flusherNo][instanceId][idx][FlushRepo::dnsFlCnt[flusherNo][instanceId][idx]].copy(pDnsSession);
	FlushRepo::dnsFlCnt[flusherNo][instanceId][idx]++;
}

void DnsSMInterface::sessionTimeOutClean()
{
	uint16_t IdleTimeSec 	= SMGlobal::DNS_CLEAN_UP_TIMEOUT_SEC;
	uint64_t curEpochSec 	= Global::CURRENT_EPOCH_SEC;
	uint64_t nanoEpochSec 	= Global::CURRENT_EPOCH_NANO_SEC;
	uint16_t flushType = 0;

	dnsSessionCleanCnt = 0;

	IPStats::dnsV4SessionTotalCnt[instanceId] 	= 0;
	IPStats::dnsV4SessionScanned[instanceId] 	= 0;
	IPStats::dnsV4SessionCleaned[instanceId] 	= 0;

	IPStats::dnsV6SessionTotalCnt[instanceId] 	= 0;
	IPStats::dnsV6SessionScanned[instanceId] 	= 0;
	IPStats::dnsV6SessionCleaned[instanceId] 	= 0;

	IPStats::dnsV4SessionTotalCnt[instanceId] = ipV4dnsSessionMap.size();
	IPStats::dnsV6SessionTotalCnt[instanceId] = ipV6dnsSessionMap.size();

	for(auto elem : ipV4dnsSessionMap)
	{
		IPStats::dnsV4SessionScanned[instanceId]++;
		dnsSession *pDnsSession = getDnsSessionFromPool(elem.second);

		switch(pDnsSession->state)
		{
			case QUERY:
					if((pDnsSession->queryStartEpochSec > 0) && (curEpochSec - pDnsSession->queryStartEpochSec) > IdleTimeSec)
					{
						flushType = DNS_FLUSH_CLEANUP_REQ_RSP;
						pDnsSession->queryEndEpochSec = curEpochSec;
						pDnsSession->queryEndEpochNanoSec = nanoEpochSec;

						pDnsSession->causeCode = SYSTEM_CLEANUP_DNS_QUERY;
						pDnsSession->errorCode = SYSTEM_CLEANUP_DNS_QUERY;
						strcpy(pDnsSession->errorDesc, "No Response");

						if(strlen(pDnsSession->url) != 0)
							flushDnsSession(pDnsSession, flushType);

						dnsSessionCleanUpMap[dnsSessionCleanCnt].ipv4key = elem.first;
						dnsSessionCleanUpMap[dnsSessionCleanCnt].poolIndex = elem.second;
						dnsSessionCleanCnt++;
					}
					break;

			case RESPONSE:
					if((pDnsSession->queryEndEpochSec > 0) && (curEpochSec - pDnsSession->queryEndEpochSec) > IdleTimeSec)
					{
						dnsSessionCleanUpMap[dnsSessionCleanCnt].ipv4key = elem.first;
						dnsSessionCleanUpMap[dnsSessionCleanCnt].poolIndex = elem.second;
						dnsSessionCleanCnt++;
					}
					break;
		}
	}

	IPStats::dnsV4SessionCleaned[instanceId] = dnsSessionCleanCnt;

	for(uint32_t i = 0; i < dnsSessionCleanCnt; i++)
	{
		releaseIndexDns(dnsSessionCleanUpMap[i].poolIndex);
		ipV4dnsSessionMap.erase(dnsSessionCleanUpMap[i].ipv4key);
	}

	dnsSessionCleanUpMap.clear();
	dnsSessionCleanCnt = 0;
}


void DnsSMInterface::initializeResolvedIpv6()
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

				if(pch == NULL) break;

				ip = std::string(pch);

				pch = strtok (NULL, ",");

				if(pch == NULL) break;
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

void DnsSMInterface::dnsIpV4LookUpCount()
{
	IPStats::dnsLookupMapSize = 0;

	for(int i = 0; i < 10; i++)
		IPStats::dnsLookupMapSize += DNSGlobal::dnsLookUpMap[i].size();
}

