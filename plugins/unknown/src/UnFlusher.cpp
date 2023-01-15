/*
 * UnFlusher.cpp
 *
 *  Created on: 16-Aug-2021
 *      Author: Debashis
 */

#include "UnFlusher.h"

UnFlusher::UnFlusher()
{
	_thisLogLevel = 0;
	this->_name = "UnFlusher";
	this->setLogLevel(Log::theLog().level());

	this->repoInitStatus = false;
	this->curIndex		 = 0;
	this->lastIndex		 = 0;
	this->curMin		 = 0;
	this->prevMin		 = 0;
	this->totalCnt		 = 0;
	this->pFlUtility	 = new FUtility();
}

UnFlusher::~UnFlusher() {
	delete(this->pFlUtility);
}

bool UnFlusher::isUnFlusherReady()
{ return repoInitStatus; }

void UnFlusher::run()
{
	repoInitStatus = true;
	lastIndex = curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

	curMin = prevMin = Global::CURRENT_MIN;
	openXdrFile(PACKET_IPPROTO_TCP, Global::CURRENT_MIN, Global::CURRENT_HOUR, Global::CURRENT_DAY, Global::CURRENT_MONTH, Global::CURRENT_YEAR);
	openXdrFile(PACKET_IPPROTO_UDP, Global::CURRENT_MIN, Global::CURRENT_HOUR, Global::CURRENT_DAY, Global::CURRENT_MONTH, Global::CURRENT_YEAR);
	openXdrFile(PACKET_IPPROTO_DNS, Global::CURRENT_MIN, Global::CURRENT_HOUR, Global::CURRENT_DAY, Global::CURRENT_MONTH, Global::CURRENT_YEAR);

	while(SMGlobal::UNM_FLUSHER_RUNNING_STATUS)
	{
		usleep(SMGlobal::THREAD_SLEEP_TIME);
		curIndex = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

		curMin = Global::CURRENT_MIN;

		if(curMin != prevMin)
		{
			closeXdrFile(PACKET_IPPROTO_TCP);
			closeXdrFile(PACKET_IPPROTO_UDP);
			closeXdrFile(PACKET_IPPROTO_DNS);
			prevMin = curMin;
			openXdrFile(PACKET_IPPROTO_TCP, Global::CURRENT_MIN, Global::CURRENT_HOUR, Global::CURRENT_DAY, Global::CURRENT_MONTH, Global::CURRENT_YEAR);
			openXdrFile(PACKET_IPPROTO_UDP, Global::CURRENT_MIN, Global::CURRENT_HOUR, Global::CURRENT_DAY, Global::CURRENT_MONTH, Global::CURRENT_YEAR);
			openXdrFile(PACKET_IPPROTO_DNS, Global::CURRENT_MIN, Global::CURRENT_HOUR, Global::CURRENT_DAY, Global::CURRENT_MONTH, Global::CURRENT_YEAR);
		}

		while(lastIndex != curIndex)
		{
			processTcpData(lastIndex);
			processUdpData(lastIndex);
			processDnsData(lastIndex);

			lastIndex = READ_NEXT_TIME_INDEX(lastIndex, Global::TIME_INDEX);
		}
	}
	printf("  UnMapped Flusher Stopped... \n");
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void UnFlusher::processTcpData(uint16_t idx)
{
	for(uint16_t sm = 0; sm < SMGlobal::UNM_SM_INSTANCES; sm++)
		flushTcpData(UnFlushRepo::uTcpFlCnt[sm][idx], UnFlushRepo::uTcpFlStore[sm][idx]);
}

void UnFlusher::processUdpData(uint16_t idx)
{
	for(uint16_t sm = 0; sm < SMGlobal::UNM_SM_INSTANCES; sm++)
		flushUdpData(UnFlushRepo::uUdpFlCnt[sm][idx], UnFlushRepo::uUdpFlStore[sm][idx]);
}

void UnFlusher::processDnsData(uint16_t idx)
{
	for(uint16_t sm = 0; sm < SMGlobal::UNM_SM_INSTANCES; sm++)
		flushDnsData(UnFlushRepo::uDnsFlCnt[sm][idx], UnFlushRepo::uDnsFlStore[sm][idx]);
}

void UnFlusher::flushTcpData(uint32_t &flCnt, std::unordered_map<uint32_t, tcpSession> &pkt)
{
	totalCnt = flCnt;

	if(totalCnt > 0)
	{
		for (auto it = pkt.cbegin(), next_it = it; it != pkt.cend(); it = next_it)
		{
			if(createTcpXdrData(it->second))
				tcpXdrHandler << std::string(tcpXdr) << endl;

			++next_it;
			pkt.erase(it);
			flCnt --;
		}
		flCnt = 0;
	}
}

void UnFlusher::flushUdpData(uint32_t &flCnt, std::unordered_map<uint32_t, udpSession> &pkt)
{
	totalCnt = flCnt;

	if(totalCnt > 0)
	{
		for (auto it = pkt.cbegin(), next_it = it; it != pkt.cend(); it = next_it)
		{
			if(createUdpXdrData(it->second))
				udpXdrHandler << std::string(udpXdr) << endl;

			++next_it;
			pkt.erase(it);
			flCnt --;
		}
		flCnt = 0;
	}
}

void UnFlusher::flushDnsData(uint32_t &flCnt, std::unordered_map<uint32_t, dnsSession> &pkt)
{
	totalCnt = flCnt;

	if(totalCnt > 0)
	{
		for (auto it = pkt.cbegin(), next_it = it; it != pkt.cend(); it = next_it)
		{
			if(createDnsXdrData(it->second))
				dnsXdrHandler << std::string(dnsXdr) << endl;

			++next_it;
			pkt.erase(it);
			flCnt --;
		}
		flCnt = 0;
	}
}

bool UnFlusher::createTcpXdrData(tcpSession pTcpSession)
{
	if(&pTcpSession == NULL)
		return false;

	tcpXdr[0] = 0;
	buildTcpXdr(&pTcpSession);

	if(strlen(tcpXdr) <= 0)
		return false;
	else
		return true;
}

bool UnFlusher::createUdpXdrData(udpSession pUdpSession)
{
	if(&pUdpSession == NULL)
		return false;

	udpXdr[0] = 0;
	buildUdpXdr(&pUdpSession);

	if(strlen(udpXdr) <= 0)
		return false;
	else
		return true;
}

bool UnFlusher::createDnsXdrData(dnsSession pDnsSession)
{
	if(&pDnsSession == NULL)
		return false;

	dnsXdr[0] = 0;
	pFlUtility->buildDnsXdr(&pDnsSession, dnsXdr);

	if(strlen(dnsXdr) <= 0)
		return false;
	else
		return true;
}

void UnFlusher::buildTcpXdr(tcpSession *pTcpSession)
{
	string url = "NULL";
	string sessionKey = "";
	uint32_t	temp = 0;

	sIp[0] = 0;
	dIp[0] = 0;

	if(pTcpSession->frCount <= 15 || pTcpSession->pLoadPkt == 0)
		return;

	if(checkPeeringDestIp(pTcpSession->sIpv4))
	{
		temp = pTcpSession->sIpv4;

		pTcpSession->sIpv4 = pTcpSession->dIpv4;
		pTcpSession->dIpv4 = temp;
	}

	if(check_UnknownSourceIP(pTcpSession->dIpv4))
	{
		temp = pTcpSession->sIpv4;

		pTcpSession->sIpv4 = pTcpSession->dIpv4;
		pTcpSession->dIpv4 = temp;
	}

	if(check_CacheIpPool(pTcpSession->sIpv4))
	{
		temp = pTcpSession->sIpv4;

		pTcpSession->sIpv4 = pTcpSession->dIpv4;
		pTcpSession->dIpv4 = temp;
	}


	/* get URL */
	switch(pTcpSession->ipVer)
	{
		case IPVersion4:
						sessionKey = to_string(pTcpSession->sessionIpV4Key);

//						url = pFlUtility->getResolvedIp4(pTcpSession->dIpv4);

						long2Ip(pTcpSession->sIpv4, sIp);
						long2Ip(pTcpSession->dIpv4, dIp);
						break;

		default:
			break;
	}

	sprintf(tcpXdr, "%d,%d,%d,%s,"	// 01 - Probe Id			02 - XDR Id, 		     03 - Protocol Type		04 - Protocol Desc,
					"%d,%s-%d,"		// 05 - Dest Port	    	06 - Session Key-Slice Counter
					"%s,%d,%s,%d,"	// 07 - Source Ip			08 - Source Port		 09 - Dest Ip			10 - Dest Port,
					"%d,%d,%u,"		// 11 - VLAN Id				12 - Frame Cnt			 13 - Frame Size
					"%d,%u,"		// 14 - Payload Pkt   		15 - Payload Size
					"%lu,%lu,"		// 16 - Start Time			17 - End Time
					"%d,%s,%d",		// 18 - Cause Code			19 - URL				 20 - Ip version

				SMGlobal::PROBE_ID, XDR_ID_IP, pTcpSession->pType, initSection::protocolName[pTcpSession->pType].c_str(),
				pTcpSession->dPort, sessionKey.c_str(), pTcpSession->sliceCounter,
				sIp, pTcpSession->sPort, dIp, pTcpSession->dPort,
				pTcpSession->vLanId, pTcpSession->frCount, pTcpSession->frSize,
				pTcpSession->pLoadPkt, pTcpSession->pLoadSize,
				pTcpSession->startTimeEpochNanoSec, pTcpSession->endTimeEpochNanoSec,
				pTcpSession->causeCode, url.c_str(), pTcpSession->ipVer);
}

void UnFlusher::buildUdpXdr(udpSession *pUdpSession)
{
	string url = "NULL";
	string sessionKey = "";
	uint32_t temp = 0;

	sIp[0] = 0;
	dIp[0] = 0;

	if(pUdpSession->frCount <= 15 || pUdpSession->pLoadPkt == 0)
		return;

	if(checkPeeringDestIp(pUdpSession->sIpv4))
	{
		temp = pUdpSession->sIpv4;

		pUdpSession->sIpv4 = pUdpSession->dIpv4;
		pUdpSession->dIpv4 = temp;
	}

    if(check_UnknownSourceIP(pUdpSession->dIpv4))
     {
             temp = pUdpSession->sIpv4;

             pUdpSession->sIpv4 = pUdpSession->dIpv4;
             pUdpSession->dIpv4 = temp;
     }

	if(check_CacheIpPool(pUdpSession->sIpv4))
	{
		temp = pUdpSession->sIpv4;

		pUdpSession->sIpv4 = pUdpSession->dIpv4;
		pUdpSession->dIpv4 = temp;
	}

	/* get URL */
	switch(pUdpSession->ipVer)
	{
		case IPVersion4:
				sessionKey = to_string(pUdpSession->sessionIpV4Key);

//				url = pFlUtility->getResolvedIp4(pUdpSession->dIpv4);

				long2Ip(pUdpSession->sIpv4, sIp);
				long2Ip(pUdpSession->dIpv4, dIp);
				break;

		default:
			break;
	}

	sprintf(udpXdr, "%d,%d,%d,%s,"	// 01 - Probe Id			02 - XDR Id, 		     03 - Protocol Type		04 - Protocol Desc,
					"%d,%s-%d,"		// 05 - Dest Port	    	06 - Session Key-Slice Counter
					"%s,%d,%s,%d,"	// 08 - Source Ip			09 - Source Port		 10 - Dest Ip			11 - Dest Port,
					"%d,%d,%u,"		// 12 - VLAN Id				13 - Frame Cnt			 14 - Frame Size
					"%d,%u,"		// 15 - Payload Pkt   		16 - Payload Size
					"%lu,%lu,"		// 17 - Start Time			18 - End Time
					"%d,%s,%d",		// 19 - Cause Code			20 - URL				 21 - Ip version

				SMGlobal::PROBE_ID, XDR_ID_IP, pUdpSession->pType, initSection::protocolName[pUdpSession->pType].c_str(),
				pUdpSession->dPort, sessionKey.c_str(), pUdpSession->sliceCounter,
				sIp, pUdpSession->sPort, dIp, pUdpSession->dPort,
				0, pUdpSession->frCount, pUdpSession->frSize,
				pUdpSession->pLoadPkt, pUdpSession->pLoadSize,
				pUdpSession->startTimeEpochNanoSec, pUdpSession->endTimeEpochNanoSec,
				pUdpSession->causeCode, url.c_str(), pUdpSession->ipVer);
}

bool UnFlusher::IsIPInRange(uint32_t ip, uint32_t network, uint32_t mask)
{
    uint32_t net_lower = (network & mask);
    uint32_t net_upper = (net_lower | (~mask));

    if(ip >= net_lower && ip <= net_upper)
        return true;
    return false;
}

bool UnFlusher::checkPeeringDestIp(uint32_t ip)
{
	if(check_I3DNET(ip))
		return true;
	else if(check_VERISIGN(ip))
		return true;
	else if(check_SUBSPACE(ip))
		return true;
	else if(check_EDGECAST(ip))
		return true;
	else if(check_FASTLY(ip))
		return true;
	else if(check_PKT_CLEARING_HOUSE(ip))
		return true;
	else if(check_HUAWEI_CLOUD(ip))
		return true;
	else if(check_PEERING_TENCENT(ip))
		return true;
	else if(check_ALIBABA(ip))
		return true;
	else if(check_QUANTIL(ip))
		return true;
	else if(check_MICROSOFT(ip))
		return true;
	else if(check_LIMELIGHT(ip))
		return true;
	else if(check_ZENLAYER(ip))
		return true;
	else if(check_CLOUDFLARE(ip))
		return true;
	else if(check_NETFLIX(ip))
		return true;
	else if(check_AKAMAI(ip))
		return true;
	else if(check_GOOGLE(ip))
		return true;
	else if(check_FACEBOOK(ip))
		return true;
	else if(check_AMAZON(ip))
		return true;
	else if(check_DECIX(ip))
		return true;
	else
		return false;
}

bool UnFlusher::check_I3DNET(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_I3DNET_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_I3DNET[counter][0], SMGlobal::PEERING_I3DNET[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_VERISIGN(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_VERISIGN_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_VERISIGN[counter][0], SMGlobal::PEERING_VERISIGN[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_SUBSPACE(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_SUBSPACE_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_SUBSPACE[counter][0], SMGlobal::PEERING_SUBSPACE[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_EDGECAST(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_EDGECAST_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_EDGECAST[counter][0], SMGlobal::PEERING_EDGECAST[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_FASTLY(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_FASTLY_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_FASTLY[counter][0], SMGlobal::PEERING_FASTLY[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_PKT_CLEARING_HOUSE(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_PKT_CLEARING_HOUSE_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_PKT_CLEARING_HOUSE[counter][0], SMGlobal::PEERING_PKT_CLEARING_HOUSE[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_HUAWEI_CLOUD(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_HUAWEI_CLOUD_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_HUAWEI_CLOUD[counter][0], SMGlobal::PEERING_HUAWEI_CLOUD[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_PEERING_TENCENT(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_TENCENT_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_TENCENT[counter][0], SMGlobal::PEERING_TENCENT[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_ALIBABA(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_ALIBABA_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_ALIBABA[counter][0], SMGlobal::PEERING_ALIBABA[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_QUANTIL(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_QUANTIL_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_QUANTIL[counter][0], SMGlobal::PEERING_QUANTIL[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_MICROSOFT(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_MICROSOFT_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_MICROSOFT[counter][0], SMGlobal::PEERING_MICROSOFT[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_LIMELIGHT(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_LIMELIGHT_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_LIMELIGHT[counter][0], SMGlobal::PEERING_LIMELIGHT[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_ZENLAYER(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_ZENLAYER_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_ZENLAYER[counter][0], SMGlobal::PEERING_ZENLAYER[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_CLOUDFLARE(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_CLOUDFLARE_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_CLOUDFLARE[counter][0], SMGlobal::PEERING_CLOUDFLARE[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_NETFLIX(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_NETFLIX_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_NETFLIX[counter][0], SMGlobal::PEERING_NETFLIX[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_AKAMAI(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_AKAMAI_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_AKAMAI[counter][0], SMGlobal::PEERING_AKAMAI[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_GOOGLE(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_GOOGLE_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_GOOGLE[counter][0], SMGlobal::PEERING_GOOGLE[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_FACEBOOK(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_FACEBOOK_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_FACEBOOK[counter][0], SMGlobal::PEERING_FACEBOOK[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_AMAZON(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_AMAZON_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_AMAZON[counter][0], SMGlobal::PEERING_AMAZON[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_DECIX(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::PEERING_DECIX_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::PEERING_DECIX[counter][0], SMGlobal::PEERING_DECIX[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_CacheIpPool(uint32_t ip)
{
	bool ret = false;

	for(uint16_t counter = 0; counter < SMGlobal::CACHE_IP_POOL_COUNT; counter++)
	{
		if(IsIPInRange(ip, SMGlobal::CACHE_IP_POOL[counter][0], SMGlobal::CACHE_IP_POOL[counter][1]))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool UnFlusher::check_UnknownSourceIP(uint32_t ip)
{
        bool ret = false;

        for(uint16_t counter = 0; counter < SMGlobal::UNKNOWN_SOURCE_POOL_COUNT; counter++)
        {
                if(IsIPInRange(ip, SMGlobal::UNKNOWN_SOURCE_POOL[counter][0], SMGlobal::UNKNOWN_SOURCE_POOL[counter][1]))
                {
                        ret = true;
                        break;
                }
        }
        return ret;
}

void UnFlusher::openXdrFile(uint16_t protocol, uint16_t min, uint16_t hour, uint16_t day, uint16_t month, uint16_t year)
{
	char filePath[300];
	filePath[0] = 0;

	switch(protocol)
	{
		case PACKET_IPPROTO_TCP:
				sprintf(filePath, "%s%s/%s_%d-%02d-%02d-%02d-%02d_%d.csv",
								SMGlobal::UNM_DIR.c_str(),
								"ip",
								SMGlobal::TCP_FILE_PREFIX.c_str(),
								year,
								month,
								day,
								hour,
								min,
								0);
				tcpXdrHandler.open((char *)filePath, ios :: out | ios :: app);
				break;

		case PACKET_IPPROTO_UDP:
				sprintf(filePath, "%s%s/%s_%d-%02d-%02d-%02d-%02d_%d.csv",
								SMGlobal::UNM_DIR.c_str(),
								"ip",
								SMGlobal::UDP_FILE_PREFIX.c_str(),
								year,
								month,
								day,
								hour,
								min,
								0);
				udpXdrHandler.open((char *)filePath, ios :: out | ios :: app);
				break;

		case PACKET_IPPROTO_DNS:
				sprintf(filePath, "%s%s/%s_%d-%02d-%02d-%02d-%02d_%d.csv",
								SMGlobal::UNM_DIR.c_str(),
								SMGlobal::DNS_FILE_PREFIX.c_str(),
								SMGlobal::DNS_FILE_PREFIX.c_str(),
								year,
								month,
								day,
								hour,
								min,
								0);
				dnsXdrHandler.open((char *)filePath, ios :: out | ios :: app);
				break;
	}
}

void UnFlusher::closeXdrFile(uint16_t protocol)
{
	switch(protocol)
	{
		case PACKET_IPPROTO_TCP:
				tcpXdrHandler.close();
				break;

		case PACKET_IPPROTO_UDP:
				udpXdrHandler.close();
				break;

		case PACKET_IPPROTO_DNS:
				dnsXdrHandler.close();
				break;
	}
}

//void UnFlusher::flushTcpData(uint32_t &flCnt, std::unordered_map<uint32_t, tcpSession> &pkt)
//{
//	totalCnt = flCnt;
//
//	if(totalCnt > 0)
//	{
//		for(uint32_t cnt = 0; cnt < totalCnt; cnt++)
//		{
//			if(createTcpXdrData(&pkt[cnt]))
//			{
//				tcpXdrHandler << std::string(tcpXdr) << endl;
//			}
//			pkt.erase(cnt);
//			flCnt--;
//		}
//		pkt.clear();
//	}
//	flCnt = 0;
//}
//
//void UnFlusher::flushUdpData(uint32_t &flCnt, std::unordered_map<uint32_t, udpSession> &pkt)
//{
//	totalCnt = flCnt;
////	batchCount = 0;
//
//	if(totalCnt > 0)
//	{
//		for(uint32_t cnt = 0; cnt < totalCnt; cnt++)
//		{
//			if(createUdpXdrData(&pkt[cnt]))
//			{
////				records = std::string(udpXdr) + "\n";
//				udpXdrHandler << std::string(udpXdr) << endl;
////				records = "";
//			}
//			pkt.erase(cnt);
//			flCnt--;
//		}
//		pkt.clear();
//	}
//	flCnt = 0;
//}
//
//void UnFlusher::flushDnsData(uint32_t &flCnt, std::unordered_map<uint32_t, dnsSession> &pkt)
//{
//	totalCnt = flCnt;
//
//	if(totalCnt > 0)
//	{
//		for(uint32_t cnt = 0; cnt < totalCnt; cnt++)
//		{
//			if(createDnsXdrData(&pkt[cnt]))
//			{
//				dnsXdrHandler << std::string(dnsXdr) << endl;
//			}
//			pkt.erase(cnt);
//			flCnt--;
//		}
//		pkt.clear();
//	}
//	flCnt = 0;
//}

//void UnFlusher::flushTcpData(uint32_t &flCnt, std::unordered_map<uint32_t, tcpSession> &pkt)
//{
//	tcpSession fLocal;
//	totalCnt = flCnt;
//
//	if(totalCnt > 0)
//	{
//		std::list<std::unordered_map<uint32_t, tcpSession>::const_iterator> iterators;
//
//		for (auto it = pkt.cbegin(); it != pkt.cend(); it++)
//		{
//			fLocal.reset();
//			fLocal = it->second;
//
//			if(createTcpXdrData(&fLocal))
//			{
//				tcpXdrHandler << std::string(tcpXdr) << endl;
//				iterators.push_back(it);
//			}
//		}
//		flCnt = 0;
//
//		for (auto it: iterators)
//			pkt.erase(it);
//	}
//}
//
//void UnFlusher::flushUdpData(uint32_t &flCnt, std::unordered_map<uint32_t, udpSession> &pkt)
//{
//	udpSession fLocal;
//	totalCnt = flCnt;
//
//	if(totalCnt > 0)
//	{
//		std::list<std::unordered_map<uint32_t, udpSession>::const_iterator> iterators;
//
//		for (auto it = pkt.cbegin(); it != pkt.cend(); it++)
//		{
//			fLocal.reset();
//			fLocal = it->second;
//
//			if(createUdpXdrData(&fLocal))
//			{
//				udpXdrHandler << std::string(udpXdr) << endl;
//				iterators.push_back(it);
//			}
//		}
//		flCnt = 0;
//
//		for (auto it: iterators)
//			pkt.erase(it);
//	}
//}
//
//void UnFlusher::flushDnsData(uint32_t &flCnt, std::unordered_map<uint32_t, dnsSession> &pkt)
//{
//	dnsSession fLocal;
//	totalCnt = flCnt;
//
//	if(totalCnt > 0)
//	{
//		std::list<std::unordered_map<uint32_t, dnsSession>::const_iterator> iterators;
//
//		for (auto it = pkt.cbegin(); it != pkt.cend(); it++)
//		{
//			fLocal.reset();
//			fLocal = it->second;
//
//			if(createDnsXdrData(&fLocal))
//			{
//				dnsXdrHandler << std::string(dnsXdr) << endl;
//				iterators.push_back(it);
//			}
//		}
//		flCnt = 0;
//
//		for (auto it: iterators)
//			pkt.erase(it);
//	}
//}
