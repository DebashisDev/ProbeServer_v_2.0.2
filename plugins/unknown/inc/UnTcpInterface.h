/*
 * UnTcpInetrface.h
 *
 *  Created on: 16-Aug-2021
 *      Author: Debashis
 */

#ifndef PLUGINS_UNKNOWN_INC_UNTCPINTERFACE_H_
#define PLUGINS_UNKNOWN_INC_UNTCPINTERFACE_H_

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

class UnTcpInterface : BaseConfig
{
	private:
			uint64_t ipV4Key;
			std::string ipV6Key;

			ULONG freeIndexTime = 0;
			timeval curTime;
			bool vpsFlag = true;
			uint16_t timeindex;

			uint16_t instanceId = 0;

			uint32_t tcpFreeBitPos  = 0;
			uint32_t tcpFreeBitPosMax;

			std::bitset<UNM_SESSION_POOL_ARRAY_ELEMENTS> tcpBitFlagsSession[UNM_SESSION_POOL_ARRAY_SIZE];
			std::map<uint32_t, tcpSession*> tcpSessionPoolMap[UNM_SESSION_POOL_ARRAY_SIZE];
			uint32_t 	tcpGetFreeIndex();
			void 	tcpReleaseIndex(uint32_t idx);
			void 	tcpInitializeSessionPool();
			tcpSession* tcpGetSessionFromPool(uint32_t idx);
			uint32_t cleanUpCnt;

			std::map<uint64_t, uint32_t> tcpV4SessionMap[UNM_SESSION_POOL_ARRAY_ELEMENTS];
			std::map<std::string, uint32_t> tcpV6SessionMap[UNM_SESSION_POOL_ARRAY_ELEMENTS];

			tcpSession* 	tcpGetSession(TcpPacket *msgObj, bool *found, bool create);

			void 		tcpEraseSession(tcpSession *pTcpSession);

			void 		tcpInitializeSession(tcpSession *pTcpSession, TcpPacket *msgObj);
			void		tcpUpdateSession(tcpSession *pTcpSession, TcpPacket *msgObj);

			void 		tcpFlushSession(uint16_t id, tcpSession *pTcpSession, bool erase);
			void 		tcpStoreSession(uint16_t index, tcpSession *pTcpSession);

			void 		tcpCleanSession(tcpSession *pTcpSession, bool endOfDay);

			void		timeStampArrivalPacket(tcpSession *pTcpSession, uint64_t epochSec, uint64_t epochNanoSec);

			uint32_t	getMapIndexAndSessionKey(TcpPacket *msgObj);

	public:
		UnTcpInterface(uint16_t id);
		~UnTcpInterface();

		void 	TCPPacketEntry(TcpPacket *msgObj);
		void 	tcpTimeOutClean(bool endOfDay);
};

#endif /* PLUGINS_UNKNOWN_INC_UNTCPINTERFACE_H_ */
