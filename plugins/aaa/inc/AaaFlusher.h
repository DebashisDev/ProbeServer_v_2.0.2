/*
 * RadiusFlusher.h
 *
 *  Created on: 17-Sep-2019
 *      Author: Debashis
 */

#ifndef PLUGINS_AAA_INC_AAAFLUSHER_H_
#define PLUGINS_AAA_INC_AAAFLUSHER_H_

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

class AaaFlusher : BaseConfig
{
	private:
		bool			flusherReadyFlag;
		uint16_t 		instance;
		uint16_t 		lastIndex;
		uint16_t 		curIndex;
		uint16_t		curMin;
		uint16_t		prevMin;
		uint32_t 		totalCnt;

		char 			aaaXdr[XDR_MAX_LEN];
		bool			errorXdr;

		FUtility*	pFlUtility;
		fstream			aaaXdrHandler;
		fstream			aaaErrorXdrHandler;

		void 	processAaaData(uint16_t index);
		void 	flushAaaData(uint32_t &flCnt, std::unordered_map<uint32_t, aaaSession> &pkt);
		bool 	createAaaXdrData(aaaSession pRadiusSession);

		void 	openAaaXdrFile(uint16_t currentMin, uint16_t currentHour, uint16_t currentDay, uint16_t currentMonth, uint16_t currentYear);
		void 	closeAaaXdrFile();

	public:
		AaaFlusher(uint16_t id);
		~AaaFlusher();

		bool 	isAaaFlusherReady();
		void 	run();
};

#endif /* PLUGINS_AAA_INC_AAAFLUSHER_H_ */
