/*
 * startSMRouters.cpp
 *
 *  Created on: Feb 5, 2019
 *      Author: Debashis
 */

#include "Routers.h"

Routers::Routers(uint16_t interfaceId, uint16_t routerId)
{
	this->_name = "Routers";
	this->setLogLevel(Log::theLog().level());

	this->intfId 			= interfaceId;
	this->routerId 			= routerId;
	this->routersReadyState = false;
	this->smId 				= -1;
}

Routers::~Routers()
{}

bool Routers::isSMRouterInitialized()
{ return this->routersReadyState; }

void Routers::run()
{
	uint16_t lastIdx, curIdx;

	lastIdx = curIdx = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

	routersReadyState = true;

	while(SMGlobal::ROUTER_RUNNING_STATUS[intfId][routerId])
	{
		usleep(SMGlobal::THREAD_SLEEP_TIME);
		curIdx = READ_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

		while(lastIdx != curIdx)
		{
			processPacket(lastIdx);
			lastIdx = READ_NEXT_TIME_INDEX(lastIdx, Global::TIME_INDEX);
		}
	}

	printf("  Router [%02d][%02d] Stopped...\n", intfId, routerId);
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void Routers::processPacket(uint16_t tIdx)
{
	readTcpPacket(agentStore::tcpABusy[intfId][routerId][tIdx], agentStore::tcpACnt[intfId][routerId][tIdx], agentStore::tcpAStore[intfId][routerId][tIdx]);
	readUdpPacket(agentStore::udpABusy[intfId][routerId][tIdx], agentStore::udpACnt[intfId][routerId][tIdx], agentStore::udpAStore[intfId][routerId][tIdx]);
	readAaaPacket(agentStore::aaaABusy[intfId][routerId][tIdx], agentStore::aaaACnt[intfId][routerId][tIdx], agentStore::aaaAStore[intfId][routerId][tIdx]);
	readDnsPacket(agentStore::dnsABusy[intfId][routerId][tIdx], agentStore::dnsACnt[intfId][routerId][tIdx], agentStore::dnsAStore[intfId][routerId][tIdx]);
}


void Routers::readTcpPacket(bool &busyFlag, uint32_t &pktCnt, std::unordered_map<uint32_t, TcpPacket*> &tcpStore)
{
	uint32_t recCnt = pktCnt;

	if(recCnt > 0)
	{
		busyFlag = true;

		for(uint32_t i = 0; i < recCnt; i++)
		{
			findSmForTcpPacket(tcpStore[i]);
			pktCnt --;
		}
		pktCnt 	= 0;
		busyFlag = false;
	}
}

void Routers::readUdpPacket(bool &busyFlag, uint32_t &pktCnt, std::unordered_map<uint32_t, UdpPacket*> &udpStore)
{
	uint32_t recCnt = pktCnt;

	if(recCnt > 0)
	{
		busyFlag = true;

		for(uint32_t i = 0; i < recCnt; i++)
		{
			findSmForUdpPacket(udpStore[i]);
			pktCnt --;
		}
		pktCnt 	= 0;
		busyFlag = false;
	}
}

void Routers::readAaaPacket(bool &busyFlag, uint32_t &pktCnt, std::unordered_map<uint32_t, AAAPacket*> &aaaStore)
{
	uint32_t recCnt = pktCnt;

	if(recCnt > 0)
	{
		busyFlag = true;

		for(uint32_t i = 0; i < recCnt; i++)
		{
			findSmForAaaPacket(aaaStore[i]);
			pktCnt --;
		}
		pktCnt 	= 0;
		busyFlag = false;
	}
}

void Routers::readDnsPacket(bool &busyFlag, uint32_t &pktCnt, std::unordered_map<uint32_t, DnsPacket*> &dnsStore)
{
	uint32_t recCnt = pktCnt;

	if(recCnt > 0)
	{
		busyFlag = true;

		for(uint32_t i = 0; i < recCnt; i++)
		{
			findSmForDnsPacket(dnsStore[i]);
			pktCnt --;
		}
		pktCnt 	= 0;
		busyFlag = false;
	}
}



void Routers::findSmForTcpPacket(TcpPacket* tcpPkt)
{
	smId = -1;

	if(tcpPkt->direction == 0)
		return;

	switch(tcpPkt->ipVer)
	{
		case IPVersion4:
				switch(tcpPkt->direction)
				{
					case UNMAPPED:
						smId = tcpPkt->ipv4FlowId % SMGlobal::UNM_SM_INSTANCES;
						break;

					default:
						smId = tcpPkt->ipv4FlowId % SMGlobal::TCP_SM_INSTANCES;
						break;
				}

				pushTcpPacketToSm(smId, tcpPkt);
				break;

		case IPVersion6:
				switch(tcpPkt->direction)
				{
					case UP:
						smId = tcpPkt->sPort % SMGlobal::TCP_SM_INSTANCES;
						break;

					case DOWN:
						smId = tcpPkt->dPort % SMGlobal::TCP_SM_INSTANCES;
						break;

					case UNMAPPED:
						smId = tcpPkt->sPort % SMGlobal::UNM_SM_INSTANCES;
						break;

					default:
						return;
						break;
				}
				pushTcpPacketToSm(smId, tcpPkt);
				break;

		default:
				break;
	}
}

void Routers::pushTcpPacketToSm(int16_t &smId, TcpPacket* tcpMsg)
{
	uint16_t tIdx = PKT_WRITE_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

	if(smId == -1 || smStore::tcpSmBusy[smId][intfId][routerId][tIdx] == true)
		return;

	if(tcpMsg->direction == UNMAPPED)
		copyTcpObj(smStore::unTcpSmBusy[smId][intfId][routerId][tIdx], smStore::unTcpSmCnt[smId][intfId][routerId][tIdx], smStore::unTcpSmStore[smId][intfId][routerId][tIdx], tcpMsg);
	else
		copyTcpObj(smStore::tcpSmBusy[smId][intfId][routerId][tIdx], smStore::tcpSmCnt[smId][intfId][routerId][tIdx], smStore::tcpSmStore[smId][intfId][routerId][tIdx], tcpMsg);
}

void Routers::copyTcpObj(bool &smBusy, uint32_t &smStoreCnt, std::unordered_map<uint32_t, TcpPacket> &store, TcpPacket* tcpPkt)
{
	if(smBusy)
		return;

	store[smStoreCnt].copy(tcpPkt);
	smStoreCnt++;
}



void Routers::findSmForUdpPacket(UdpPacket* udpPkt)
{
	smId = -1;

	if(udpPkt->direction == 0) return;

	switch(udpPkt->ipVer)
	{
		case IPVersion4:
			switch(udpPkt->direction)
			{
				case UNMAPPED:
					smId = udpPkt->ipv4FlowId % SMGlobal::UNM_SM_INSTANCES;
					break;

				case UP:
				case DOWN:
					smId = udpPkt->ipv4FlowId % SMGlobal::UDP_SM_INSTANCES;
					break;
			}
			pushUdpPacketToSm(smId, udpPkt);
			break;

			break;

		case IPVersion6:
//			switch(udpPkt->pType)
//			{
//				case PACKET_IPPROTO_UDP:
//						switch(udpPkt->direction)
//						{
//							case UNMAPPED:
//								smId = udpPkt->sPort % SMGlobal::UNM_SM_INSTANCES;
//								break;
//
//							case UP:
//								smId = udpPkt->sPort % SMGlobal::UDP_SM_INSTANCES;
//								break;
//
//							case DOWN:
//								smId = udpPkt->dPort % SMGlobal::UDP_SM_INSTANCES;
//								break;
//						}
//						break;
//			}
//			pushUdpPacketToSm(smId, udpPkt);
			break;
	}
}

void Routers::pushUdpPacketToSm(int16_t &smid, UdpPacket* udpPkt)
{
	int tIdx = PKT_WRITE_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

	if(smid == -1 || smStore::udpSmBusy[smid][intfId][routerId][tIdx] == true) return;

	if(udpPkt->direction == UNMAPPED)
		copyUdpObj(smStore::unUdpSmBusy[smid][intfId][routerId][tIdx], smStore::unUdpSmCnt[smid][intfId][routerId][tIdx], smStore::unUdpSmStore[smid][intfId][routerId][tIdx], udpPkt);
	else
		copyUdpObj(smStore::udpSmBusy[smid][intfId][routerId][tIdx], smStore::udpSmCnt[smid][intfId][routerId][tIdx], smStore::udpSmStore[smid][intfId][routerId][tIdx], udpPkt);
}

void Routers::copyUdpObj(bool &smBusy, uint32_t &smStoreCnt, std::unordered_map<uint32_t, UdpPacket> &store, UdpPacket* udpPkt)
{
	if(smBusy) return;

	store[smStoreCnt].copy(udpPkt);
	smStoreCnt++;
}



void Routers::findSmForDnsPacket(DnsPacket* dnsPkt)
{
	smId = -1;

	switch(dnsPkt->ipVer)
	{
		case IPVersion4:
			smId = dnsPkt->ipv4FlowId % SMGlobal::DNS_SM_INSTANCES;
			pushDnsPacketToSm(smId, dnsPkt);
			break;
	}
}

void Routers::pushDnsPacketToSm(int16_t &smid, DnsPacket* dnsPkt)
{
	uint16_t tIdx = PKT_WRITE_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

	if(smid == -1 || smStore::dnsSmBusy[smid][intfId][routerId][tIdx] == true) return;

	copyDnsObj(smStore::dnsSmBusy[smid][intfId][routerId][tIdx], smStore::dnsSmCnt[smid][intfId][routerId][tIdx], smStore::dnsSmStore[smid][intfId][routerId][tIdx], dnsPkt);
}

void Routers::copyDnsObj(bool &smBusy, uint32_t &smStoreCnt, std::unordered_map<uint32_t, DnsPacket> &store, DnsPacket* dnsPkt)
{
	if(smBusy) return;

	store[smStoreCnt].copy(dnsPkt);
	smStoreCnt++;
}



void Routers::findSmForAaaPacket(AAAPacket* aaaMsg)
{
	smId = -1;

	smId = aaaMsg->identifier % SMGlobal::AAA_SM_INSTANCES;
	pushAaaPacketToSm(smId, aaaMsg);
}

void Routers::pushAaaPacketToSm(int16_t &smid, AAAPacket* aaaPkt)
{
	uint16_t tIdx = PKT_WRITE_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

	if(smid == -1 || smStore::aaaSmBusy[smid][intfId][routerId][tIdx] == true) return;

	copyAaaObj(smStore::aaaSmBusy[smid][intfId][routerId][tIdx], smStore::aaaSmCnt[smid][intfId][routerId][tIdx], smStore::aaaSmStore[smid][intfId][routerId][tIdx], aaaPkt);
}

void Routers::copyAaaObj(bool &smBusy, uint32_t &smStoreCnt, std::unordered_map<uint32_t, AAAPacket> &store, AAAPacket* aaaPkt)
{
	if(smBusy) return;

	store[smStoreCnt].copy(aaaPkt);
	smStoreCnt++;
}
