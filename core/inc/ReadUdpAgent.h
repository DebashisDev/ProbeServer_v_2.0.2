/*
 * ReadUdpAgent.h
 *
 *  Created on: 16 Mar 2021
 *      Author: Debashis
 */

#ifndef CORE_SRC_READUDPAGENT_H_
#define CORE_SRC_READUDPAGENT_H_

#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "map"

#include "BaseConfig.h"
#include "Log.h"
#include "MapStores.h"
#include "ServerGlobal.h"
#include "SmGlobal.h"
#include "zhelpers.hpp"
#include "zmq.hpp"

class ReadUdpAgent : BaseConfig
{
	private:
		uint16_t	interfaceId;
		uint16_t 	router;
		uint16_t	t_index;
		uint16_t 	previous_t_index;
		uint32_t	maxPktPerRouter;
		uint32_t 	noOfPackets;
		bool 		udpAgentReadyState;

		zmq::context_t 	ctx;
		zmq::socket_t 	servSocket;
		zmq::message_t 	nwtData;

		void 	processPacket();

	public:
		ReadUdpAgent(uint16_t interfaceId);
		~ReadUdpAgent();

		void run();
		bool isUdpAgentReady();
};

#endif /* CORE_SRC_READUDPAGENT_H_ */
