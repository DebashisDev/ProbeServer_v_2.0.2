/*
 * DnsSM.h
 *
 *  Created on: 29 Sep 2021
 *      Author: Debashis
 */

#ifndef PLUGINS_DNS_SRC_DNSSM_H_
#define PLUGINS_DNS_SRC_DNSSM_H_

#include <time.h>
#include <sys/time.h>
#include <sstream>
#include <locale.h>

#include "ServerGlobal.h"
#include "Log.h"
#include "DnsSMInterface.h"
#include "BaseConfig.h"
#include "MapStores.h"
#include "SmGlobal.h"


using namespace std;

class DnsSM : BaseConfig
{
	public:
		DnsSM(uint16_t id);
		~DnsSM();

		void	run();
		bool  	isInitialized();

	private:
		uint16_t		instanceId;
		bool 			initStats;
		uint16_t 		lastTidx, curTidx, curIndexClnUp, lastIndexClnUp;

		DnsSMInterface	*pDnsSMInterface;

		void 	processQueue(uint16_t t_index);
		void 	processPackets(bool &busy, uint32_t &cnt, std::unordered_map<uint32_t, DnsPacket> &store);

		void 	callInterface(DnsPacket msgObj);

		void	executeDayEndActivity();

};

#endif /* PLUGINS_DNS_SRC_DNSSM_H_ */
