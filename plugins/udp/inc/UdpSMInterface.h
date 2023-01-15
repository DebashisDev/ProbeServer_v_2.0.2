/*
 * UdpSMInterface.h
 *
 *  Created on: 18 Mar 2021
 *      Author: Debashis
 */

#ifndef PLUGINS_UDP_SRC_UDPSMINTERFACE_H_
#define PLUGINS_UDP_SRC_UDPSMINTERFACE_H_

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <string>

#include "ServerGlobal.h"
#include "SmGlobal.h"
#include "MapStores.h"
#include "SpectaTypedef.h"
#include "Log.h"
#include "BaseConfig.h"
#include "Initialize.h"
#include "FUtility.h"

using namespace std;

class UdpSMInterface : BaseConfig
{
	private:
			uint64_t 	ipV4Key;
			std::string ipV6Key;

			bool vpsFlag = true;

			uint16_t instanceId = 0;
			FUtility	*pFUtility;

			uint32_t udpFreeBitPos  = 0;
			uint32_t udpFreeBitPosMax = 0;
			std::bitset<UDP_SESSION_POOL_ARRAY_ELEMENTS> udpBitFlagsSession[UDP_SESSION_POOL_ARRAY_SIZE];
			std::map<uint32_t, udpSession*> udpSessionPoolMap[UDP_SESSION_POOL_ARRAY_SIZE];
			uint32_t 	udpGetFreeIndex();
			void 	udpReleaseIndex(uint32_t idx);
			void 	udpInitializeSessionPool();
			udpSession* udpGetSessionFromPool(uint32_t idx);
			uint32_t sessionCleanCnt;

			std::map<uint64_t, uint32_t> udpV4SessionMap[UDP_SESSION_POOL_ARRAY_ELEMENTS];
			std::map<std::string, uint32_t> udpV6SessionMap[UDP_SESSION_POOL_ARRAY_ELEMENTS];

			udpSession* 	udpGetSession(UdpPacket *msgObj, bool *found, bool create);

			void 			udpEraseSession(udpSession *pUdpSession);
			void 			udpInitializeSession(udpSession *pUdpSession, UdpPacket *msgObj);
			void			udpUpdateSession(udpSession *pUdpSession, UdpPacket *msgObj);

			void 			udpFlushSession(uint16_t id, udpSession *pUdpSession, bool erase);
			void 			udpStoreSession(uint16_t index, udpSession *pUdpSession);

			void 			udpCleanSession(udpSession *pUdpSession, bool endOfday);

			bool			checkDuplicate(udpSession *pUdpSession, UdpPacket *msgObj);
			bool			updateVPS(udpSession *pUdpSession, UdpPacket *msgObj);
			void			timeStampArrivalPacket(udpSession *pUdpSession, uint64_t epochSec, uint64_t epochNanoSec);
			void			updateTime(udpSession *pUdpSession, int id);

			uint32_t		getMapIndexAndSessionKey(UdpPacket *msgObj);

			void 			initializeUdpSession(udpSession *pUdpSession, UdpPacket *udpMsg);
			void			updateUdpSession(udpSession *pUdpSession, UdpPacket *udpMsg);

	public:
			UdpSMInterface(uint16_t id);
			~UdpSMInterface();

			void 	UDPPacketEntry(UdpPacket *msgObj);

			void 	udpTimeOutClean(bool endOfDay);
			void 	udpV4SessionCount();
			void 	udpV6SessionCount();
};

#endif /* PLUGINS_UDP_SRC_UDPSMINTERFACE_H_ */
