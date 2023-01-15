/*
 * FlusherUtility.cpp
 *
 *  Created on: 18 Mar 2021
 *      Author: Debashis
 */

#include "FUtility.h"

#include <netinet/in.h>
#include <netinet/ip6.h>
#include <arpa/inet.h>
#include <string.h>


FUtility::FUtility()
{ }

FUtility::~FUtility()
{ }

void FUtility::lockDnsMap()
{
	pthread_mutex_lock(&mapDnsLock::lockCount);
	while (mapDnsLock::count == 0)
		pthread_cond_wait(&mapDnsLock::nonzero, &mapDnsLock::lockCount);
	mapDnsLock::count = mapDnsLock::count - 1;
	pthread_mutex_unlock(&mapDnsLock::lockCount);
}

void FUtility::unLockDnsMap()
{
    pthread_mutex_lock(&mapDnsLock::lockCount);
    if (mapDnsLock::count == 0)
        pthread_cond_signal(&mapDnsLock::nonzero);
    mapDnsLock::count = mapDnsLock::count + 1;
    pthread_mutex_unlock(&mapDnsLock::lockCount);
}

void FUtility::lockAAAMap()
{
	    pthread_mutex_lock(&mapAAALock::lockCount);
	    while (mapAAALock::count == 0)
	        pthread_cond_wait(&mapAAALock::nonzero, &mapAAALock::lockCount);
	    mapAAALock::count = mapAAALock::count - 1;
	    pthread_mutex_unlock(&mapAAALock::lockCount);
}

void FUtility::unLockAAAMap()
{
    pthread_mutex_lock(&mapAAALock::lockCount);
    if (mapAAALock::count == 0)
        pthread_cond_signal(&mapAAALock::nonzero);
    mapAAALock::count = mapAAALock::count + 1;
    pthread_mutex_unlock(&mapAAALock::lockCount);
}

void FUtility::getTcpVPS(tcpSession *pTcpSession, char *dnVolPerSec, char *upVolPerSec)
{
	char timeBuffer[100];
	bool printUpvps = false;
	bool printDnvps = false;

	uint16_t tcpUpCount, tcpDnCount;
	uint32_t volume, c_pickThrPut, f_pickThrPut;
	uint32_t c_up_pickThrPut, f_up_pickThrPut, c_dn_pickThrPut, f_dn_pickThrPut;
	uint32_t upvolume, dnvolume;
	uint32_t tcpUpPacket, tcpDnPacket;
	uint32_t timeDiff = 0;

	upvolume = dnvolume = 0;
	tcpUpPacket = tcpDnPacket = 0;
	tcpUpCount = tcpDnCount = 0;
	uint32_t firstSec = 0;

	int packetCount = 0;
	volume = c_pickThrPut = f_pickThrPut = 0;
	c_up_pickThrPut = f_up_pickThrPut = c_dn_pickThrPut = f_dn_pickThrPut = 0;

	for(int i = 0; i < 10000; i++)
		vpsTimeKeys[i] = 0;

	for(auto elem = pTcpSession->packTimeMap.begin(), next_elem = elem; elem != pTcpSession->packTimeMap.end(); elem = next_elem)
	{
		++next_elem;

		packetCount ++;
		vpsTimeKeys[packetCount] = elem->first;
		volume = elem->second.totalVolume;
		upvolume = elem->second.upVolume;
		dnvolume = elem->second.dnVolume;

		c_pickThrPut = volume;
		if (c_pickThrPut > f_pickThrPut)
			f_pickThrPut = c_pickThrPut;

		if(upvolume > 0)
		{
			tcpUpCount += 1;
			tcpUpPacket = elem->second.upPackets;
			upvolume = elem->second.upVolume;
			c_up_pickThrPut = upvolume;

			if (c_up_pickThrPut > f_up_pickThrPut)
				f_up_pickThrPut = c_up_pickThrPut;
		}

		if(dnvolume > 0)
		{
			tcpDnCount += 1;
			tcpDnPacket = elem->second.dnPackets;
			dnvolume = elem->second.dnVolume;
			c_dn_pickThrPut = dnvolume;

			if (c_dn_pickThrPut > f_dn_pickThrPut)
				f_dn_pickThrPut = c_dn_pickThrPut;
		}

		if(packetCount == 1 )
		{
			firstSec = elem->first;
			dnVolPerSec[0] = 0;
			upVolPerSec[0] = 0;

			sprintf(dnVolPerSec, "%u=", firstSec);
			sprintf(upVolPerSec, "%u=", firstSec);
		}

		if(dnvolume > 0)
		{
			if(strlen(dnVolPerSec) < (VPS_MAX_LEN - VPS_SINGLE_ELEMENT_SIZE))
			{
				timeDiff = elem->first - firstSec;
				if(timeDiff >= 0) { // Checking the TP
					timeBuffer[0] = 0;
					sprintf(timeBuffer, "%d#%u#%u;", timeDiff, tcpDnPacket, dnvolume);	//Volume only, keep in Bytes
					strcat(dnVolPerSec, timeBuffer);
					timeBuffer[0] = 0;
					printDnvps = true;
				}
			}
		}

		if(upvolume > 0)
		{
			if(strlen(upVolPerSec) < (VPS_MAX_LEN - VPS_SINGLE_ELEMENT_SIZE))
			{
				timeDiff = elem->first - firstSec;
				if(timeDiff >= 0)
				{
					timeBuffer[0] = 0;
					sprintf(timeBuffer, "%d#%u#%u;", timeDiff, tcpUpPacket, upvolume);	//Volume only, keep in Bytes
					strcat(upVolPerSec, timeBuffer);
					timeBuffer[0] = 0;
					printUpvps = true;
				}
			}
		}
		dnvolume = upvolume = 0;
		pTcpSession->packTimeMap.erase(elem);
	}

	pTcpSession->pckTotalTimeSec = tcpUpCount + tcpDnCount;

	for(int i=0; i <= packetCount; i++)
	{
		pTcpSession->packTimeMap[vpsTimeKeys[i]].reset();
		pTcpSession->packTimeMap.erase(vpsTimeKeys[i]);
		vpsTimeKeys[i] = 0;
	}

	pTcpSession->packTimeMap.clear();

	if(tcpDnCount == 0 || printDnvps == false)
		strcpy(dnVolPerSec, "NULL");

	if(tcpUpCount == 0 || printUpvps == false)
		strcpy(upVolPerSec, "NULL");


	/* Pick Throughput / Sec */
	pTcpSession->peakSessionTP 	= f_pickThrPut * 8;		//Converting to bits per second
	pTcpSession->upPeakSessionTP = f_up_pickThrPut * 8;	//Converting to bits per second
	pTcpSession->dnPeakSessionTP = f_dn_pickThrPut * 8;	//Converting to bits per second

	if(packetCount > 0 && pTcpSession->frSize > 0)
		pTcpSession->sessionTP = (pTcpSession->frSize * 8) / packetCount;		//Converting to bps
	else
		pTcpSession->sessionTP = 0;

	if(tcpUpPacket > 0 && pTcpSession->upFrSize > 0)
		pTcpSession->upSessionTP = (pTcpSession->upFrSize * 8) / tcpUpCount;	//Converting to bps
	else
		pTcpSession->upSessionTP = 0;

	if(tcpDnPacket > 0 && pTcpSession->dnFrSize > 0)
		pTcpSession->dnSessionTP = (pTcpSession->dnFrSize * 8) / tcpDnCount;	//Converting to bps
	else
		pTcpSession->dnSessionTP = 0;

	if(!f_dn_pickThrPut)
		pTcpSession->dnPeakSessionTP = pTcpSession->dnSessionTP;

	if(!f_up_pickThrPut)
		pTcpSession->upPeakSessionTP = pTcpSession->upSessionTP;
}

void FUtility::buildTcpXdr(tcpSession *pTcpSession, char *xdr)
{
	string sessionKey = "";
	string url = "";
	xdr[0] = 0;

	sIp[0] = sIp[0] = 0;
	dnVolPerSec[0] = upVolPerSec[0] = 0;
	userIp[0] = userIdChar[0] = 0;

	strcpy(userId, "NA");
	strcpy(userOlt, "NA");
	strcpy(userPlan, "NA");
	strcpy(userPolicyPlan, "NA");

	std::size_t found;

	/* Get Volume / Sec */
	getTcpVPS(pTcpSession, dnVolPerSec, upVolPerSec);

	switch(pTcpSession->ipVer)
	{
		case IPVersion4:
		{
			sessionKey 	= to_string(pTcpSession->sessionIpV4Key);
			url 		= getResolvedIp4(pTcpSession->dIpv4);

			long2Ip(pTcpSession->sIpv4, sIp);
			long2Ip(pTcpSession->dIpv4, dIp);

			if(strlen(pTcpSession->userId) == 0)
			{
				uint32_t ip = getV4UserId(pTcpSession->sIpv4, pTcpSession->userId);
				if(strlen(pTcpSession->userId)== 0)
				{ long2Ip(pTcpSession->sIpv4, pTcpSession->userId); }
			}

			long2Ip(pTcpSession->sIpv4, userIp);

			strcpy(userOlt, "NA");
		}
		break;
	}

	if(pTcpSession->frSize <= 60 || pTcpSession->pLoadSize <= 60) return;

	if(!someTcpChecks(pTcpSession)) return;

	if((strcmp(dnVolPerSec, "NULL") == 0) && (pTcpSession->dnPLoadSize > 0)) return;
	if((strcmp(upVolPerSec, "NULL") == 0) && (pTcpSession->upPLoadSize > 0)) return;

	if(strlen(pTcpSession->httpAgent) < 2)
		strcpy(pTcpSession->httpAgent, "NA");


	sprintf(xdr, "%d,%d,%d,%s,"		// 01- Probe Id,       02- XDR Id, 		       03- Protocol Type,     04- Protocol Desc,
				 "%d,%s-%d,"		// 05- Dest Port,      06- Session Key,
				 "%s,%s,"			// 07- Source Mac,     08- Dest Mac,
				 "%s,%d,%s,%d,"		// 09- Source Ip,      10- Source Port,        11- Dest Ip,           12- Dest Port,
				 "%d,%d,"			// 13- VLAN Id, 	   14- Slice Counter,
				 "%d,%d,%d,"		// 15- Frame Cnt,      16- Up Frame Cnt,       17- Dn Frame Cnt,
				 "%u,%u,%u,"		// 18- Frame Size,     19- Up Frame Size,      20- Dn Frame Size,
				 "%d,%d,%d,"		// 21- Payload Pkt,    22- Up Payload Pkt,     23- Dn Payload Pkt,
				 "%u,%u,%u,"		// 24- Payload Size,   25- Up Payload Size,    26- Dn Payload Size,
				 "%lu,%lu,%d,"	// 27- Start Time,     28- End Time,           29- Pkt Total Time,
				 "%u,%u,"			// 30- Session TP,     31- Peak Session TP,
				 "%u,%u,%u,%u,"		// 32- Up Session TP,  33- Up Session Peak TP, 34- Dn Session TP,     35- Dn Session Peak TP,
				 "%u,%d,%s,%s,"		// 36- Syn Latency,	   37- Cause Code,         38- Content Type,      39- Dn Vol/Sec,
				 "%d,%lu,"			// 40- SynRcv,         41- SynRcv Nano Sec,
				 "%d,%lu,"			// 42- SynAckRcv,      43- SynAckRcv Nano Sec,
				 "%d,%lu,"			// 44- DataRcv,        45- DataRcv Nano Sec,
				 "%d,%lu,"			// 46- FinRcv,         47- FinRcv Nano Sec,
				 "%s,%d,%s,"		// 48- Up Vol/Sec,     49- App Ip, 				50- URL,
				 "%s,%s,%s,%s,%s,"	// 51- UserId,         52- User Plan,			53- User Policy	     54- User IP,     55- User Mac
				 "%d,%u,%d,%s,%d,"	// 56- Flush Id		   57- Flush time		    58- TTL			     59- OLT		   60- Session Id
				 "%s,%d,%d,"		// 61- User-Agent	   62- JitterUp				63- JitterDn
				 "%d,%d,%d,%d,"		// 64- ReTransmission  65- layer3LoopCnt		66- duplicateCnt	 67- totalFrameCount
				 "%d,%llu,%d",		// 68- ackRcv,         69- Ack Nano Sec			70- Ip version

			SMGlobal::PROBE_ID, XDR_ID_IP, pTcpSession->pType, initSection::protocolName[pTcpSession->pType].c_str(),
			pTcpSession->dPort, sessionKey.c_str(), pTcpSession->sliceCounter,
			"NA", "NA",
			sIp, pTcpSession->sPort, dIp, pTcpSession->dPort,
			pTcpSession->vLanId, pTcpSession->sliceCounter,
			pTcpSession->frCount, pTcpSession->upFrCount, pTcpSession->dnFrCount,
			pTcpSession->frSize, pTcpSession->upFrSize, pTcpSession->dnFrSize,
			pTcpSession->pLoadPkt, pTcpSession->upPLoadPkt, pTcpSession->dnPLoadPkt,
			pTcpSession->pLoadSize, pTcpSession->upPLoadSize, pTcpSession->dnPLoadSize,
			pTcpSession->startTimeEpochNanoSec, pTcpSession->endTimeEpochNanoSec, pTcpSession->pckTotalTimeSec,
			pTcpSession->sessionTP, pTcpSession->peakSessionTP,
			pTcpSession->upSessionTP, pTcpSession->upPeakSessionTP, pTcpSession->dnSessionTP, pTcpSession->dnPeakSessionTP,
			0, pTcpSession->causeCode, "NULL", dnVolPerSec,
			pTcpSession->synRcv, pTcpSession->synTimeEpochNanoSec,
			pTcpSession->synAckRcv, pTcpSession->synAckTimeEpochNanoSec,
			pTcpSession->dataRcv, pTcpSession->firstDataTimeEpochNanoSec,
			pTcpSession->finRcv, pTcpSession->finTimeEpochNanoSec,
			upVolPerSec, 0, url.c_str(),
			pTcpSession->userId, userPlan, userPolicyPlan, userIp, "NA",
			pTcpSession->flushOrgId, pTcpSession->flushTime, pTcpSession->TTL, userOlt, pTcpSession->smInstanceId,
			pTcpSession->httpAgent, jitterUp, jitterDn,
			pTcpSession->reTransmissionCnt, pTcpSession->layer3LoopCnt, pTcpSession->duplicateCnt, pTcpSession->totalFrCount,
			pTcpSession->ackRcv, pTcpSession->ackTimeEpochNanoSec, pTcpSession->ipVer);
}

bool FUtility::someTcpChecks(tcpSession *pTcpSession)
{
	bool xdrProcess = true;

	/* These Checkes are for Spike in TP */
	if(pTcpSession->frSize < pTcpSession->pLoadSize)
	{
		xdrProcess = false;
		return xdrProcess;
	}

	if(!pTcpSession->synRcv)
	{
		pTcpSession->synRcv = true;
		pTcpSession->synTimeEpochNanoSec = pTcpSession->startTimeEpochNanoSec;
	}

	if((pTcpSession->synAckRcv) && (!pTcpSession->ackRcv))
	{
		pTcpSession->ackRcv = true;
		pTcpSession->ackTimeEpochNanoSec = pTcpSession->synAckTimeEpochNanoSec + 10;
	}
	else if((!pTcpSession->synAckRcv) && (pTcpSession->ackRcv))
	{
		pTcpSession->synAckRcv = true;
		pTcpSession->synAckTimeEpochNanoSec = pTcpSession->synTimeEpochNanoSec + 10;
	}
	else if((!pTcpSession->synAckRcv) && (!pTcpSession->ackRcv))
	{
		pTcpSession->synAckRcv = true;
		pTcpSession->synAckTimeEpochNanoSec = pTcpSession->synTimeEpochNanoSec + 10;
		pTcpSession->ackRcv = true;
		pTcpSession->ackTimeEpochNanoSec = pTcpSession->synAckTimeEpochNanoSec + 10;
	}

	if(pTcpSession->dataRcv)
		swap4(&pTcpSession->synTimeEpochNanoSec, &pTcpSession->synAckTimeEpochNanoSec, &pTcpSession->ackTimeEpochNanoSec, &pTcpSession->firstDataTimeEpochNanoSec);
	else
		swap3(&pTcpSession->synTimeEpochNanoSec, &pTcpSession->synAckTimeEpochNanoSec, &pTcpSession->ackTimeEpochNanoSec);

	return xdrProcess;
}

void FUtility::getUdpVPS(udpSession *pUdpSession, char *dnVolPerSec, char *upVolPerSec)
{
	char timeBuffer[100];
	bool printUpvps = false;
	bool printDnvps = false;

	uint16_t tcpUpCount, tcpDnCount;
	uint32_t volume, c_pickThrPut, f_pickThrPut;
	uint32_t c_up_pickThrPut, f_up_pickThrPut, c_dn_pickThrPut, f_dn_pickThrPut;
	uint32_t upvolume, dnvolume;
	uint32_t tcpUpPacket, tcpDnPacket;
	uint32_t timeDiff = 0;

	upvolume = dnvolume = 0;
	tcpUpPacket = tcpDnPacket = 0;
	tcpUpCount = tcpDnCount = 0;
	uint32_t firstSec = 0;

	int packetCount = 0;
	volume = c_pickThrPut = f_pickThrPut = 0;
	c_up_pickThrPut = f_up_pickThrPut = c_dn_pickThrPut = f_dn_pickThrPut = 0;

	for(int i = 0; i < 10000; i++)
		vpsTimeKeys[i] = 0;

	for(auto elem = pUdpSession->packTimeMap.begin(), next_elem = elem; elem != pUdpSession->packTimeMap.end(); elem = next_elem)
	{
		++next_elem;

		packetCount ++;
		vpsTimeKeys[packetCount] = elem->first;
		volume = elem->second.totalVolume;
		upvolume = elem->second.upVolume;
		dnvolume = elem->second.dnVolume;

		c_pickThrPut = volume;
		if (c_pickThrPut > f_pickThrPut)
			f_pickThrPut = c_pickThrPut;

		if(upvolume > 0)
		{
			tcpUpCount += 1;
			tcpUpPacket = elem->second.upPackets;
			upvolume = elem->second.upVolume;
			c_up_pickThrPut = upvolume;

			if (c_up_pickThrPut > f_up_pickThrPut)
				f_up_pickThrPut = c_up_pickThrPut;
		}

		if(dnvolume > 0)
		{
			tcpDnCount += 1;
			tcpDnPacket = elem->second.dnPackets;
			dnvolume = elem->second.dnVolume;
			c_dn_pickThrPut = dnvolume;

			if (c_dn_pickThrPut > f_dn_pickThrPut)
				f_dn_pickThrPut = c_dn_pickThrPut;
		}

		if(packetCount == 1 )
		{
			firstSec = elem->first;
			dnVolPerSec[0] = 0;
			upVolPerSec[0] = 0;

			sprintf(dnVolPerSec, "%u=", firstSec);
			sprintf(upVolPerSec, "%u=", firstSec);
		}

		if(dnvolume > 0)
		{
			if(strlen(dnVolPerSec) < (VPS_MAX_LEN - VPS_SINGLE_ELEMENT_SIZE))
			{
				timeDiff = elem->first - firstSec;
				if(timeDiff >= 0) { // Checking the TP
					timeBuffer[0] = 0;
					sprintf(timeBuffer, "%d#%u#%u;", timeDiff, tcpDnPacket, dnvolume);	//Volume only, keep in Bytes
					strcat(dnVolPerSec, timeBuffer);
					timeBuffer[0] = 0;
					printDnvps = true;
				}
			}
		}

		if(upvolume > 0)
		{
			if(strlen(upVolPerSec) < (VPS_MAX_LEN - VPS_SINGLE_ELEMENT_SIZE))
			{
				timeDiff = elem->first - firstSec;
				if(timeDiff >= 0)
				{
					timeBuffer[0] = 0;
					sprintf(timeBuffer, "%d#%u#%u;", timeDiff, tcpUpPacket, upvolume);	//Volume only, keep in Bytes
					strcat(upVolPerSec, timeBuffer);
					timeBuffer[0] = 0;
					printUpvps = true;
				}
			}
		}
		dnvolume = upvolume = 0;
		pUdpSession->packTimeMap.erase(elem);
	}

	pUdpSession->pckTotalTimeSec = tcpUpCount + tcpDnCount;

	for(int i=0; i <= packetCount; i++)
	{
		pUdpSession->packTimeMap[vpsTimeKeys[i]].reset();
		pUdpSession->packTimeMap.erase(vpsTimeKeys[i]);
		vpsTimeKeys[i] = 0;
	}

	pUdpSession->packTimeMap.clear();

	if(tcpDnCount == 0 || printDnvps == false)
		strcpy(dnVolPerSec, "NULL");

	if(tcpUpCount == 0 || printUpvps == false)
		strcpy(upVolPerSec, "NULL");


	/* Pick Throughput / Sec */
	pUdpSession->peakSessionTP 	= f_pickThrPut * 8;		//Converting to bits per second
	pUdpSession->upPeakSessionTP = f_up_pickThrPut * 8;	//Converting to bits per second
	pUdpSession->dnPeakSessionTP = f_dn_pickThrPut * 8;	//Converting to bits per second

	if(packetCount > 0 && pUdpSession->frSize > 0)
		pUdpSession->sessionTP = (pUdpSession->frSize * 8) / packetCount;		//Converting to bps
	else
		pUdpSession->sessionTP = 0;

	if(tcpUpPacket > 0 && pUdpSession->upFrSize > 0)
		pUdpSession->upSessionTP = (pUdpSession->upFrSize * 8) / tcpUpCount;	//Converting to bps
	else
		pUdpSession->upSessionTP = 0;

	if(tcpDnPacket > 0 && pUdpSession->dnFrSize > 0)
		pUdpSession->dnSessionTP = (pUdpSession->dnFrSize * 8) / tcpDnCount;	//Converting to bps
	else
		pUdpSession->dnSessionTP = 0;

	if(!f_dn_pickThrPut)
		pUdpSession->dnPeakSessionTP = pUdpSession->dnSessionTP;

	if(!f_up_pickThrPut)
		pUdpSession->upPeakSessionTP = pUdpSession->upSessionTP;
}

void FUtility::buildUdpXdr(udpSession *pUdpSession, char *xdr)
{
	string sessionKey = "";
	string url = "NULL";

	sIp[0] = sIp[0] = 0;
	dnVolPerSec[0] = upVolPerSec[0] = 0;
	userIp[0] = userIdChar[0] = 0;

	strcpy(userId, "NA");
	strcpy(userOlt, "NA");
	strcpy(userPlan, "NA");
	strcpy(userPolicyPlan, "NA");

	xdr[0] = 0;

	std::size_t found;

	if(pUdpSession->startTimeEpochNanoSec == 0) return;

	/* Get Volume / Sec */
	getUdpVPS(pUdpSession, dnVolPerSec, upVolPerSec);

	/* get URL */
	switch(pUdpSession->ipVer)
	{
		case IPVersion4:
		{
			sessionKey = to_string(pUdpSession->sessionIpV4Key);

			url = getResolvedIp4(pUdpSession->dIpv4);

			long2Ip(pUdpSession->sIpv4, sIp);
			long2Ip(pUdpSession->dIpv4, dIp);

			if(strlen(pUdpSession->userId) == 0)
			{
				uint32_t ip = getV4UserId(pUdpSession->sIpv4, pUdpSession->userId);
				if(strlen(pUdpSession->userId)== 0)
				{ long2Ip(pUdpSession->sIpv4, pUdpSession->userId); }
			}

			long2Ip(pUdpSession->sIpv4, userIp);

			strcpy(userOlt, "NA");
		}
		break;
	}

	if(pUdpSession->frSize <= 60 && pUdpSession->pLoadSize <= 60) return;

	if(!someUdpChecks(pUdpSession)) return;

	if((strcmp(dnVolPerSec, "NULL") == 0) && (pUdpSession->dnPLoadSize > 0)) return;
	if((strcmp(upVolPerSec, "NULL") == 0) && (pUdpSession->upPLoadSize > 0)) return;

	sprintf(xdr, "%d,%d,%d,%s,"		// 01- Probe Id,       02- XDR Id, 		       03- Protocol Type,     04- Protocol Desc,
				 "%d,%s-%d,"		// 05- Dest Port,      06- Session Key,
				 "%s,%s,"			// 07- Source Mac,     08- Dest Mac,
				 "%s,%d,%s,%d,"		// 09- Source Ip,      10- Source Port,        11- Dest Ip,           12- Dest Port,
				 "%d,%d,"			// 13- VLAN Id, 	   14- Slice Counter,
				 "%d,%d,%d,"		// 15- Frame Cnt,      16- Up Frame Cnt,       17- Dn Frame Cnt,
				 "%u,%u,%u,"		// 18- Frame Size,     19- Up Frame Size,      20- Dn Frame Size,
				 "%d,%d,%d,"		// 21- Payload Pkt,    22- Up Payload Pkt,     23- Dn Payload Pkt,
				 "%u,%u,%u,"		// 24- Payload Size,   25- Up Payload Size,    26- Dn Payload Size,
				 "%lu,%lu,%d,"		// 27- Start Time,     28- End Time,           29- Pkt Total Time,
				 "%u,%u,"			// 30- Session TP,     31- Peak Session TP,
				 "%u,%u,%u,%u,"		// 32- Up Session TP,  33- Up Session Peak TP, 34- Dn Session TP,     35- Dn Session Peak TP,
				 "%u,%d,%s,%s,"		// 36- Syn Latency,	   37- Cause Code,         38- Content Type,      39- Dn Vol/Sec,
				 "%d,%lu,"			// 40- SynRcv,         41- SynRcv Nano Sec,
				 "%d,%lu,"			// 42- SynAckRcv,      43- SynAckRcv Nano Sec,
				 "%d,%lu,"			// 44- DataRcv,        45- DataRcv Nano Sec,
				 "%d,%lu,"			// 46- FinRcv,         47- FinRcv Nano Sec,
				 "%s,%u,%s,"		// 48- Up Vol/Sec,     49- App Ip, 				50- URL,
				 "%s,%s,%s,%s,%s,"	// 51- UserId,         52- User Plan,			53- User Policy	     54- User IP,     55- User Mac
				 "%d,%u,%d,%s,%d,"	// 56- Flush Id		   57- Flush time		    58- TTL			     59- OLT		   60- Session Id
				 "%s,%d,%d,"		// 61- User-Agent	   62- JitterUp				63- JitterDn
				 "%d,%d,%d,%d,"		// 64- ReTransmission  65- layer3LoopCnt		66- duplicateCnt	 67- totalFrameCount
				 "%d,%llu,%d",		// 68- ackRcv,         69- Ack Nano Sec			70- Ip version

			SMGlobal::PROBE_ID, XDR_ID_IP, pUdpSession->pType, initSection::protocolName[pUdpSession->pType].c_str(),
			pUdpSession->dPort, sessionKey.c_str(), pUdpSession->sliceCounter,
			"NA", "NA",
			sIp, pUdpSession->sPort, dIp, pUdpSession->dPort,
			pUdpSession->vLanId, pUdpSession->sliceCounter,
			pUdpSession->frCount, pUdpSession->upFrCount, pUdpSession->dnFrCount,
			pUdpSession->frSize, pUdpSession->upFrSize, pUdpSession->dnFrSize,
			pUdpSession->pLoadPkt, pUdpSession->upPLoadPkt, pUdpSession->dnPLoadPkt,
			pUdpSession->pLoadSize, pUdpSession->upPLoadSize, pUdpSession->dnPLoadSize,
			pUdpSession->startTimeEpochNanoSec, pUdpSession->endTimeEpochNanoSec, pUdpSession->pckTotalTimeSec,
			pUdpSession->sessionTP, pUdpSession->peakSessionTP,
			pUdpSession->upSessionTP, pUdpSession->upPeakSessionTP, pUdpSession->dnSessionTP, pUdpSession->dnPeakSessionTP,
			0, pUdpSession->causeCode, "NULL", dnVolPerSec,
			0, 0,
			0, 0,
			0, 0,
			0, 0,
			upVolPerSec, 0, url.c_str(),
			pUdpSession->userId, userPlan, userPolicyPlan, userIp, "NA",
			pUdpSession->flushOrgId, pUdpSession->flushTime, 0, userOlt, pUdpSession->smInstanceId,
			"NA", jitterUp, jitterDn,
			0, 0, 0, pUdpSession->totalFrCount,
			0, 0, pUdpSession->ipVer);
}

bool FUtility::someUdpChecks(udpSession *pUdpSession)
{
	bool xdrProcess = true;

	/* These Checkes are for Spike in TP */
	if(pUdpSession->frSize < pUdpSession->pLoadSize)
	{
		xdrProcess = false;
		return xdrProcess;
	}
	return xdrProcess;
}

void FUtility::swap3(uint64_t *a, uint64_t *b, uint64_t *c)
{
	uint64_t lr, mi, sm;

	if(*a > *b)
	{
		mi = *a;
		sm = *b;
	}
	else
	{
		mi = *b;
		sm = *a;
	}

	if(mi > *c)
	{
		lr = mi;
		if(sm > *c)
		{
			mi = sm;
			sm = *c;
		}
		else
		{
			mi = *c;
		}
	}
	else
		lr = *c;

	*a = sm;
	*b = mi;
	*c = lr;
}

void FUtility::swap4(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t *d)
{
	uint64_t temp = 0;

	if(*a > *b)
	{
		temp = *a;
		*a = *b;
		*b = temp;
	}
	if(*c > *d)
	{
		temp = *c;
		*c = *d;
		*d = temp;
	}
	if(*a > *c)
	{
		temp = *a;
		*a = *c;
		*c = temp;
	}
	if(*b > *d)
	{
		temp = *b;
		*b = *d;
		*d = temp;
	}
	if(*b > *c)
	{
		temp = *b;
		*b = *c;
		*c = temp;
	}
}

void FUtility::buildDnsXdr(dnsSession *pDnsSession, char *csvXDR)
{
	char userId[IPV6_ADDR_LEN];

	uint32_t dnsResTimeMilliSec = 0;
	string sessionKey = "";

	userId[0] = 0;

	uint64_t sTime = pDnsSession->queryStartEpochNanoSec;
	uint64_t eTime = pDnsSession->queryEndEpochNanoSec;

	csvXDR[0] = 0;

	if(eTime > 0 && sTime > 0 && (eTime > sTime)) {
		if(sTime > 1000000) {
			sTime = sTime / (1000 * 1000);
			if(eTime > 1000000) {
				eTime = eTime / (1000 * 1000);
				dnsResTimeMilliSec = (uint32_t) (eTime - sTime);
			}
		}
	}

	switch(pDnsSession->ipVer)
	{
		case IPVersion4:
			sessionKey = to_string(pDnsSession->sessionV4Key);
			long2Ip(pDnsSession->sIpv4, pDnsSession->sIpv6);
			long2Ip(pDnsSession->dIpv4, pDnsSession->dIpv6);

			getV4UserId(pDnsSession->sIpv4, userId);

			if(strlen(userId) == 0)
				long2Ip(pDnsSession->sIpv4, userId);

			break;

		case IPVersion6:
			sessionKey = pDnsSession->sessionV6Key;
			break;
	}

	if(!checkNewLine(pDnsSession->url))
			return;

	if(strlen(pDnsSession->url) <= 5 || strlen(pDnsSession->url) > 50)
		return;

	if(strstr(pDnsSession->errorDesc, "No Error") != NULL)
		pDnsSession->errorCode = 0;

	sprintf(csvXDR, "%d,%d,17,DNS,"			// 1- Probe Id			2- XDR Id		3- UDP				4-  DNS
					"%s,%s,%d,%s,%d,"		// 5- User Id			6- Source Ip	7- Source Port		8-  Dest Ip		9- Dest Port
					"%s,%d,%s,"				// 10- URL				11- Error Code	12- Error Desc
					"%s,"					// 13- Address
					"%lu,%lu,%u,%s,"		// 14- Start time		15- End Time	16- Resolve Time    17- OLT
					"%s,%s,%s,%d,%s",		// 18- User Policy		19- User Plan	20- User Mac		21- Flush Type
					SMGlobal::PROBE_ID, XDR_ID_DNS,
					userId, pDnsSession->sIpv6, pDnsSession->sPort, pDnsSession->dIpv6, pDnsSession->dPort,
					pDnsSession->url, pDnsSession->errorCode, pDnsSession->errorDesc,
					"NA",
					pDnsSession->queryStartEpochNanoSec, pDnsSession->queryEndEpochNanoSec, dnsResTimeMilliSec, "NA",
					"NA", "NA", "NA", pDnsSession->flushType, sessionKey.c_str());
}

uint32_t FUtility::getV4UserId(uint32_t &sourceIP, char* userId)
{
	uint32_t userIp = 0;
	bool ipFound = false;

	/* Get User Name against User IP */
	lockAAAMap();

	std::map<uint32_t, userInfo>::iterator it = aaaGlbMap::aaaGlbUserIpMap.find(sourceIP);
	if(it != aaaGlbMap::aaaGlbUserIpMap.end())
	{
		userIp 		= it->first;
		strcpy(userId, it->second.userName);
	}
	unLockAAAMap();

	return userIp;
}

void FUtility::tcpGetV6UserId(
		char *sourceIP,
		char *destIP,
		char* userId,
		char* userPolicyPlan,
		char* userPlan,
		char* userMac,
		char* userOlt)
{
//	lockRadiusMap();
//
//	if(strlen(sourceIP) < 16)
//		return;
//
//	std::map<std::string, userInfo>::iterator it = radiusGlbMap::glbRadiusIpv6UserMap.find(std::string(sourceIP).substr(0, IPV6_PREFIX_LAN));
//
//	if(it != radiusGlbMap::glbRadiusIpv6UserMap.end())
//	{
//		 strcpy(userId, it->second.userId);
//		 strcpy(userPolicyPlan, it->second.userPolicyPlan);
//		 strcpy(userPlan, it->second.userPlan);
//		 strcpy(userMac, it->second.userMac);
//		 strcpy(userOlt, it->second.OLT);
//	}
//
//	unLockRadiusMap();
}


void FUtility::buildAaaXdr(aaaSession *pRadiusSession, char *xdr, bool *errorXdr)
{
	char terminationCause[25];
	char sourceIp[IPV6_ADDR_LEN], destIp[IPV6_ADDR_LEN];
	char ipvAddress[IPV6_ADDR_LEN];

	xdr[0] = terminationCause[0] = ipvAddress[0] = 0;

	if(pRadiusSession->ipVer == IPVersion4)
	{
		sourceIp[0] = destIp[0] = 0;
		long2Ip(pRadiusSession->sourceAddr, sourceIp);
		long2Ip(pRadiusSession->destAddr, destIp);
	}
	else
	{ return; }

	if(pRadiusSession->framedIpLong == 0)
		*errorXdr = true;

	if(pRadiusSession->StartTimeEpochMiliSec > pRadiusSession->EndTimeEpochMiliSec)
	{
		uint64_t temp = pRadiusSession->StartTimeEpochMiliSec;
		pRadiusSession->StartTimeEpochMiliSec = pRadiusSession->EndTimeEpochMiliSec;
		pRadiusSession->EndTimeEpochMiliSec = temp;
	}
	long2Ip(pRadiusSession->framedIpLong, ipvAddress);


	if(pRadiusSession->ipv6AddressPrefixFlag)
		strcpy(ipvAddress, pRadiusSession->userIpV6);

	pRadiusSession->appPort 				= pRadiusSession->dPort;

	/*
	 * 18 termination code are been defined in specification, apart form that all the Unknown
	 */

	if(pRadiusSession->accStatusType == 2 && (pRadiusSession->accTerminationCause == 0 || pRadiusSession->accTerminationCause > 18))
		strcpy(terminationCause, "Unknown");
	else
		strcpy(terminationCause, initSection::acctTeminateMap[pRadiusSession->accTerminationCause].c_str());

	if(pRadiusSession->accAuth > 3) pRadiusSession->accAuth = 0;

	checkNewLine(pRadiusSession->userName);

	sprintf(xdr, "%d,%d,%d,%s,"		// 01- Probe Id,       02- XDR Id, 		       03- App Port,      04- Protocol Desc,
				 "%d,%s,"			// 05- Protocol,       06- framed Protocol,
				 "%u,"				// 07- Session Key
				 "%s,%s,"			// 08- Source Mac,     09- Dest Mac,
			 	 "%s,%d,%s,%d,"		// 10- Source Ip,      11- Source Port,        12- Dest Ip,       13- Dest Port
				 "%llu,%llu,"		// 14- Start Time,     15- End Time,
				 "%d,%s,"			// 16- Req Code,       17- Req Code Desc,
				 "%s,%s,%s,%s,"		// 18- User Name,	   19- framed IP,      	   20- NAS IP,		  21- Calling Station Id
				 "%u,%s,%s,"		// 22- Service Type	   23- Service Type Desc   24- NAS Identifier
				 "%s,%s,"			// 25- User Plan, 	   26- User Policy Plan,
				 "%u,%s,"			// 27- Acc Status Type,28- Acc Status Type Desc,
				 "%u,%s,"			// 29- Termination C,  30- Termination C Desc,
				 "%d,%s,"			// 31- Resp Code,      32- Resp Code Desc,
				 "%d,%s,%u,"		// 33- NAS Port Type,  34- NAS Port Type Desc, 35- SessionTimeOut
				 "%u,%s,%s,"		// 36- Acc Auth,       37- Acc Auth Desc,	   38- Reply Msg
				 "%lu,%d,%s,%s,%s,"	// 39- Flush Time      40- Flush Type		   41- OLT    		  42- IPv6      43- User Mac
				 "%u,%u,%u,%u,%u,"	// 44- Input Octets	   45- Output Octets	   46- Session Time   47- InputPackets    48- Output Packets
				 "%u,%u",			// 49- Input Gigawords 50- Output Gigawords

				SMGlobal::PROBE_ID, XDR_ID_AAA, pRadiusSession->appPort, initSection::protocolName[pRadiusSession->dPort].c_str(),
				pRadiusSession->protocol, initSection::framedProtocolMap[pRadiusSession->protocol].c_str(),
				pRadiusSession->aaaKey,
				"NA", "NA",
				sourceIp, pRadiusSession->sPort, destIp, pRadiusSession->dPort,
				pRadiusSession->StartTimeEpochMiliSec, pRadiusSession->EndTimeEpochMiliSec,
				pRadiusSession->reqCode, initSection::radiusCodeMap[pRadiusSession->reqCode].c_str(),
				pRadiusSession->userName, ipvAddress, /*pRadiusSession->nasIP*/ "NA", pRadiusSession->callingStationId,
				pRadiusSession->serviceType, initSection::serviceTypeMap[pRadiusSession->serviceType].c_str(), pRadiusSession->nasIdentifier,
				"NA", "NA",
				pRadiusSession->accStatusType, initSection::acctStatusMap[pRadiusSession->accStatusType].c_str(),
				pRadiusSession->accTerminationCause, terminationCause,
				pRadiusSession->respCode, initSection::radiusCodeMap[pRadiusSession->respCode].c_str(),
				pRadiusSession->nasPortType, initSection::nasPortTypeMap[pRadiusSession->nasPortType].c_str(), 0,
				pRadiusSession->accAuth, initSection::acctAuthenticMap[pRadiusSession->accAuth].c_str(), pRadiusSession->replyMsg,
				pRadiusSession->flushTime, pRadiusSession->flushType, "NA", pRadiusSession->userIpV6, "NA",
				pRadiusSession->inputOctets, pRadiusSession->outputOctets, 0, 0, 0,
				pRadiusSession->inputGigaWords, pRadiusSession->outputGigaWords);

		return;
}

bool FUtility::checkNewLine(char *s)
{
	if(isalpha(*s))
    {
		while(*s && *s != '\n' && *s != '\r') s++;
		*s = 0;

		return true;
    }
	else
	{
		return false;
	}
}

string FUtility::getResolvedIp4(uint32_t dIp)
{
	lockDnsMap();
	std::string URL = findDns(dIp);
	unLockDnsMap();

//	if(!URL.length())
//		return("NULL");

	if(URL.length() <= 5)
			return("NULL");
	else
		return(URL);
}

string FUtility::findDns(uint32_t dIp)
{ return(getURLLookUp(dIp, DNSGlobal::dnsLookUpMap[dIp % 10])); }

string FUtility::getURLLookUp(uint32_t ip, std::map<uint32_t, std::string> &dnsMap)
{
	std::map<uint32_t, std::string>::iterator itSp = dnsMap.find(ip);

	if(itSp != dnsMap.end())
		return(itSp->second);

	return "";
}


void FUtility::formateIPv6(char *buffer)
{
	unsigned char buf[sizeof(struct in6_addr)];
	int domain = AF_INET6, ret;


	ret = inet_pton(domain, buffer, buf);
	if (ret <= 0)
	{
		if (ret == 0) {
			fprintf(stderr, "Not in presentation format");
		}
		else
			perror("inet_pton");
	}

	if (inet_ntop(domain, buf, buffer, INET6_ADDRSTRLEN) == NULL) {
	               perror("inet_ntop");
	}
}
