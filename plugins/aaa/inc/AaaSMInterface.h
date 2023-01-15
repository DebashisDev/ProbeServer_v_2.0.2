/*
 * radiusSessionManager.h
 *
 *  Created on: Oct 22, 2016
 *      Author: Debashis
 */

#ifndef PLUGINS_AAA_SRC_AAASMINTERFACE_H_
#define PLUGINS_AAA_SRC_AAASMINTERFACE_H_

#include "ServerGlobal.h"
#include "SmGlobal.h"
#include "MapStores.h"
#include "Log.h"
#include "BaseConfig.h"

#define START_STATUS_TYPE  	1
#define STOP_STATUS_TYPE  	2
#define UPDATE_STATUS_TYPE  3

class AaaSMInterface  : BaseConfig
{
	private:
		uint16_t instanceId = 0;
		uint16_t timeIndex;
		uint16_t flusherId;
		uint64_t aaaKey;

		typedef struct _cleanObj
		{
			std::string key;
			int mapIndex;
			int poolIndex;
		}cleanObj;

		std::map<uint64_t, aaaSession> accessMap[AAA_SESSION_POOL_ARRAY_SIZE];
		std::map<uint64_t, aaaSession> accountingMap[AAA_SESSION_POOL_ARRAY_SIZE];

		std::map<uint64_t, uint16_t> cleanMap;

		void			scanAccessMap(map<uint64_t, aaaSession> &pAccessMap);
		void			scanAccountingMap(map<uint64_t, aaaSession> &pAccountingMap);

		void 			cleanSession(uint32_t key, aaaSession *pRadiusSession, uint16_t count);
//		bool 			cleanSession(aaaSession pAaaSession);
		void 			eraseAccessSession();
		void 			eraseAccountingSession();

		aaaSession*		getAccessSession(AAAPacket *msgObj, bool *found);
		aaaSession*		getAccountingSession(AAAPacket *msgObj, bool *found);
		void 			eraseSession(aaaSession *pRadiusSession);

		void 			createSession(aaaSession *pRadiusSession, AAAPacket *msgObj);
		void 			updateSession(aaaSession *pRadiusSession, AAAPacket *msgObj);

		void 			processAccessRequest(AAAPacket *msgObj);
		void 			processAccessResponse(AAAPacket *msgObj);

		void 			processAccountingRequest(AAAPacket *msgOb);
		void 			processAccountingResponse(AAAPacket *msgOb);

		void			updateGlbTable(aaaSession *pRadiusSession);

		void 			flushSession(aaaSession *pRadiusSession, bool erase, uint16_t flushType);
		void 			storeSession(uint16_t idx, aaaSession *pRadiusSession);

		void			getSessionKey(AAAPacket *aaaObj, uint64_t &key);

		std::vector<cleanObj*> sessionCleanUpVector;
		long sessionCleanCount = 0;

	public:
		AaaSMInterface(uint16_t id);
		~AaaSMInterface();

		void 	lockAAAMap();
		void 	unLockAAAMap();

		void 	packetEntry(AAAPacket *aaaObj);

		void 	glbSessionCount();
		void 	timeOutCleanSession();

		void	aaaDumpUserInfo(uint16_t hour);
		void 	aaaLoadUserInfo();
};

#endif /* PLUGINS_AAA_SRC_AAASMINTERFACE_H_ */
