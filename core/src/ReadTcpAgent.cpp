/*
 * ReadInterface.cpp
 *
 *  Created on: March 20, 2020
 *      Author: Debashis
 */

#include "ReadTcpAgent.h"


ReadTcpAgent::ReadTcpAgent(uint16_t interfaceId) : ctx(1), servSocket(ctx, ZMQ_PULL)
{
	this->_name = "ReadTcpAgent";
	this->setLogLevel(Log::theLog().level());

	this->interfaceId			= interfaceId;
	this->tcpAgentReadyState 	= false;

	this->router 				= 0;
	this->t_index				= 0;
	this->previous_t_index 		= 0;
	this->noOfPackets			= 0;
	this->maxPktPerRouter		= SMGlobal::TCP_PPS_PER_INTERFACE[this->interfaceId]/SMGlobal::ROUTER_PER_INTERFACE[this->interfaceId];
}

ReadTcpAgent::~ReadTcpAgent()
{ }

bool ReadTcpAgent::isTcpAgentReady()
{ return tcpAgentReadyState; }

void ReadTcpAgent::run()
{
	servSocket.bind(SMGlobal::TCP_SERVER_ADDRESS[interfaceId].c_str());
	printf("     ----- Interface [%02d] Agent Bind to TCP Channel :: %s ", interfaceId, SMGlobal::TCP_SERVER_ADDRESS[interfaceId].c_str());
	TheLog_nc_v2(Log::Info, name(),"     ----- Interface [%02d] Agent Bind to TCP Channel :: %s", interfaceId, SMGlobal::TCP_SERVER_ADDRESS[interfaceId].c_str());
	printf("Connected....\n");

	tcpAgentReadyState = true;

	while(SMGlobal::TCP_AGENT_RUNNING_STATUS[interfaceId])
	{ processPacket(); }

	printf("  ReadTcpAgent [%02d] Stopped...\n", interfaceId);
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void ReadTcpAgent::processPacket()
{
	servSocket.recv(&nwtData);

	t_index = PKT_WRITE_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

	if(t_index >= 10) t_index = 0;

	SMGlobal::TCP_PACKET_PER_DAY[interfaceId]++;

	if(agentStore::tcpABusy[interfaceId][router][t_index] || agentStore::tcpACnt[interfaceId][router][t_index] >= maxPktPerRouter)
		return;

	TcpPacket *pTcpPacket = agentStore::tcpAStore[interfaceId][router][t_index][agentStore::tcpACnt[interfaceId][router][t_index]];

	if(pTcpPacket == NULL)
			return;

	try
	{ memcpy((void *)pTcpPacket, nwtData.data() , nwtData.size()); }
	catch(int x)
	{ printf("Encounted Exception ReadTcpAgent ..\n"); }

	agentStore::tcpACnt[interfaceId][router][t_index]++ ;

	router++ ;
	if(router >= SMGlobal::ROUTER_PER_INTERFACE[interfaceId])
		router = 0;
}
