/*
 * UdpFlusher.h
 *
 *  Created on: 18 Mar 2021
 *      Author: Debashis
 */

#ifndef PLUGINS_UDP_SRC_UDPFLUSHER_H_
#define PLUGINS_UDP_SRC_UDPFLUSHER_H_

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

class UdpFlusher : BaseConfig
{
	private:
		uint16_t		instanceId;
		uint16_t 		lastIndex;
		uint16_t 		curIndex;
		uint16_t		curMin;
		uint16_t		prevMin;
		bool			readyFlag;
		uint32_t 		totalCnt;

		FUtility *pFlUtility;

		char 	udpXdr[XDR_MAX_LEN];

		fstream	xdrUdpHandler;
//		string 	records;

		void 	processUdpData(uint16_t idx);
		void 	flushUdpData(uint32_t &flCnt, std::unordered_map<uint32_t, udpSession> &pkt);
		bool 	createUdpXdrData(udpSession pUdpSession);

		void 	openUdpXdrFile(uint16_t min, uint16_t hour, uint16_t day, uint16_t month, uint16_t year);
		void 	closeUdpXdrFile();

	public:
			UdpFlusher(uint16_t id);
			~UdpFlusher();
			bool 	isUdpFlusherReady();
			void 	run();
};
#endif /* PLUGINS_UDP_SRC_UDPFLUSHER_H_ */
