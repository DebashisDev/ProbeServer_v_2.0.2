/*
 * IPFlusher.h
 *
 *  Created on: Apr 24, 2017
 *      Author: Debashis
 */

#ifndef PLUGINS_TCP_SRC_IPFLUSHER_H_
#define PLUGINS_TCP_SRC_IPFLUSHER_H_

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

class TcpFlusher : BaseConfig
{
	private:
		uint16_t	instanceId;
		uint16_t 	lastIndex;
		uint16_t 	curIndex;
		uint16_t	curMin;
		uint16_t	prevMin;
		bool		repoInitStatus;
		uint32_t 	totalCnt;

		FUtility *pFlUtility;

		char 	tcpXdr[XDR_MAX_LEN];

		fstream	 xdrTcpHandler;

		char 	bwXdr[XDR_MAX_LEN];
		char	cdnXdr[XDR_MAX_LEN];
		char	uncXdr[XDR_MAX_LEN];
		fstream	BwXdrHandler;
		fstream	CDNXdrHandler;


		void 	processTcpData(uint16_t idx);
		void 	flushTcpData(uint32_t &flCnt, std::unordered_map<uint32_t, tcpSession> &pkt);
		bool 	createTcpXdrData(tcpSession pTcpSession);

		void 	openTcpXdrFile(uint16_t min, uint16_t hour, uint16_t day, uint16_t month, uint16_t year);
		void 	closeTcpXdrFile();

	public:
			TcpFlusher(uint16_t id);
			~TcpFlusher();
			bool 	isTcpFlusherReady();
			void 	run();
};
#endif /* PLUGINS_TCP_SRC_IPFLUSHER_H_ */
