/*
 * ProbeStatsLog.h
 *
 *  Created on: Jul 21, 2017
 *      Author: Debashis
 */

#ifndef CORE_SRC_PROBESTATSLOG_H_
#define CORE_SRC_PROBESTATSLOG_H_

#include <unistd.h>
#include <locale.h>
#include <time.h>
#include <sys/time.h>

#include "BaseConfig.h"
#include "SpectaTypedef.h"
#include "Log.h"
#include "ServerGlobal.h"
#include "SmGlobal.h"
#include "MapStores.h"

class ProbeStatsLog : public BaseConfig
{
	public:
		ProbeStatsLog();
		~ProbeStatsLog();
		void run();

	private:
		void printPktReceived();
		void printSMReport();
		void printfFlusherReport();
};

#endif /* CORE_SRC_PROBESTATSLOG_H_ */
