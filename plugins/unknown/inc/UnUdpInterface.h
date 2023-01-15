/*
 * UnUdpInterface.h
 *
 *  Created on: 16-Aug-2021
 *      Author: Debashis
 */

#ifndef PLUGINS_UNKNOWN_INC_UNUDPINTERFACE_H_
#define PLUGINS_UNKNOWN_INC_UNUDPINTERFACE_H_

#include <string>

#include "ServerGlobal.h"
#include "SmGlobal.h"
#include "MapStores.h"
#include "SpectaTypedef.h"
#include "Log.h"
#include "BaseConfig.h"
#include "Initialize.h"

#define DNS_FLUSH_REQ_RSP		30
#define DNS_FLUSH_RSP_REQ		31
#define DNS_FLUSH_CLEANUP_REQ_RSP	32
#define DNS_FLUSH_CLEANUP_REQ_NORSP	33
#define DNS_FLUSH_CLEANUP_RSP_NOREQ	34


class UnUdpInterface: BaseConfig
{
	private:
			uint64_t ipV4Key;
			std::string ipV6Key;

			ULONG freeIndexTime = 0;
			timeval curTime;
			bool vpsFlag = true;

			typedef struct _cleanObj
			{
				uint64_t ipv4key;
				string 	 ipv6key;
				uint32_t poolIndex;
			}cleanObj;

			uint16_t instanceId = 0;

			uint32_t udpFreeBitPos  = 0;
			uint32_t udpFreeBitPosMax;

			std::bitset<UNM_SESSION_POOL_ARRAY_ELEMENTS> udpBitFlagsSession[UNM_SESSION_POOL_ARRAY_SIZE];
			std::map<uint32_t, udpSession*> udpSessionPoolMap[UNM_SESSION_POOL_ARRAY_SIZE];
			uint32_t 	udpGetFreeIndex();
			void 	udpReleaseIndex(uint32_t idx);
			void 	udpInitializeSessionPool();
			udpSession* udpGetSessionFromPool(uint32_t idx);
			uint32_t cleanUpCnt;

			std::map<uint64_t, uint32_t> udpV4SessionMap[UNM_SESSION_POOL_ARRAY_ELEMENTS];
			std::map<std::string, uint32_t> udpV6SessionMap[UNM_SESSION_POOL_ARRAY_ELEMENTS];

			udpSession* 	udpGetSession(UdpPacket *msgObj, bool *found, bool create);

			void 			udpEraseSession(udpSession *pUdpSession);
			void 			udpInitializeSession(udpSession *pUdpSession, UdpPacket *msgObj);
			void			udpUpdateSession(udpSession *pUdpSession, UdpPacket *msgObj);

			void 			udpFlushSession(uint16_t id, udpSession *pUdpSession, bool erase);
			void 			udpStoreSession(uint16_t idx, udpSession *pUdpSession);

			void 			udpCleanSession(udpSession *pUdpSession, bool endOfday);

			bool			checkDuplicate(udpSession *pUdpSession, UdpPacket *msgObj);
			void			timeStampArrivalPacket(udpSession *pUdpSession, uint64_t epochSec, uint64_t epochNanoSec);
			void			updateTime(udpSession *pUdpSession, int id);

			uint32_t		getMapIndexAndSessionKey(UdpPacket *msgObj);

			void 			initializeUdpSession(udpSession *pUdpSession, UdpPacket *udpMsg);
			void			updateUdpSession(udpSession *pUdpSession, UdpPacket *udpMsg);

			/* ---- DNS ----- */

			uint32_t freeBitPosDns  = 0;
			uint32_t freeBitPosDnsMax = 0;
			std::bitset<DNS_SESSION_POOL_ARRAY_ELEMENTS> bitFlagsDnsSession[DNS_SESSION_POOL_ARRAY_SIZE];
			std::map<uint32_t, dnsSession*> dnsSessionPoolMap[DNS_SESSION_POOL_ARRAY_SIZE];
			uint32_t 	getFreeIndexDns();
			void 	releaseIndexDns(uint32_t idx);
			void 	dnsinitializeSessionPool();

			dnsSession* getDnsSessionFromPool(uint32_t idx);
			std::map<uint32_t, cleanObj> dnsSessionCleanUpMap;
			uint32_t dnsSessionCleanUpMap_cnt = 0;

			std::map<uint64_t, uint32_t> ipV4dnsSessionMap;
			std::map<string, uint32_t> ipV6dnsSessionMap;

			dnsSession* getDnsSession(UdpPacket *msgObj, bool *found);

			void 		getIpv6DNSSessionKey(std::string &key, char* userAddrLong, uint32_t dnsTransactionId, uint16_t port);
			void		getIpv4DNSSessionKey(uint64_t &key, uint32_t userAddrLong, uint16_t port, uint32_t destAddrLong, uint32_t dnsTransactionId);

			void 		flushDnsSession(dnsSession *pDnsSession, uint16_t type);
			void		storeDnsSession(uint16_t idx, dnsSession *pDnsSession);

	public:
			UnUdpInterface(uint16_t id);
			~UnUdpInterface();

			void 	UDPPacketEntry(UdpPacket *msgObj);
			void 	DNSPacketEntry(UdpPacket *msgObj);

			void 	udpTimeOutClean(bool endOfDay);
			void	dnsIpV4LookUpCount();

			void	dnsTimeOutClean();

			void	initializeResolvedIpv4();
			void	initializeResolvedIpv6();
};

#endif /* PLUGINS_UNKNOWN_INC_UNUDPINTERFACE_H_ */
