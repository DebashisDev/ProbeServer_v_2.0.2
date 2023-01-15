/*
 * DnsInterface.h
 *
 *  Created on: 29 Sep 2021
 *      Author: Debashis
 */

#ifndef PLUGINS_DNS_SRC_DNSSMINTERFACE_H_
#define PLUGINS_DNS_SRC_DNSSMINTERFACE_H_

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <string>

#include "ServerGlobal.h"
#include "SmGlobal.h"
#include "SpectaTypedef.h"
#include "Log.h"
#include "BaseConfig.h"
#include "Initialize.h"
#include "MapStores.h"

#define DNS_FLUSH_REQ_RSP			30
#define DNS_FLUSH_RSP_REQ			31
#define DNS_FLUSH_CLEANUP_REQ_RSP	32
#define DNS_FLUSH_CLEANUP_REQ_NORSP	33
#define DNS_FLUSH_CLEANUP_RSP_NOREQ	34

using namespace std;

class DnsSMInterface : BaseConfig
{
	public:
		DnsSMInterface(uint16_t id);
		~DnsSMInterface();

		void DnsPacketEntry(DnsPacket *udpMsg);
		void sessionTimeOutClean();
		void loadResolvedIpv4();
		void initializeResolvedIpv6();
		void dnsIpV4LookUpCount();

	private:
		uint16_t instanceId;
		uint32_t freeBitPosDns;
		uint32_t freeBitPosDnsMax;

		typedef struct _cleanObj
		{
			uint64_t ipv4key;
			string ipv6key;
			uint32_t poolIndex;
		}cleanObj;

		std::bitset<DNS_SESSION_POOL_ARRAY_ELEMENTS> bitFlagsDnsSession[DNS_SESSION_POOL_ARRAY_SIZE];
		std::map<uint32_t, dnsSession*> dnsSessionPoolMap[DNS_SESSION_POOL_ARRAY_SIZE];
		uint32_t 	getFreeIndexDns();
		void 	releaseIndexDns(uint32_t idx);
		void 	dnsinitializeSessionPool();

		dnsSession* getDnsSessionFromPool(uint32_t idx);
		std::map<uint32_t, cleanObj> dnsSessionCleanUpMap;
		uint32_t dnsSessionCleanCnt = 0;

		std::map<uint64_t, uint32_t> ipV4dnsSessionMap;
		std::map<string, uint32_t> ipV6dnsSessionMap;

		void		requestUpdateDnsSession(dnsSession *pDnsSession, DnsPacket *msgObj);
		void 		responseUpdateDnsSession(dnsSession *pDnsSession, DnsPacket *msgObj);
		dnsSession* getDnsSession(DnsPacket *msgObj, bool *found);

		void 		getIpv6DNSSessionKey(std::string &key, char* userAddrLong, uint32_t dnsTransactionId, uint16_t port);
		void		getIpv4DNSSessionKey(uint64_t &key, uint32_t userAddrLong, uint16_t port, uint32_t destAddrLong, uint32_t dnsTransactionId);

		void 		flushDnsSession(dnsSession *pDnsSession, int type);
		void		storeDnsSession(uint16_t idx, dnsSession *pDnsSession);

};

#endif /* PLUGINS_DNS_SRC_DNSSMINTERFACE_H_ */
