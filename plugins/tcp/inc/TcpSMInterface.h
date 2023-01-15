/*
 * IPSMInterface.h
 *
 *  Created on: 20-Jul-2016
 *      Author: Debashis
 */

#ifndef PLUGINS_TCP_SRC_IPSMINTERFACE_H_
#define PLUGINS_TCP_SRC_IPSMINTERFACE_H_

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
#include "FUtility.h"

using namespace std;

class TcpSMInterface : BaseConfig
{
	private:
			uint64_t 	ipV4Key;
			std::string ipV6Key;

			bool vpsFlag = true;

			uint16_t instanceId;
			uint16_t tIdx;
			uint16_t flusherId;
			uint16_t cleanUpCnt;

			FUtility 	*pFUtility;

			uint32_t tcpFreeBitPos  = 0;
			uint32_t tcpFreeBitPosMax = 0;

			std::bitset<TCP_SESSION_POOL_ARRAY_ELEMENTS> tcpBitFlagsSession[TCP_SESSION_POOL_ARRAY_SIZE];
			std::map<uint32_t, tcpSession*> tcpSessionPoolMap[TCP_SESSION_POOL_ARRAY_SIZE];

			uint32_t 	tcpGetFreeIndex();
			void 		tcpReleaseIndex(uint32_t idx);
			void 		initSessionPool();

			tcpSession* tcpGetSessionFromPool(uint32_t idx);

			std::map<uint64_t, uint32_t> tcpV4SessionMap[TCP_SESSION_POOL_ARRAY_ELEMENTS];
			std::map<std::string, uint32_t> tcpV6SessionMap[TCP_SESSION_POOL_ARRAY_ELEMENTS];

			tcpSession* tcpGetSession(TcpPacket *msgObj, bool *found, bool create);
			void 		tcpEraseSession(tcpSession *pTcpSession);

			void 		tcpInitializeSession(tcpSession *pTcpSession, TcpPacket *msgObj);
			void		tcpUpdateSession(tcpSession *pTcpSession, TcpPacket *msgObj);

			void 		tcpFlushSession(uint16_t id, tcpSession *pTcpSession, bool erase);
			void 		tcpStoreSession(uint16_t index, tcpSession *pTcpSession);

			void 		tcpCleanSession(tcpSession *pTcpSession, bool endOfDay);

			bool		checkDuplicate(tcpSession *pTcpSession, TcpPacket *msgObj);
			bool		updateVPS(tcpSession *pTcpSession, TcpPacket *msgObj);
			void		timeStampArrivalPacket(tcpSession *pTcpSession, uint64_t epochSec, uint64_t epochNanoSec);
			void		updateTime(tcpSession *pTcpSession, uint16_t id);

			void		getMapIndexAndSessionKey(TcpPacket *pkt, uint32_t &idx);
	public:
			TcpSMInterface(uint16_t id);
			~TcpSMInterface();

			void 	TCPPacketEntry(TcpPacket *msgObj);
			void 	tcpTimeOutClean(bool endOfDay);
};

#endif /* PLUGINS_TCP_SRC_IPSMINTERFACE_H_ */
