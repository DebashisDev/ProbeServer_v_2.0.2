/*
 * ProbeStats.h
 *
 *  Created on: Feb 1, 2017
 *      Author: Debashis
 */

#ifndef CORE_SRC_PROBESTATS_H_
#define CORE_SRC_PROBESTATS_H_

#include <unistd.h>
#include <locale.h>
#include <time.h>
#include <sys/time.h>
#include <ctime>

#include "BaseConfig.h"
#include "SpectaTypedef.h"
#include "Log.h"
#include "MapStores.h"
#include "SmGlobal.h"

class ProbeStats : public BaseConfig
{
	public:
		ProbeStats();
		~ProbeStats();
		void run();
		bool isProbeStatInitialized();

	private:
		timeval curTime;
		bool isStatsInitialized;

		void printRouterCount(char *runTime);
		void printInterfaceRouterStoreStats();
		void printIpXdrFlushStats();

		void printSessionStoreCount();

		void printPktReceived();

		void printTcpSessionStoreCount();
		void printUdpSessionStoreCount();
		void printAaaSessionStoreCount();
};

#endif /* CORE_SRC_PROBESTATS_H_ */
