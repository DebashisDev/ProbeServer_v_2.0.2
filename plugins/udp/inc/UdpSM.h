/*
 * UdpSessionManager.h
 *
 *  Created on: 18 Mar 2021
 *      Author: Debashis
 */

#ifndef PLUGINS_UDP_SRC_UDPSESSIONMANAGER_H_
#define PLUGINS_UDP_SRC_UDPSESSIONMANAGER_H_

#include <time.h>
#include <sys/time.h>
#include <sstream>
#include <locale.h>

#include "SmGlobal.h"
#include "Log.h"
#include "MapStores.h"
#include "UdpSMInterface.h"

using namespace std;

class UdpSM : BaseConfig
{
	private:

		bool 			udpSMReadyState;
		uint16_t		instanceId;
		uint16_t 		lastIndex;
		uint16_t 		curIndex;
		uint16_t 		curIndexClnUp;
		uint16_t 		lastIndexClnUp;

		UdpSMInterface	*udpSMInterface;

		void 			udpProcessQueue(uint16_t t_index);
		void 			processQueue_sm(bool &smBudy, uint32_t &smCnt, std::unordered_map<uint32_t, UdpPacket> &pkt);

		void 			udpProcessPacket(UdpPacket udpMsg);

	public:
		UdpSM(uint16_t id);
		~UdpSM();

		void			run();
		bool  			isUdpSMReady();
};

#endif /* PLUGINS_UDP_SRC_UDPSESSIONMANAGER_H_ */
