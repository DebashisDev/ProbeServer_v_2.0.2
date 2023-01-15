/*
 * GConfig.h
 *
 *  Created on: 26-Jul-2016
 *      Author: Debashis
 */

#ifndef CORE_SRC_GCONFIG_H_
#define CORE_SRC_GCONFIG_H_

#include <string.h>
#include <fstream>
#include <iostream>
#include <stack>
#include <vector>
#include <math.h>

#include "SmGlobal.h"
#include "SpectaTypedef.h"

#define NUM_OCTETTS 4

using namespace std;

class GConfig
{
	private:
		ifstream 	fp;
		string 		Key, Value;
		char 		startIp[16], endIp[16];
		uint32_t 	x = 0;

		void	loadInterfaceSettings(char *fileName);
		void	loadAgentsSettings(char *fileName);
		void	loadRoutersSettings(char *fileName);
		void	loadSmSettings(char *fileName);
		void	loadFlushersSettings(char *fileName);
		void	loadFilePrefix(char *filename);
		void	loadCleanUpSettings(char *filename);
		void	loadEndOfDayCleanTime(char *filename);
		void	loadTimerCore(char *filename);
		void	loadDirSettings(char *filename);
		void	loadAdminSettings(char *filename);
		void	loadTcpSettings(char *filename);
		void	loadUdpSettings(char *filename);
		void	loadLogSettings(char *filename);
		void	loadPeeringIps(char *filename);
		void	get_instanceId(std::string& Key);

		void 	openConfigFile(char *fileName);
		void 	closeConfigFile();

		void 	get_probeId(std::string& Key);
		void	get_logLevel(std::string& Key);
		void 	get_printStats(std::string& Key);
		void 	get_printStatsFreq(std::string& Key);
		void	get_logStatsFreq(std::string& Key);
		void 	get_logDir(std::string& Key);
		void 	get_xdrDir(std::string& Key);
		void 	get_unmDir(std::string& Key);
		void 	get_dataDir(std::string& Key);
		void	get_adminFlag(std::string& Key);
		void	get_adminPort(std::string& Key);

		void	get_dnsLoadFreq(std::string& Key);
		void	get_aaaLoadFreq(std::string& Key);
		void	get_aaaDumpFreq(std::string& Key);

		void	get_timerThrCore(std::string& Key);
		void 	get_noOfInterface(std::string& Key);
		void 	get_InterfaceName(std::string& Key);

		void	get_tcpPPSPerInterface(std::string& Key);
		void	get_udpPPSPerInterface(std::string& Key);
		void	get_aaaPPSPerInterface(std::string& Key);
		void	get_dnsPPSPerInterface(std::string& Key);

		void	get_tcpAgentCpuCore(std::string& Key);
		void	get_udpAgentCpuCore(std::string& Key);
		void	get_aaaAgentCpuCore(std::string& Key);
		void	get_dnsAgentCpuCore(std::string& Key);

		void	get_serverTcpAddress(std::string& Key);
		void	get_serverUdpAddress(std::string& Key);
		void	get_serverAaaAddress(std::string& Key);
		void	get_serverDnsAddress(std::string& Key);

		void	get_routerPerInterface(std::string& Key);
		void	get_routerCPU(std::string& Key);
		void	get_vpsPacketPerSec(std::string& Key);

		void	get_endOfDayCleanTime(std::string& Key);

		void	get_noOfTcpSm(std::string& Key);
		void	get_tcpSmCore(std::string& Key);
		void	get_noOfTcpFlusher(std::string& Key);
		void	get_tcpFlusherCore(std::string& Key);
		void	get_tcpFilePrefix(std::string& Key);
		void	get_checkDuplicatePkt(std::string& Key);
		void	get_checkProcessAck(std::string& Key);
		void	get_checkAckCreateSession(std::string& Key);
		void	get_sessionPacketLimit(std::string& Key);
		void	get_sessionTimeLimit(std::string& Key);
		void	get_tcpCleanUpTimeSec(std::string& Key);

		void	get_noOfUdpSm(std::string& Key);
		void	get_udpSessionManagerTimeIndexPktLimit(std::string& Key);
		void	get_udpSmCore(std::string& Key);
		void	get_noOfUdpFlusher(std::string& Key);
		void	get_udpFlusherCore(std::string& Key);
		void	get_udpFilePrefix(std::string& Key);
		void	get_udpXdrForDns(std::string& Key);
		void	get_udpCleanUpTimeSec(std::string& Key);

		void	get_noOfDnsSm(std::string& Key);
		void	get_dnsSmCore(std::string& Key);
		void	get_noOfDnsFlusher(std::string& Key);
		void	get_dnsFlusherCore(std::string& Key);
		void	get_dnsFilePrefix(std::string& Key);
		void	get_dnsCleanUpTimeSec(std::string& Key);

		void	get_noOfAaaSm(std::string& Key);
		void	get_aaaSmCore(std::string& Key);
		void	get_noOfAaaFlusher(std::string& Key);
		void	get_aaaFlusherCore(std::string& Key);
		void	get_aaaFilePrefix(std::string& Key);
		void	get_aaaCleanUpTimeSec(std::string& Key);

		void	get_noOfUnmSm(std::string& Key);
		void	get_unmSmCore(std::string& Key);
		void	get_unmFlusherCore(std::string& Key);

		void	get_I3DNET(std::string& Key);				/* PEERING_I3DNET */
		void	get_VERISIGN(std::string& Key);				/* PEERING_VERISIGN */
		void	get_SUBSPACE(std::string& Key);				/* PEERING_SUBSPACE */
		void	get_EDGECAST(std::string& Key);				/* PEERING_EDGECAST */
		void	get_FASTLY(std::string& Key);				/* PEERING_FASTLY */
		void	get_PKT_CLEARING_HOUSE(std::string& Key);	/* PEERING_PKT_CLEARING_HOUSE */
		void	get_HUAWEI_CLOUD(std::string& Key);			/* PEERING_HUAWEI_CLOUD */
		void	get_PEERING_TENCENT(std::string& Key);/* PEERING_TENCENT_GLOBAL  */
		void	get_ALIBABA(std::string& Key);				/* PEERING_ALIBABA */
		void	get_APPLE(std::string& Key);				/* PEERING_QUANTIL */
		void	get_MICROSOFT(std::string& Key);			/* PEERING_MICROSOFT */
		void	get_LIMELIGHT(std::string& Key);			/* PEERING_LIMELIGHT */
		void	get_ZENLAYER(std::string& Key);				/* PEERING_ZENLAYER */
		void	get_CLOUDFLARE(std::string& Key);			/* PEERING_CLOUDFLARE */
		void	get_DE_CIX(std::string& Key);				/* PEERING_NETFLIX */
		void	get_AKAMAI(std::string& Key);				/* PEERING_AKAMAI */
		void	get_NETFLIX(std::string& Key);				/* PEERING_NTFLIX */
		void	get_GOOGLE(std::string& Key);				/* PEERING_GOOGLE */
		void	get_FACEBOOK(std::string& Key);				/* PEERING_FACEBOOK */
		void	get_AMAZON(std::string& Key);				/* PEERING_AMAZON */
		void	get_UnknownSourcePool(std::string& Key);	/* UNKNOWN_SOURCE_POOL */
		void	get_CacheIpPool(std::string& Key);			/* CACHE_IP_POOL */

		void	converSubNetToRange(char *ipr, char *Start, char *End);
		uint32_t ipToLong(char *ip, uint32_t *plong);

	public:
		GConfig(char *fileName);
		~GConfig();
};

#endif /* CORE_SRC_GCONFIG_H_ */
