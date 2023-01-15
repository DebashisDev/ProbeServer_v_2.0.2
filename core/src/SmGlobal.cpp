/*
 * TCPGlobal.cpp
 *
 *  Created on: Nov 14, 2015
 *      Author: Debashis
 */

#include "SmGlobal.h"

using namespace std;

SmGlobal::SmGlobal()
{};

SmGlobal::~SmGlobal()
{};

namespace initSection
{
	std::map<uint8_t, std::string> protocolName;
	std::map<uint16_t, std::string> dnsErrorCode;
	std::map<uint16_t, std::string> tcpPorts;
	std::map<uint32_t, std::string> radiusCodeMap;
	std::map<uint32_t, std::string> serviceTypeMap;
	std::map<uint32_t, std::string> framedProtocolMap;
	std::map<uint32_t, std::string> acctAuthenticMap;
	std::map<uint32_t, std::string> acctTeminateMap;
	std::map<uint32_t, std::string> acctStatusMap;
	std::map<uint32_t, std::string> nasPortTypeMap;
	std::map<uint32_t, std::string> ipSubNetMap;
}

namespace IPStats
{
	uint64_t dnsLookupMapSize 			= 0;

	uint32_t tcpV4SessionScanned[MAX_TCP_SM_SUPPORT] = {0};
	uint32_t tcpV4SessionCleaned[MAX_TCP_SM_SUPPORT] = {0};
	uint32_t tcpV4SessionTotalCnt[MAX_TCP_SM_SUPPORT] = {0};

	uint32_t tcpV6SessionScanned[MAX_TCP_SM_SUPPORT] = {0};
	uint32_t tcpV6SessionCleaned[MAX_TCP_SM_SUPPORT] = {0};
	uint32_t tcpV6SessionTotalCnt[MAX_TCP_SM_SUPPORT] = {0};

	uint32_t udpV4SessionScanned[MAX_UDP_SM_SUPPORT] = {0};
	uint32_t udpV4SessionCleaned[MAX_UDP_SM_SUPPORT] = {0};
	uint32_t udpV4SessionTotalCnt[MAX_UDP_SM_SUPPORT] = {0};

	uint32_t udpV6SessionScanned[MAX_UDP_SM_SUPPORT] = {0};
	uint32_t udpV6SessionCleaned[MAX_UDP_SM_SUPPORT] = {0};
	uint32_t udpV6SessionTotalCnt[MAX_UDP_SM_SUPPORT] = {0};

	uint32_t dnsV4SessionScanned[MAX_DNS_SM_SUPPORT] = {0};
	uint32_t dnsV4SessionCleaned[MAX_DNS_SM_SUPPORT] = {0};
	uint32_t dnsV4SessionTotalCnt[MAX_DNS_SM_SUPPORT] = {0};

	uint32_t dnsV6SessionScanned[MAX_DNS_SM_SUPPORT] = {0};
	uint32_t dnsV6SessionCleaned[MAX_DNS_SM_SUPPORT] = {0};
	uint32_t dnsV6SessionTotalCnt[MAX_DNS_SM_SUPPORT] = {0};

	uint32_t unTcpSessionCnt[MAX_UNM_SM_SUPPORT] = {0};
	uint32_t unTcpSessionScanned[MAX_UNM_SM_SUPPORT] = {0};
	uint32_t unTcpSessionCleaned[MAX_UNM_SM_SUPPORT] = {0};

	uint32_t unUdpSessionCnt[MAX_UNM_SM_SUPPORT] = {0};
	uint32_t unUdpSessionScanned[MAX_UNM_SM_SUPPORT] = {0};
	uint32_t unUdpSessionCleaned[MAX_UNM_SM_SUPPORT] = {0};

	uint32_t aaaAccessSessionCnt[MAX_AAA_SM_SUPPORT] = {0};
	uint32_t aaaAccessSessionScanned[MAX_AAA_SM_SUPPORT] = {0};
	uint32_t aaaAccessSessionCleaned[MAX_AAA_SM_SUPPORT] = {0};

	uint32_t aaaAccounSessionCnt[MAX_AAA_SM_SUPPORT] = {0};
	uint32_t aaaAccounSessionScanned[MAX_AAA_SM_SUPPORT] = {0};
	uint32_t aaaAccounSessionCleaned[MAX_AAA_SM_SUPPORT] = {0};
}

namespace SMGlobal
{
	/* ** Interface Settings ** */
	uint16_t	NO_OF_INTERFACE_SUPPORT					= 1;
	string		INTERFACE_NAME[MAX_INTERFACE_SUPPORT] 	=	{""};

	/* ** Agents Setting ** */
	uint64_t	TCP_PACKET_PER_DAY[MAX_INTERFACE_SUPPORT] = {0};
	uint64_t	UDP_PACKET_PER_DAY[MAX_INTERFACE_SUPPORT] = {0};
	uint64_t	AAA_PACKET_PER_DAY[MAX_INTERFACE_SUPPORT] = {0};
	uint64_t	DNS_PACKET_PER_DAY[MAX_INTERFACE_SUPPORT] = {0};

	string 		TCP_SERVER_ADDRESS[MAX_INTERFACE_SUPPORT] = {""};
	string 		UDP_SERVER_ADDRESS[MAX_INTERFACE_SUPPORT] = {""};
	string 		AAA_SERVER_ADDRESS[MAX_INTERFACE_SUPPORT] = {""};
	string 		DNS_SERVER_ADDRESS[MAX_INTERFACE_SUPPORT] = {""};

	uint32_t 	TCP_PPS_PER_INTERFACE[MAX_INTERFACE_SUPPORT] = {0};
	uint32_t 	UDP_PPS_PER_INTERFACE[MAX_INTERFACE_SUPPORT] = {0};
	uint32_t 	AAA_PPS_PER_INTERFACE[MAX_INTERFACE_SUPPORT] = {0};
	uint32_t 	DNS_PPS_PER_INTERFACE[MAX_INTERFACE_SUPPORT] = {0};

	bool		TCP_AGENT_RUNNING_STATUS[MAX_INTERFACE_SUPPORT] = {false};
	bool		UDP_AGENT_RUNNING_STATUS[MAX_INTERFACE_SUPPORT] = {false};
	bool		AAA_AGENT_RUNNING_STATUS[MAX_INTERFACE_SUPPORT] = {false};
	bool		DNS_AGENT_RUNNING_STATUS[MAX_INTERFACE_SUPPORT] = {false};

	uint16_t 	TCP_AGENT_CORE[MAX_INTERFACE_SUPPORT] = {0};
	uint16_t 	UDP_AGENT_CORE[MAX_INTERFACE_SUPPORT] = {0};
	uint16_t 	AAA_AGENT_CORE[MAX_INTERFACE_SUPPORT] = {0};
	uint16_t 	DNS_AGENT_CORE[MAX_INTERFACE_SUPPORT] = {0};

	/* ** Packet Received by SM ** */
	uint64_t	TCP_SM_PKT_RECEIVED[MAX_TCP_SM_SUPPORT]	= {0};
	uint64_t	UDP_SM_PKT_RECEIVED[MAX_UDP_SM_SUPPORT] = {0};
	uint64_t	DNS_SM_PKT_RECEIVED[MAX_DNS_SM_SUPPORT] = {0};
	uint64_t	AAA_SM_PKT_RECEIVED[MAX_AAA_SM_SUPPORT] = {0};
	uint64_t	UNM_SM_PKT_RECEIVED[MAX_UNM_SM_SUPPORT] = {0};

	/* ** Router Settings ** */
	bool		ROUTER_RUNNING_STATUS[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE]  = {0};
	uint16_t 	ROUTER_PER_INTERFACE[MAX_INTERFACE_SUPPORT] 							= {0};
	uint16_t	ROUTER_CORE[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE] 			= {0};

	/* ** Session Manager Settings ** */
	bool		TCP_SM_RUNNING_STATUS[MAX_TCP_SM_SUPPORT] = {false};
	bool		UDP_SM_RUNNING_STATUS[MAX_UDP_SM_SUPPORT] = {false};
	bool		DNS_SM_RUNNING_STATUS[MAX_DNS_SM_SUPPORT] = {false};
	bool		AAA_SM_RUNNING_STATUS[MAX_AAA_SM_SUPPORT] = {false};
	bool		UNM_SM_RUNNING_STATUS[MAX_UNM_SM_SUPPORT] = {false};

	uint16_t	TCP_SM_INSTANCES = 1;
	uint16_t	UDP_SM_INSTANCES = 1;
	uint16_t	DNS_SM_INSTANCES = 1;
	uint16_t 	AAA_SM_INSTANCES = 1;
	uint16_t	UNM_SM_INSTANCES = 1;

	uint16_t 	TCP_SM_CORE[MAX_TCP_SM_SUPPORT] = {0};
	uint16_t 	UDP_SM_CORE[MAX_UDP_SM_SUPPORT] = {0};
	uint16_t 	DNS_SM_CORE[MAX_DNS_SM_SUPPORT] = {0};
	uint16_t 	AAA_SM_CORE[MAX_AAA_SM_SUPPORT] = {0};
	uint16_t 	UNM_SM_CORE[MAX_UNM_SM_SUPPORT] = {0};

	/* ** Flusher Settings ** */
	bool		TCP_FLUSHER_RUNNING_STATUS[MAX_FLUSHER_SUPPORT] = {false};
	bool		UDP_FLUSHER_RUNNING_STATUS[MAX_FLUSHER_SUPPORT] = {false};
	bool		DNS_FLUSHER_RUNNING_STATUS[MAX_FLUSHER_SUPPORT] = {false};
	bool		AAA_FLUSHER_RUNNING_STATUS[MAX_FLUSHER_SUPPORT] = {false};
	bool		UNM_FLUSHER_RUNNING_STATUS 						= false;

	uint16_t 	TCP_FLUSHER_INSTANCE = 1;
	uint16_t 	UDP_FLUSHER_INSTANCE = 1;
	uint16_t 	DNS_FLUSHER_INSTANCE = 1;
	uint16_t 	AAA_FLUSHER_INSTANCE = 1;
	uint16_t 	UNM_FLUSHER_INSTANCE = 1;

	uint16_t 	TCP_FLUSHER_CORE[MAX_FLUSHER_SUPPORT] = {0};
	uint16_t 	UDP_FLUSHER_CORE[MAX_FLUSHER_SUPPORT] = {0};
	uint16_t 	DNS_FLUSHER_CORE[MAX_FLUSHER_SUPPORT] = {0};
	uint16_t 	AAA_FLUSHER_CORE[MAX_FLUSHER_SUPPORT] = {0};
	uint16_t 	UNM_FLUSHER_CORE 					  = 0;

	/* ** XDR File Name Prefix ** */
	string  	TCP_FILE_PREFIX	= "tcp";
	string 	 	UDP_FILE_PREFIX	= "udp";
	string  	DNS_FILE_PREFIX	= "dns";
	string  	AAA_FILE_PREFIX	= "aaa";

	/* ** Cleanup Settings ** */
	uint16_t	TCP_CLEAN_UP_TIMEOUT_SEC = 120;
	uint16_t	UDP_CLEAN_UP_TIMEOUT_SEC = 120;
	uint16_t	DNS_CLEAN_UP_TIMEOUT_SEC = 120;
	uint16_t	AAA_CLEAN_UP_TIMEOUT_SEC = 120;

	uint16_t 	SESSION_SCAN_FREQ_SEC 	 = 15;

	/* ** End of the Day Settings ** */
	uint16_t	DAY_CLEAN_HOUR	= 23;
	uint16_t	DAY_CLEAN_MIN	= 59;
	uint16_t	DAY_CLEAN_SEC	= 30;

	/* ** Threads Running Settings ** */

	bool		SERVER_RUNNING_STATUS 	= true;
	bool		STATSLOG_RUNNING_STATUS	= false;
	bool		LOG_RUNNING_STATUS		= true;
	bool		TIMER_RUNNING_STATUS 	= false;

	uint16_t	THREAD_SLEEP_TIME		= 10000;
	uint16_t	TIMER_THREAD_CORE		= 0;

	/* ** Directories Settings ** */
	string		LOG_DIR;
	string		XDR_DIR;
	string		DATA_DIR[5] 	=	{""};
	string		UNM_DIR;

	/* ** Admin Settings ** */
	bool		ADMIN_FLAG	= false;
	string		ADMIN_PORT;

	/* ** TCP Settings ** */
	uint16_t	VPS_PACKET_PER_SEC	= 1000;
	uint16_t	SESSION_PKT_LIMIT 	= 5000;
	uint16_t 	SESSION_TIME_LIMIT 	= 900;
	bool		CHECK_DUPLICATE_PKT = false;
	bool		PROCESS_ACK 		= false;
	bool		ACK_CREATE_SESSION 	= false;

	/* ** UDP Settings ** */
	bool 		UDP_XDR_FOR_DNS 	= false;
	uint16_t	DNS_DUMP_HOUR 		= 4;

	/* ** Log Settings ** */

	bool		PRINT_STATS				= false;
	uint16_t	PROBE_ID 				= 10;
	uint16_t	INSTANCE_ID 			= 0;
	uint16_t	LOG_LEVEL 				= 4;
	uint16_t 	PRINT_STATS_FREQ_SEC 	= 30;
	uint16_t	LOG_STATS_FREQ_SEC 		= 30;

	uint16_t	DNS_LOAD_FREQ_SEC	 		= 480;	/* 08 Minutes */
	uint16_t	AAA_LOAD_FREQ_SEC	 		= 600;	/* 10 Minutes */
	uint16_t	AAA_DUMP_FREQ_SEC	 		= 180;	/* 03 Minutes */

	uint32_t	PEERING_DECIX[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_DECIX_COUNT = 0;

	uint32_t	PEERING_I3DNET[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_I3DNET_COUNT = 0;

	uint32_t	PEERING_VERISIGN[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_VERISIGN_COUNT = 0;

	uint32_t	PEERING_SUBSPACE[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_SUBSPACE_COUNT = 0;

	uint32_t	PEERING_EDGECAST[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_EDGECAST_COUNT = 0;

	uint32_t	PEERING_FASTLY[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_FASTLY_COUNT = 0;

	uint32_t	PEERING_PKT_CLEARING_HOUSE[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_PKT_CLEARING_HOUSE_COUNT = 0;

	uint32_t	PEERING_HUAWEI_CLOUD[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_HUAWEI_CLOUD_COUNT = 0;

	uint32_t	PEERING_TENCENT[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_TENCENT_COUNT = 0;

	uint32_t	PEERING_ALIBABA[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_ALIBABA_COUNT = 0;

	uint32_t	PEERING_QUANTIL[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_QUANTIL_COUNT = 0;

	uint32_t	PEERING_MICROSOFT[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_MICROSOFT_COUNT = 0;

	uint32_t	PEERING_LIMELIGHT[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_LIMELIGHT_COUNT = 0;

	uint32_t	PEERING_ZENLAYER[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_ZENLAYER_COUNT = 0;

	uint32_t	PEERING_CLOUDFLARE[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_CLOUDFLARE_COUNT = 0;

	uint32_t	PEERING_NETFLIX[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_NETFLIX_COUNT = 0;

	uint32_t	PEERING_AKAMAI[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_AKAMAI_COUNT = 0;

	uint32_t	PEERING_GOOGLE[MAX_PEERING_IP][2] = {0};
	uint16_t 	PEERING_GOOGLE_COUNT = 0;

	uint32_t	PEERING_FACEBOOK[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_FACEBOOK_COUNT = 0;

	uint32_t	PEERING_AMAZON[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_AMAZON_COUNT = 0;

	uint32_t	UNKNOWN_SOURCE_POOL[MAX_PEERING_IP][2] = {0};
	uint16_t	UNKNOWN_SOURCE_POOL_COUNT = 0;

	uint32_t	CACHE_IP_POOL[MAX_PEERING_IP][2] = {0};
	uint16_t	CACHE_IP_POOL_COUNT = 0;

}

namespace DNSGlobal
{
	std::map<uint32_t, std::string> dnsLookUpMap[10];
	std::map<std::string, std::string> dnsV6LookUpMap;
}

namespace aaaGlbMap
{
	std::map<uint32_t, userInfo> aaaGlbUserIpMap;	/* 01295072520@airtelbroadband.in */
	std::map<string, userInfo> aaaGlbUserIdMap;
	std::map<std::string, userInfo> aaaGlbIpv6UserMap;
}

namespace mapDnsLock
{
	pthread_mutex_t lockCount = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t nonzero = PTHREAD_COND_INITIALIZER;
	unsigned count;
}

namespace mapAAALock
{
	pthread_mutex_t lockCount = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t nonzero = PTHREAD_COND_INITIALIZER;
	unsigned count;
}
