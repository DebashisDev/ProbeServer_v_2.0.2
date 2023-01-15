/*
 * ReadAaaAgent.cpp
 *
 *  Created on: 16 Mar 2021
 *      Author: Debashis
 */

#include "ReadAaaAgent.h"

ReadAaaAgent::ReadAaaAgent(uint16_t interfaceId) : ctx(1), servSocket(ctx, ZMQ_PULL)
{
	this->_name = "ReadAaaAgent";
	this->setLogLevel(Log::theLog().level());

	this->interfaceId			= interfaceId;
	this->aaaAgentReadyState 	= false;

	this->router 				= 0;
	this->t_index				= 0;
	this->previous_t_index 		= 0;
	this->noOfPackets			= 0;
	this->maxPktPerRouter		= SMGlobal::AAA_PPS_PER_INTERFACE[interfaceId]/SMGlobal::ROUTER_PER_INTERFACE[interfaceId];
}

ReadAaaAgent::~ReadAaaAgent()
{ }

bool ReadAaaAgent::isAaaAgentReady()
{ return aaaAgentReadyState; }

void ReadAaaAgent::run()
{
	servSocket.bind(SMGlobal::AAA_SERVER_ADDRESS[interfaceId].c_str());
	printf("     ----- Interface [%02d] Agent Bind to AAA Channel :: %s ", interfaceId, SMGlobal::AAA_SERVER_ADDRESS[interfaceId].c_str());
	TheLog_nc_v2(Log::Info, name(),"     ----- Interface [%02d] Agent Bind to AAA Channel :: %s", interfaceId, SMGlobal::AAA_SERVER_ADDRESS[interfaceId].c_str());
	printf("Connected....\n");

	aaaAgentReadyState = true;

	while(SMGlobal::AAA_AGENT_RUNNING_STATUS[interfaceId])
	{ processPacket(); }

	printf("  ReadAaaAgent [%02d] Stopped...\n", interfaceId);
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void ReadAaaAgent::processPacket()
{
	servSocket.recv(&nwtData);

	t_index = PKT_WRITE_TIME_INDEX(Global::CURRENT_EPOCH_SEC, Global::TIME_INDEX);

	if(t_index >= 10) t_index = 0;

	SMGlobal::AAA_PACKET_PER_DAY[interfaceId]++;

	if(agentStore::aaaABusy[interfaceId][router][t_index] || agentStore::aaaACnt[interfaceId][router][t_index] >= maxPktPerRouter)
		return;

	AAAPacket *pAaaPacket = agentStore::aaaAStore[interfaceId][router][t_index][agentStore::aaaACnt[interfaceId][router][t_index]];

	if(pAaaPacket == NULL || SMGlobal::INSTANCE_ID != 1) return;

	try
	{ memcpy((void *)pAaaPacket, nwtData.data() , nwtData.size()); }
	catch(int x)
	{ printf("Encounted Exception in ReadAaaAgent ..\n"); }

	agentStore::aaaACnt[interfaceId][router][t_index]++ ;

	router++ ;
	if(router >= SMGlobal::ROUTER_PER_INTERFACE[interfaceId])
		router = 0;
}
