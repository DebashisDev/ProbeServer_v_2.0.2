/*
 * IPSessionManager.h
 *
 *  Created on: Apr 24, 2017
 *      Author: Debashis
 */

#ifndef PLUGINS_TCP_SRC_TCPSESSIONMANAGER_H_
#define PLUGINS_TCP_SRC_TCPSESSIONMANAGER_H_

#include <time.h>
#include <sys/time.h>
#include <sstream>
#include <locale.h>

#include "SmGlobal.h"
#include "Log.h"
#include "MapStores.h"
#include "TcpSMInterface.h"

using namespace std;

class TcpSM : BaseConfig
{
	private:
			uint16_t		instanceId;

			bool 			tcpSMReadyState = false;
			long 			processStartEpochSec = 0;

			uint16_t 		lastIndex, curIndex, curIndexClnUp, lastIndexClnUp;

			timeval 		curTime;
			TcpSMInterface	*tcpSMInterface;

			void 			tcpProcessQueue(uint16_t t_index);
			void 			processQueue_sm(bool &smBusy, uint32_t &smCnt, std::unordered_map<uint32_t, TcpPacket> &pkt);

			void 			tcpProcessPacket(TcpPacket pkt);

	public:
			TcpSM(uint16_t id);
			~TcpSM();

			void			run();
			bool  			isTcpSMReady();
};

#endif /* PLUGINS_TCP_SRC_TCPSESSIONMANAGER_H_ */
