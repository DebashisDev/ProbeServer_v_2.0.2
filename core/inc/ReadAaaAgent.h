/*
 * ReadAaaAgent.h
 *
 *  Created on: 16 Mar 2021
 *      Author: Debashis
 */

#ifndef CORE_SRC_READAAAAGENT_H_
#define CORE_SRC_READAAAAGENT_H_

#include <sys/time.h>

#include "unistd.h"
#include "map"

#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "BaseConfig.h"
#include "Log.h"
#include "MapStores.h"
#include "ServerGlobal.h"
#include "SmGlobal.h"
#include "zhelpers.hpp"
#include "zmq.hpp"

class ReadAaaAgent : BaseConfig
{
	private:
		uint16_t	interfaceId;
		uint16_t 	router;
		uint16_t	t_index;
		uint16_t 	previous_t_index;
		uint32_t	maxPktPerRouter;
		uint32_t 	noOfPackets;
		bool 		aaaAgentReadyState;

		zmq::context_t ctx;
		zmq::socket_t servSocket;
		zmq::message_t nwtData;

		void 		processPacket();

	public:
		ReadAaaAgent(uint16_t interfaceId);
		~ReadAaaAgent();

		void run();
		bool isAaaAgentReady();
};

#endif /* CORE_SRC_READAAAAGENT_H_ */
