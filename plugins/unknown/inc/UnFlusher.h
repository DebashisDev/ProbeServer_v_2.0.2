 /*
 * UnFlusher.h
 *
 *  Created on: 16-Aug-2021
 *      Author: Debashis
 */

#ifndef PLUGINS_UNKNOWN_INC_UNFLUSHER_H_
#define PLUGINS_UNKNOWN_INC_UNFLUSHER_H_

#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <iterator>
#include <list>

#include "FUtility.h"
#include "PUtility.h"
#include "SmGlobal.h"
#include "MapStores.h"
#include "Log.h"
#include "BaseConfig.h"

class UnFlusher: BaseConfig
{
	private:
		FUtility*	pFlUtility;

		bool		repoInitStatus;
		uint16_t 	lastIndex;
		uint16_t 	curIndex;
		uint16_t	curMin;
		uint16_t	prevMin;
		uint32_t 	totalCnt;

		char 		sIp[IPV6_ADDR_LEN];
		char 		dIp[IPV6_ADDR_LEN];

		char 		tcpXdr[XDR_MAX_LEN], udpXdr[XDR_MAX_LEN], dnsXdr[XDR_MAX_LEN];
		fstream	 	tcpXdrHandler, udpXdrHandler, dnsXdrHandler;

		void 	processTcpData(uint16_t idx);
		void 	processUdpData(uint16_t idx);
		void 	processDnsData(uint16_t idx);

		void 	flushTcpData(uint32_t &flCnt, std::unordered_map<uint32_t, tcpSession> &pkt);
		void 	flushUdpData(uint32_t &flCnt, std::unordered_map<uint32_t, udpSession> &pkt);
		void 	flushDnsData(uint32_t &flCnt, std::unordered_map<uint32_t, dnsSession> &pkt);

		bool 	createTcpXdrData(tcpSession pTcpSession);
		bool 	createUdpXdrData(udpSession pUdpSession);
		bool 	createDnsXdrData(dnsSession pDnsSession);

		void	buildTcpXdr(tcpSession *pTcpSession);
		void	buildUdpXdr(udpSession *pUdpSession);

		void 	openXdrFile(uint16_t protocol, uint16_t min, uint16_t hour, uint16_t day, uint16_t month, uint16_t year);
		void 	closeXdrFile(uint16_t protocol);

		bool 	IsIPInRange(uint32_t ip, uint32_t network, uint32_t mask);
		bool	checkPeeringDestIp(uint32_t sourceIp);

		bool 	check_I3DNET(uint32_t ip);
		bool 	check_VERISIGN(uint32_t ip);
		bool 	check_SUBSPACE(uint32_t ip);
		bool 	check_EDGECAST(uint32_t ip);
		bool 	check_FASTLY(uint32_t ip);
		bool 	check_PKT_CLEARING_HOUSE(uint32_t ip);
		bool 	check_HUAWEI_CLOUD(uint32_t ip);
		bool 	check_PEERING_TENCENT(uint32_t ip);
		bool 	check_ALIBABA(uint32_t ip);
		bool 	check_QUANTIL(uint32_t ip);
		bool 	check_MICROSOFT(uint32_t ip);
		bool 	check_LIMELIGHT(uint32_t ip);
		bool 	check_ZENLAYER(uint32_t ip);
		bool 	check_CLOUDFLARE(uint32_t ip);
		bool 	check_NETFLIX(uint32_t ip);
		bool 	check_AKAMAI(uint32_t ip);
		bool 	check_GOOGLE(uint32_t ip);
		bool 	check_FACEBOOK(uint32_t ip);
		bool 	check_AMAZON(uint32_t ip);
		bool 	check_DECIX(uint32_t ip);
		bool	check_CacheIpPool(uint32_t ip);
		bool	check_UnknownSourceIP(uint32_t ip);

	public:
		UnFlusher();
		~UnFlusher();

		bool 	isUnFlusherReady();
		void 	run();
};

#endif /* PLUGINS_UNKNOWN_INC_UNFLUSHER_H_ */
