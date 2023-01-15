/*
 * ReadUdpAgent.cpp
 *
 *  Created on: 16 Mar 2021
 *      Author: Debashis
 */

#include "ReadUdpAgent.h"

ReadUdpAgent::ReadUdpAgent(uint16_t interfaceId) : ctx(1), servSocket(ctx, ZMQ_PULL)
{
	this->_name = "ReadUdpAgent";
	this->setLogLevel(Log::theLog().level());

	this->interfaceId			= interfaceId;
	this->udpAgentReadyState 	= false;

	this->router 				= 0;
	this->t_index				= 0;
	this->previous_t_index 		= 0;
	this->noOfPackets			= 0;
	this->maxPktPerRouter		= SMGlobal::UDP_PPS_PER_INTERFACE[this->interfaceId]/SMGlobal::ROUTER_PER_INTERFACE[this->interfaceId];
}

ReadUdpAgent::~ReadUdpAgent()
{ }

bool ReadUdpAgent::isUdpAgentReady()
{ return udpAgentReadyState; }

void ReadUdpAgent::run()
{
	servSocket.bind(SMGlobal::UDP_SERVER_ADDRESS[interfaceId].c_str());
	printf("     ----- Interface [%02d] Agent Bind to UDP Channel :: %s ", interfaceId, SMGlobal::UDP_SERVER_ADDRESS[interfaceId].c_str());
	TheLog_nc_v2(Log::Info, name(),"     ----- Interface [%02d] Agent Bind to UDP Channel :: %s", interfaceId, SMGlobal::UDP_SERVER_ADDRESS[interfaceId].c_str());
	printf("Connected....\n");

	udpAgentReadyState = true;

	while(SMGlobal::UDP_AGENT_RUNNING_STATUS[interfaceId])
	{ processPacket(); }

	printf("  ReadUdpAgent [%02d] Stopped...\n", interfaceId);
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void ReadUdpAgent::processPacket()
{
	servSocket.recv(&nwtData);

	t_index = PKT_WRITE_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

	if(t_index >= 10) t_index = 0;

	SMGlobal::UDP_PACKET_PER_DAY[interfaceId]++;

	if(agentStore::udpABusy[interfaceId][router][t_index] || agentStore::udpACnt[interfaceId][router][t_index] >= maxPktPerRouter)
		return;

	UdpPacket *pUdpPacket = agentStore::udpAStore[interfaceId][router][t_index][agentStore::udpACnt[interfaceId][router][t_index]];

	if(pUdpPacket == NULL)
		return;

	try
	{ memcpy((void *)pUdpPacket, nwtData.data() , nwtData.size()); }
	catch(int x)
	{ printf("Encounted Exception ReadUdpAgent ..\n"); }

	agentStore::udpACnt[interfaceId][router][t_index]++ ;

	router++ ;
	if(router >= SMGlobal::ROUTER_PER_INTERFACE[interfaceId])
		router = 0;
}
