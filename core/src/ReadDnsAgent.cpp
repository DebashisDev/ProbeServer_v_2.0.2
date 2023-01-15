/*
 * ReadDnsAgent.cpp
 *
 *  Created on: 16 Mar 2021
 *      Author: Debashis
 */

#include "ReadDnsAgent.h"

ReadDnsAgent::ReadDnsAgent(uint16_t interfaceId) : ctx(1), servSocket(ctx, ZMQ_PULL)
{
	this->_name = "ReadDnsAgent";
	this->setLogLevel(Log::theLog().level());

	this->interfaceId			= interfaceId;
	this->dnsAgentReadyState 	= false;

	this->router 				= 0;
	this->t_index				= 0;
	this->previous_t_index 		= 0;
	this->noOfPackets			= 0;
	this->maxPktPerRouter		= SMGlobal::DNS_PPS_PER_INTERFACE[this->interfaceId]/SMGlobal::ROUTER_PER_INTERFACE[this->interfaceId];
}

ReadDnsAgent::~ReadDnsAgent()
{ }

bool ReadDnsAgent::isDnsAgentReady()
{ return dnsAgentReadyState; }

void ReadDnsAgent::run()
{
	servSocket.bind(SMGlobal::DNS_SERVER_ADDRESS[interfaceId].c_str());
	printf("     ----- Interface [%02d] Agent Bind to DNS Channel :: %s ", interfaceId, SMGlobal::DNS_SERVER_ADDRESS[interfaceId].c_str());
	TheLog_nc_v2(Log::Info, name(),"     ----- Interface [%02d] Agent Bind to DNS Channel :: %s", interfaceId, SMGlobal::DNS_SERVER_ADDRESS[interfaceId].c_str());
	printf("Connected....\n");

	dnsAgentReadyState = true;

	while(SMGlobal::DNS_AGENT_RUNNING_STATUS[interfaceId])
	{ processPacket(); }

	printf("  ReadDnsAgent [%02d] Stopped...\n", interfaceId);
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void ReadDnsAgent::processPacket()
{
	servSocket.recv(&nwtData);

	t_index = PKT_WRITE_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

	if(t_index >= 10) t_index = 0;

	SMGlobal::DNS_PACKET_PER_DAY[interfaceId]++;

	if(agentStore::dnsABusy[interfaceId][router][t_index] || agentStore::dnsACnt[interfaceId][router][t_index] >= maxPktPerRouter)
		return;

	DnsPacket *pDnsPacket = agentStore::dnsAStore[interfaceId][router][t_index][agentStore::dnsACnt[interfaceId][router][t_index]];

	if(pDnsPacket == NULL)
		return;

	try
	{ memcpy((void *)pDnsPacket, nwtData.data() , nwtData.size()); }
	catch(int x)
	{ printf("Encounted Exception ReadDnsAgent ..\n"); }

	agentStore::dnsACnt[interfaceId][router][t_index]++ ;

	router++ ;
	if(router >= SMGlobal::ROUTER_PER_INTERFACE[interfaceId])
		router = 0;
}
