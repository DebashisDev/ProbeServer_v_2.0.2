/*
 * UnSessionManager.h
 *
 *  Created on: 15-Aug-2021
 *      Author: Debashis
 */

#ifndef PLUGINS_UNKNOWN_INC_UNSM_H_
#define PLUGINS_UNKNOWN_INC_UNSM_H_

#include <time.h>
#include <sys/time.h>
#include <sstream>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "SmGlobal.h"
#include "BaseConfig.h"
#include "UnTcpInterface.h"
#include "UnUdpInterface.h"
#include "Log.h"
#include "MapStores.h"

using namespace std;

class UnSM: BaseConfig
{
	private:
		bool 			unSMReadyState;
		uint16_t		instanceId;
		uint16_t 		lastIndex;
		uint16_t 		curIndex;
		uint16_t 		curMin, prevMin;
		uint16_t 		curIndexClnUp, lastIndexClnUp;

		UnTcpInterface	*unTcpSM;
		UnUdpInterface	*unUdpSM;

		void 			ProcessQueue(uint16_t t_index);
		void 			tcpProcessQueue_sm(bool &smBusy, uint32_t &smCnt, std::unordered_map<uint32_t, TcpPacket> &pkt);

		void 			tcpProcessPacket(TcpPacket tcpMsg);

		void			lockDnsMap();
		void			unLockDnsMap();

		void 			udpProcessQueue_sm(bool &smBusy, uint32_t &smCnt, std::unordered_map<uint32_t, UdpPacket> &pkt);
		void 			udpProcessPacket(UdpPacket udpMsg);


	public:
		UnSM(uint16_t id);
		~UnSM();

		void			run();
		bool  			isUnknownSMReady();
};

#endif /* PLUGINS_UNKNOWN_INC_UNSM_H_ */
