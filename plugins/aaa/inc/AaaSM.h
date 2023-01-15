/*
 * AaaSessionManager.h
 *
 *  Created on: 19 Mar 2021
 *      Author: Debashis
 */

#ifndef PLUGINS_AAA_INC_AAASM_H_
#define PLUGINS_AAA_INC_AAASM_H_

#include <time.h>
#include <sys/time.h>
#include <sstream>
#include <locale.h>

#include "SmGlobal.h"
#include "Log.h"
#include "MapStores.h"
#include "AaaSMInterface.h"

using namespace std;

class AaaSM : BaseConfig
{
private:
		uint16_t		instanceId;
		uint16_t 		lastIndex;
		uint16_t 		curIndex;
		uint16_t 		curIndexClnUp;
		uint16_t 		lastIndexClnUp;

		bool 			aaaSMReadyState = false;
		long 			processStartEpochSec = 0;

		timeval 		curTime;
		AaaSMInterface*	aaaSMInterface;

		void 			aaaProcessQueue(uint16_t t_index);
		void 			processQueue(bool &smBusy, uint32_t &smCnt, std::unordered_map<uint32_t, AAAPacket> &pkt);

		void 			aaaProcessPacket(AAAPacket pkt);

		void 			lockAAAMap();
		void 			unLockAAAMap();

	public:
		AaaSM(uint16_t id);
		~AaaSM();
		void			run();
		bool  			isAaaSMReady();

		uint16_t		minCount;
};

#endif /* PLUGINS_AAA_INC_AAASM_H_ */
