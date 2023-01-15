/*
 * startSMRouters.h
 *
 *  Created on: Feb 5, 2019
 *      Author: Debashis
 */

#ifndef CORE_SRC_SMROUTERS_H_
#define CORE_SRC_SMROUTERS_H_

#include <sys/time.h>
#include "unistd.h"
#include "map"
#include "unordered_map"
#include <time.h>
#include <string.h>
#include <string>

#include "BaseConfig.h"
#include "Log.h"
#include "MapStores.h"
#include "ServerGlobal.h"
#include "SmGlobal.h"

class Routers : BaseConfig
{
	private:
		bool 		routersReadyState;
		uint16_t 	routerId;
		uint16_t 	intfId;
		timeval 	curTime;
		int16_t 	smId;

		void		storePacket(MPacket *msgObj);
		void		copyMsgObj(bool &sm_busy, int &sm_cnt, std::map<int, MPacket> &sm, MPacket *msgObj);

		void 		processPacket(uint16_t t_index);
		void 		readTcpPacket(bool &busy, uint32_t &cnt, std::unordered_map<uint32_t, TcpPacket*>&store);
		void 		readUdpPacket(bool &busy, uint32_t &cnt, std::unordered_map<uint32_t, UdpPacket*>&store);
		void 		readAaaPacket(bool &busy, uint32_t &cnt, std::unordered_map<uint32_t, AAAPacket*>&store);
		void 		readDnsPacket(bool &busy, uint32_t &cnt, std::unordered_map<uint32_t, DnsPacket*>&store);

		void 		findSmForTcpPacket(TcpPacket* msgObj);
		void 		findSmForUdpPacket(UdpPacket* msgObj);
		void 		findSmForAaaPacket(AAAPacket* msgObj);
		void 		findSmForDnsPacket(DnsPacket* msgObj);

		void 		pushTcpPacketToSm(int16_t &smid, TcpPacket *tcpMsg);
		void 		copyTcpObj(bool &ip_msg_sm_busy, uint32_t &ip_msg_sm_cnt, std::unordered_map<uint32_t, TcpPacket> &ip_msg_sm, TcpPacket *tcpMsg);

		void 		pushDnsPacketToSm(int16_t &smid, DnsPacket *dnsMsg);
		void 		copyDnsObj(bool &ip_msg_sm_busy, uint32_t &ip_msg_sm_cnt, std::unordered_map<uint32_t, DnsPacket> &ip_msg_sm, DnsPacket *dnsMsg);

		void 		pushUdpPacketToSm(int16_t &smid, UdpPacket *udpMsg);
		void 		copyUdpObj(bool &ip_msg_sm_busy, uint32_t &ip_msg_sm_cnt, std::unordered_map<uint32_t, UdpPacket> &ip_msg_sm, UdpPacket *udpMsg);

		void 		pushAaaPacketToSm(int16_t &smid, AAAPacket *aaaMsg);
		void 		copyAaaObj(bool &ip_msg_sm_busy, uint32_t &ip_msg_sm_cnt, std::unordered_map<uint32_t, AAAPacket> &ip_msg_sm, AAAPacket *aaaMsg);

	public:
		Routers(uint16_t interfaceId, uint16_t routerId);
		~Routers();
		void run();
		bool isSMRouterInitialized();
};

#endif /* CORE_SRC_SMROUTERS_H_ */
