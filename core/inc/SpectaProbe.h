/*
 * SpectaProbe.h
 *
 *  Created on: 29-Jan-2016
 *      Author: Debashis
 */

#ifndef SRC_SPECTAPROBE_H_
#define SRC_SPECTAPROBE_H_


#include <unistd.h>
#include <locale.h>
#include <signal.h>
#include <string.h>
#include <string>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

#include "TcpSM.h"
#include "UdpSM.h"
#include "DnsSM.h"
#include "UnSM.h"
#include "AaaSM.h"

#include "TcpFlusher.h"
#include "UdpFlusher.h"
#include "DnsFlusher.h"
#include "UnFlusher.h"
#include "AaaFlusher.h"

#include "GConfig.h"
#include "Log.h"
#include "BaseConfig.h"
#include "ProbeStats.h"
#include "ProbeStatsLog.h"
#include "SpectaTypedef.h"
#include "Routers.h"
#include "ReadTcpAgent.h"
#include "ReadUdpAgent.h"
#include "ReadAaaAgent.h"
#include "ReadDnsAgent.h"

#include "Initialize.h"
#include "SmGlobal.h"
#include "Timer.h"


class SpectaProbe : public BaseConfig
{
	private:
		timeval curTime;
		struct tm *now_tm;

		void 	pinThread(pthread_t th,int i);
		void 	initializeLocks();
		void 	initializeLog();

		void 	initializeReadAgentRepo();
		void 	initialize_sm_maps();
		void 	initialize_sm_flusher();
		void 	initialize_xdr_flusher();


		void 	spawnTimer(uint16_t no);
		void	spawnRoutersPerInterface(uint16_t no);

		void	spawnTcpReadAgent(float no);
		void 	spawnTcpSessionManager(float no);
		void 	spawnTcpFlusher(float no);

		void	spawnUdpReadAgent(float no);
		void 	spawnUdpSessionManager(float no);
		void 	spawnUdpFlusher(float no);

		void	spawnDnsReadAgent(float no);
		void 	spawnDnsSessionManager(float no);
		void 	spawnDnsFlusher(float no);

		void	spawnAaaReadAgent(float no);
		void 	spawnAaaSessionManager(float no);
		void 	spawnAaaFlusher(float no);

		void 	spawnUnMappedSessionManager(float no);
		void 	spawnUnMappedFlusher(float no);

		void	spawnWriteStats(uint16_t no);
		void	spawnWriteLog(uint16_t no);

		Timer*				pGlbTimer;
		pthread_t			glbTimerThrId;

		ReadTcpAgent*		pReadTcpAgent[MAX_INTERFACE_SUPPORT];
		pthread_t			pReadTcpThrId[MAX_INTERFACE_SUPPORT];

		TcpSM*				pTcpSM[MAX_TCP_SM_SUPPORT];
		pthread_t			pTcpSMThr[MAX_TCP_SM_SUPPORT];

		TcpFlusher*			pTcpFlusher;
		pthread_t 			pTcpFlusherThr;

		ReadUdpAgent*		pReadUdpAgent[MAX_INTERFACE_SUPPORT];
		pthread_t			pReadUdpThrId[MAX_INTERFACE_SUPPORT];

		UdpSM*				pUdpSM[MAX_UDP_SM_SUPPORT];
		pthread_t			pUdpSMThr[MAX_UDP_SM_SUPPORT];

		UdpFlusher*			pUdpFlusher;
		pthread_t 			pUdpFlusherThr;

		ReadDnsAgent*		pReadDnsAgent[MAX_INTERFACE_SUPPORT];
		pthread_t			pReadDnsThrId[MAX_INTERFACE_SUPPORT];

		DnsSM*				pDnsSM[MAX_DNS_SM_SUPPORT];
		pthread_t			pDnsSMThr[MAX_DNS_SM_SUPPORT];

		DnsFlusher*			pDnsFlusher;
		pthread_t 			pDnsFlusherThr;

		ReadAaaAgent*		pReadAaaAgent[MAX_INTERFACE_SUPPORT];
		pthread_t			pReadAaaThrId[MAX_INTERFACE_SUPPORT];

		AaaSM*				pAaaSM[MAX_AAA_SM_SUPPORT];
		pthread_t			pAaaSMThr[MAX_AAA_SM_SUPPORT];

		AaaFlusher*			pAaaFlusher;
		pthread_t 			pAaaFlusherThr;

		UnSM*				pUnSM[MAX_UNM_SM_SUPPORT];
		pthread_t			pUnSMThr[MAX_UNM_SM_SUPPORT];

		UnFlusher*			pUnFlusher;
		pthread_t 			pUnFlusherThr;

		ProbeStats*			ps;
		pthread_t 			psThread ;

		ProbeStatsLog*		psLog;
		pthread_t 			psLogThread ;

		Routers*			pRouters[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE];
		pthread_t 			pRoutersThr[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE] ;

		Initialize*			pInit;
		GConfig*			pGConfig;

	public:
		SpectaProbe(char *fileName);
		~SpectaProbe();

		void start();
};

#endif /* SRC_SPECTAPROBE_H_ */
