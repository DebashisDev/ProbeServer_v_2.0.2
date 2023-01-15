/*
 * DNSFlusher.h
 *
 *  Created on: 17-Sep-2019
 *      Author: Debashis
 */

#ifndef PLUGINS_TCP_INC_DNSFLUSHER_H_
#define PLUGINS_TCP_INC_DNSFLUSHER_H_

#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <iterator>
#include <list>

#include "FUtility.h"
#include "PUtility.h"
#include "SmGlobal.h"
#include "MapStores.h"
#include "Log.h"
#include "BaseConfig.h"

class DnsFlusher : BaseConfig
{
	private:
		bool			readyFlag;
		uint16_t		instanceId;
		uint16_t 		lastIndex;
		uint16_t 		curIndex;
		uint16_t		curMin;
		uint16_t		prevMin;
		uint32_t 		totalCnt;

		char 		dnsXdr[XDR_MAX_LEN];

		FUtility *pFlUtility;
		fstream	xdrDnsHandler;
//		string 	records;


		void 	processDnsData(uint16_t idx);
		void 	flushDnsData(uint32_t &flCnt, std::unordered_map<uint32_t, dnsSession> &pkt);
		bool 	createDnsXdrData(dnsSession pDnsSession);

		void 	openDnsXdrFile(uint16_t min, uint16_t hour, uint16_t day, uint16_t month, uint16_t year);
		void 	closeDnsXdrFile();

	public:
		DnsFlusher(uint16_t id);
		~DnsFlusher();
		bool 	isDnsFlusherReady();
		void 	run();
};

#endif /* PLUGINS_TCP_INC_DNSFLUSHER_H_ */
