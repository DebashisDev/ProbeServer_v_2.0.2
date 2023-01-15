/*
 * readInputQueue.h
 *
 *  Created on: Feb 5, 2019
 *      Author: Debashis
 */

#ifndef CORE_SRC_READTCPAGENT_H_
#define CORE_SRC_READTCPAGENT_H_

#include <sys/time.h>

#include "unistd.h"
#include "map"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "BaseConfig.h"
#include "Log.h"
#include "MapStores.h"
#include "ServerGlobal.h"
#include "SmGlobal.h"
#include "zhelpers.hpp"
#include "zmq.hpp"

class ReadTcpAgent : BaseConfig
{
	public:
		ReadTcpAgent(uint16_t interfaceId);
		~ReadTcpAgent();
		void run();
		bool isTcpAgentReady();

	private:
		uint16_t		interfaceId;
		uint16_t 		router;
		uint16_t		t_index;
		uint16_t 		previous_t_index;
		uint32_t		maxPktPerRouter;
		uint32_t 		noOfPackets;
		bool 			tcpAgentReadyState;

		zmq::context_t 	ctx;
	    zmq::socket_t 	servSocket;
	    zmq::message_t 	nwtData;

	    void 			processPacket();
};

#endif /* CORE_SRC_READTCPAGENT_H_ */
