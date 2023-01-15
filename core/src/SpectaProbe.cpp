/*
 * SpectaProbe.cpp
 *
 *  Created on: 29-Jan-2016
 *      Author: Debashis
 */

#include "SpectaProbe.h"

/*
 * TCP Thread Functions
 */

void *tcpAgentThread(void *arg)
{
	ReadTcpAgent *ft = (ReadTcpAgent*)arg;
	ft->run();
	return NULL;
}

void *tcpSMThread(void *arg)
{
	TcpSM *ft= (TcpSM*)arg;
	ft->run() ;
	return NULL;
}

void* tcpFlusherThread(void* arg)
{
	TcpFlusher *ft = (TcpFlusher*)arg;
	ft->run();
	return NULL;
}

/*
 * UDP Thread Functions
 */

void *udpAgentThread(void *arg)
{
	ReadUdpAgent *ft = (ReadUdpAgent*)arg;
	ft->run();
	return NULL;
}

void *udpSMThread(void *arg)
{
	UdpSM *ft= (UdpSM*)arg;
	ft->run() ;
	return NULL;
}

void* udpFlusherThread(void* arg)
{
	UdpFlusher *ft = (UdpFlusher*)arg;
	ft->run();
	return NULL;
}

/*
 * DNS Thread Functions
 */

void *dnsAgentThread(void *arg)
{
	ReadDnsAgent *ft = (ReadDnsAgent*)arg;
	ft->run();
	return NULL;
}

void *dnsSMThread(void *arg)
{
	DnsSM *ft= (DnsSM*)arg;
	ft->run() ;
	return NULL;
}

void* dnsFlusherThread(void* arg)
{
	DnsFlusher *ft = (DnsFlusher*)arg;
	ft->run();
	return NULL;
}

/*
 * AAA Thread Functions
 */

void *startReadAaaAgentThread(void *arg)
{
	ReadAaaAgent *ft = (ReadAaaAgent*)arg;
	ft->run();
	return NULL;
}

void *aaaSMThread(void *arg)
{
	AaaSM *ft= (AaaSM*)arg;
	ft->run() ;
	return NULL;
}

void* aaaFlusherThread(void* arg)
{
	AaaFlusher *ft = (AaaFlusher*)arg;
	ft->run();
	return NULL;
}

void *unSMThread(void *arg)
{
	UnSM *ft= (UnSM*)arg;
	ft->run() ;
	return NULL;
}

void* unFlusherThread(void* arg)
{
	UnFlusher *ft = (UnFlusher*)arg;
	ft->run();
	return NULL;
}

/*
 * Common Thread Functions
 */

void *startTimerThread(void *arg)
{
	Timer *ft = (Timer *)arg;
	ft->run();
	return NULL;
}

void *startRoutersThread(void *arg)
{
	Routers *ft= (Routers*)arg;
	ft->run() ;
	return NULL;
}

void *startStatsThread(void *arg)
{
	ProbeStats *ft = (ProbeStats*)arg;
	ft->run();
	return NULL;
}

void *startLogThread(void *arg)
{
	ProbeStatsLog *ft = (ProbeStatsLog*)arg;
	ft->run();
	return NULL;
}

SpectaProbe::SpectaProbe(char *fileName)
{
	this->_name = "SpectaProbe";

	pInit = new Initialize();
	pGConfig = new GConfig(fileName);

	initializeLocks();
	this->setLogLevel(Log::theLog().level());

}

SpectaProbe::~SpectaProbe()
{
	delete(pGConfig);
	delete(pInit);
}

void SpectaProbe::initializeLocks()
{
	mapDnsLock::count 		= 1;
	mapAAALock::count		= 1;
}

void SpectaProbe::initializeLog()
{
	char logFile[200];
//	sprintf(logFile, "%s%s_%02d-%02d-%02d-%d.log", SMGlobal::LOG_DIR.c_str(), "server", SMGlobal::PROBE_ID, Global::CURRENT_DAY, Global::CURRENT_MONTH, Global::CURRENT_YEAR);
	sprintf(logFile, "%s%s_%02d.log", SMGlobal::LOG_DIR.c_str(), "server", SMGlobal::PROBE_ID);

	Log::theLog().open(logFile);
	Log::theLog().level(SMGlobal::LOG_LEVEL);

	char *probeVer = getenv("PROBE_VER");

	printf(" ############################################################\n");
	printf("                                                             \n");
	printf("              Starting SPECTA Server Ver : %s            \n", probeVer);
	printf("                                                             \n");
	printf(" ############################################################\n");


	TheLog_nc_v1(Log::Info, name(),"  ############################################################%s","");
	TheLog_nc_v1(Log::Info, name(),"                                                              %s","");
	TheLog_nc_v1(Log::Info, name(),"                     Starting SPECTA Server Ver : %s        ", probeVer);
	TheLog_nc_v1(Log::Info, name(),"                                                              %s","");
	TheLog_nc_v1(Log::Info, name(),"  ############################################################%s","");
	TheLog_nc_v1(Log::Info, name(),"  Log file initialized Level - %d", SMGlobal::LOG_LEVEL);
}

void SpectaProbe::start()
{
	initializeReadAgentRepo();
	sleep(5);
	initialize_sm_maps();

	/* [1] ---- Start Timer thread, which will broadcast epoch sec after every 1/2 a second ---- */
	spawnTimer(1);
	sleep(1);

	initializeLog();

	/* Start Routers ---- */
	sleep(1);
	printf("*** Going to Start Routers ... \n");
	TheLog_nc_v1(Log::Info, name(),"*** Going to Start Routers ... %s","");
	spawnRoutersPerInterface(3);

	/* Start Session Managers */
	sleep(1);
	printf("*** Going to Start Session Managers ... \n");
	TheLog_nc_v1(Log::Info, name(),"*** Going to Start Session Managers ... %s","");
	spawnTcpSessionManager(4.1);
	spawnUdpSessionManager(4.2);
	spawnDnsSessionManager(4.3);
	spawnAaaSessionManager(4.4);
	spawnUnMappedSessionManager(4.5);

	/* Start Flushers */
	sleep(1);
	printf("*** Going to Start Flushers ... \n");
	spawnTcpFlusher(5.1);
	spawnUdpFlusher(5.2);
	spawnDnsFlusher(5.3);
	spawnAaaFlusher(5.4);
	spawnUnMappedFlusher(5.5);

	/* Start Agents */
	sleep(1);
	printf("*** Going to Start Agents ... \n");
	TheLog_nc_v1(Log::Info, name(),"*** Going to Start Agents ... %s","");
	spawnTcpReadAgent(2.0);
	spawnUdpReadAgent(2.1);
	spawnAaaReadAgent(2.2);
	spawnDnsReadAgent(2.3);

	/* Start Stats */
	sleep(1);
	spawnWriteStats(6);

	/* Start Probe Log */
	sleep(1);
	spawnWriteLog(7);

	sleep(5);

	pInit->load1ResolvedIpv4();		/* Load DNS Data */
	pInit->load2ResolvedIpv4();		/* Load DNS Data */
	pInit->loadAAAData();			/* Load AAA Data */

	uint32_t dnsLoadLoop = 0, aaaLoadLoop = 0, aaaDumpLoop =0;
	char logFile[200];
	uint16_t today = 0, lastday = 0;
	lastday = today =  Global::CURRENT_DAY;

	while(SMGlobal::SERVER_RUNNING_STATUS)
	{
		uint16_t intf = 0;

		sleep(1);

		today = Global::CURRENT_DAY;

		if(lastday != today)
		{
			lastday = today;
			TheLog_nc_v1(Log::Info, name(),"  Day Changed .... !!! Initializing Counters....", "");

			for(intf = 0; intf < SMGlobal::NO_OF_INTERFACE_SUPPORT; intf++)
			{
				SMGlobal::TCP_PACKET_PER_DAY[intf] = 0;
				SMGlobal::UDP_PACKET_PER_DAY[intf] = 0;
				SMGlobal::AAA_PACKET_PER_DAY[intf] = 0;
			}

			for(uint16_t sm = 0; sm < SMGlobal::TCP_SM_INSTANCES; sm++)
				SMGlobal::TCP_SM_PKT_RECEIVED[sm] = 0;

			for(uint16_t sm = 0; sm < SMGlobal::UDP_SM_INSTANCES; sm++)
				SMGlobal::UDP_SM_PKT_RECEIVED[sm] = 0;

			for(uint16_t sm = 0; sm < SMGlobal::DNS_SM_INSTANCES; sm++)
				SMGlobal::DNS_SM_PKT_RECEIVED[sm] = 0;

			for(uint16_t sm = 0; sm < SMGlobal::AAA_SM_INSTANCES; sm++)
				SMGlobal::AAA_SM_PKT_RECEIVED[sm] = 0;

			for(uint16_t sm = 0; sm < SMGlobal::UNM_SM_INSTANCES; sm++)
				SMGlobal::UNM_SM_PKT_RECEIVED[sm] = 0;

//			TheLog_nc_v1(Log::Info, name(),"  Closing the Log file and creating new log file.", "");

//			Log::theLog().close();
//			sprintf(logFile, "%s%s_%02d-%02d-%02d-%d.log", SMGlobal::LOG_DIR.c_str(), "server", SMGlobal::PROBE_ID, Global::CURRENT_DAY, Global::CURRENT_MONTH, Global::CURRENT_YEAR);

//			Log::theLog().open(logFile);
//			Log::theLog().level(SMGlobal::LOG_LEVEL);

			if (SMGlobal::INSTANCE_ID == 1)
			{
				 pInit->clearAAAData();
				 pInit->loadAAAData();		/* Load AAA Data */
			}
		}

		/*
		 * Load DNS Resolved IP in Minutes
		 */

		if(dnsLoadLoop >= SMGlobal::DNS_LOAD_FREQ_SEC) {
			dnsLoadLoop = 0;
			pInit->load1ResolvedIpv4();
			pInit->load2ResolvedIpv4();
		}
		else
			dnsLoadLoop ++;

		switch(SMGlobal::INSTANCE_ID)
		{
			case 1:
				if(aaaDumpLoop >= SMGlobal::AAA_DUMP_FREQ_SEC) {
					aaaDumpLoop = 0;
					pInit->dumpAAAData();		/* Dump AAA Data */
				}
				else
					aaaDumpLoop ++;

				break;

			default:
				if(aaaLoadLoop >= SMGlobal::AAA_LOAD_FREQ_SEC) {
					aaaLoadLoop = 0;
					pInit->loadAAAData();		/* Load AAA Data */
				}
				else
					aaaLoadLoop ++;

				break;
		}
	}

	printf("  ** Shutdown Server Complete **\n");
	TheLog_nc_v1(Log::Info, name(),"  ** Shutdown Server Complete ** %s","");
	exit(0);
}

void SpectaProbe::spawnTimer(uint16_t no)
{
	pGlbTimer = new Timer;

	SMGlobal::TIMER_RUNNING_STATUS = true;

	pthread_create(&glbTimerThrId, NULL, startTimerThread, pGlbTimer);
	pinThread(glbTimerThrId, SMGlobal::TIMER_THREAD_CORE);

	while(!pGlbTimer->isGlbTimerInitialized())
		sleep(1);

	printf("  *** [%02d] Timer Thread Started Successfully. Pinned to CPU [%02d]\n", no, SMGlobal::TIMER_THREAD_CORE);
	TheLog_nc_v2(Log::Info, name(),"  *** [%02d] Timer Thread Started Successfully. Pinned to CPU [%02d]", no, SMGlobal::TIMER_THREAD_CORE);
}

/* Router Start */
void SpectaProbe::spawnRoutersPerInterface(uint16_t no)
{
	for(uint16_t interfaceId = 0; interfaceId < SMGlobal::NO_OF_INTERFACE_SUPPORT; interfaceId ++)
	{
		for(uint16_t routerId = 0; routerId < SMGlobal::ROUTER_PER_INTERFACE[interfaceId]; routerId ++)
		{
			pRouters[interfaceId][routerId] = new Routers(interfaceId, routerId);
			SMGlobal::ROUTER_RUNNING_STATUS[interfaceId][routerId] = true;

			pthread_create(&pRoutersThr[interfaceId][routerId] , NULL, startRoutersThread, pRouters[interfaceId][routerId]);
			pinThread(pRoutersThr[interfaceId][routerId], SMGlobal::ROUTER_CORE[interfaceId][routerId]);

			printf("  *** [%02d] Interface: %02d| Router: %02d| Core: %02d Success\n", no, interfaceId, routerId, SMGlobal::ROUTER_CORE[interfaceId][routerId]);
			TheLog_nc_v4(Log::Info, name(),"  *** [%02d] Interface: %02d| Router: %02d| Core: %02d Success", no, interfaceId, routerId, SMGlobal::ROUTER_CORE[interfaceId][routerId]);

			while(!pRouters[interfaceId][routerId]->isSMRouterInitialized())
				sleep(1);
		}
	}
	printf("\n");
}

/* Agent Start */
void SpectaProbe::spawnTcpReadAgent(float no)
{
	for(uint16_t connectionCounter = 0; connectionCounter < SMGlobal::NO_OF_INTERFACE_SUPPORT; connectionCounter ++)
	{
		pReadTcpAgent[connectionCounter] = new ReadTcpAgent(connectionCounter);
		SMGlobal::TCP_AGENT_RUNNING_STATUS[connectionCounter] = true;

		pthread_create(&pReadTcpThrId[connectionCounter], NULL, tcpAgentThread, pReadTcpAgent[connectionCounter]);
		pinThread(pReadTcpThrId[connectionCounter], SMGlobal::TCP_AGENT_CORE[connectionCounter]);

		printf("  *** [%.01f] Tcp Agent Interface: %02d| Core: %02d\n", no, connectionCounter, SMGlobal::TCP_AGENT_CORE[connectionCounter]);
		TheLog_nc_v3(Log::Info, name(),"  *** [%.01f] Tcp Agent Interface: %02d| Core: %02d", no, connectionCounter, SMGlobal::TCP_AGENT_CORE[connectionCounter]);

		while(!pReadTcpAgent[connectionCounter]->isTcpAgentReady())
			sleep(1);

	}
	printf("\n");
}

void SpectaProbe::spawnUdpReadAgent(float no)
{
	for(uint16_t interfaceId = 0; interfaceId < SMGlobal::NO_OF_INTERFACE_SUPPORT; interfaceId ++)
	{
		pReadUdpAgent[interfaceId] = new ReadUdpAgent(interfaceId);
		SMGlobal::UDP_AGENT_RUNNING_STATUS[interfaceId] = true;

		pthread_create(&pReadUdpThrId[interfaceId], NULL, udpAgentThread, pReadUdpAgent[interfaceId]);
		pinThread(pReadUdpThrId[interfaceId], SMGlobal::UDP_AGENT_CORE[interfaceId]);

		printf("  *** [%.01f] Udp Agent Interface: %02d| Core: %02d\n", no, interfaceId, SMGlobal::UDP_AGENT_CORE[interfaceId]);
		TheLog_nc_v3(Log::Info, name(),"  *** [%.01f] Udp Agent Interface: %02d| Core: %02d\n", no, interfaceId, SMGlobal::UDP_AGENT_CORE[interfaceId]);

		while(!pReadUdpAgent[interfaceId]->isUdpAgentReady())
			sleep(1);
	}
	printf("\n");
}

void SpectaProbe::spawnDnsReadAgent(float no)
{
	for(uint16_t interfaceId = 0; interfaceId < SMGlobal::NO_OF_INTERFACE_SUPPORT; interfaceId ++)
	{
//		sleep(1);
		pReadDnsAgent[interfaceId] = new ReadDnsAgent(interfaceId);
		SMGlobal::DNS_AGENT_RUNNING_STATUS[interfaceId] = true;

		pthread_create(&pReadDnsThrId[interfaceId], NULL, dnsAgentThread, pReadDnsAgent[interfaceId]);
		pinThread(pReadDnsThrId[interfaceId], SMGlobal::DNS_AGENT_CORE[interfaceId]);

		printf("  *** [%.01f] Dns Agent Interface: %02d| Core: %02d\n", no, interfaceId, SMGlobal::DNS_AGENT_CORE[interfaceId]);
		TheLog_nc_v3(Log::Info, name(),"  *** [%.01f] Dns Agent Interface: %02d| Core: %02d\n", no, interfaceId, SMGlobal::DNS_AGENT_CORE[interfaceId]);

		while(!pReadDnsAgent[interfaceId]->isDnsAgentReady())
			sleep(1);
	}
	printf("\n");
}

void SpectaProbe::spawnAaaReadAgent(float no)
{
	ReadAaaAgent*		pReadAaaAgent[MAX_INTERFACE_SUPPORT];
	pthread_t			pReadAaaThrId[MAX_INTERFACE_SUPPORT];

	for(uint16_t interfaceId = 0; interfaceId < SMGlobal::NO_OF_INTERFACE_SUPPORT; interfaceId ++)
	{
//		sleep(1);
		pReadAaaAgent[interfaceId] = new ReadAaaAgent(interfaceId);
		SMGlobal::AAA_AGENT_RUNNING_STATUS[interfaceId] = true;

		pthread_create(&pReadAaaThrId[interfaceId], NULL, startReadAaaAgentThread, pReadAaaAgent[interfaceId]);
		pinThread(pReadAaaThrId[interfaceId], SMGlobal::AAA_AGENT_CORE[interfaceId]);

		printf("  *** [%.01f] Aaa Agent Interface: %02d| Core: %02d\n", no, interfaceId, SMGlobal::AAA_AGENT_CORE[interfaceId]);
		TheLog_nc_v3(Log::Info, name(),"  *** [%.01f] Aaa Agent Interface: %02d| Core: %02d", no, interfaceId, SMGlobal::AAA_AGENT_CORE[interfaceId]);

		while(!pReadAaaAgent[interfaceId]->isAaaAgentReady())
			sleep(1);
	}
	printf("\n");
}

/* Start Session managers */
void SpectaProbe::spawnTcpSessionManager(float no)
{
	for (uint16_t sessionCount = 0; sessionCount < SMGlobal::TCP_SM_INSTANCES; sessionCount ++)
	{
//		sleep(1);
		pTcpSM[sessionCount] = new TcpSM(sessionCount);
		SMGlobal::TCP_SM_RUNNING_STATUS[sessionCount] = true;

		pthread_create(&pTcpSMThr[sessionCount], NULL, tcpSMThread, pTcpSM[sessionCount]);
		pinThread(pTcpSMThr[sessionCount], SMGlobal::TCP_SM_CORE[sessionCount]);

		printf("  *** [%.01f] Tcp SM Instance: %02d| Core: %02d Success.\n", no, sessionCount, SMGlobal::TCP_SM_CORE[sessionCount]);
		TheLog_nc_v3(Log::Info, name(),"  *** [%.01f] Tcp SM Instance: %02d| Core: %02d Success.", no, sessionCount, SMGlobal::TCP_SM_CORE[sessionCount]);

		while(!pTcpSM[sessionCount]->isTcpSMReady())
			sleep(1);
	}
	printf("\n");
}

void SpectaProbe::spawnUdpSessionManager(float no)
{
	for (uint16_t sessionCount = 0; sessionCount < SMGlobal::UDP_SM_INSTANCES; sessionCount ++)
	{
//		sleep(1);
		pUdpSM[sessionCount] = new UdpSM(sessionCount);
		SMGlobal::UDP_SM_RUNNING_STATUS[sessionCount] = true;

		pthread_create(&pUdpSMThr[sessionCount] , NULL , udpSMThread , pUdpSM[sessionCount]);
		pinThread(pUdpSMThr[sessionCount] , SMGlobal::UDP_SM_CORE[sessionCount] );

		printf("  *** [%.01f] Udp SM Instance: %02d| Core: %02d Success.\n", no, sessionCount, SMGlobal::UDP_SM_CORE[sessionCount]);
		TheLog_nc_v3(Log::Info, name(),"  *** [%.01f] Udp SM Instance: %02d| Core: %02d Success.", no, sessionCount, SMGlobal::UDP_SM_CORE[sessionCount]);

		while(!pUdpSM[sessionCount]->isUdpSMReady())
			sleep(1);
	}
	printf("\n");
}

void SpectaProbe::spawnDnsSessionManager(float no)
{
	for (uint16_t sessionCount = 0; sessionCount < SMGlobal::DNS_SM_INSTANCES; sessionCount ++)
	{
//		sleep(1);
		pDnsSM[sessionCount] = new DnsSM(sessionCount);
		SMGlobal::DNS_SM_RUNNING_STATUS[sessionCount] = true;

		pthread_create(&pDnsSMThr[sessionCount] , NULL , dnsSMThread , pDnsSM[sessionCount]);
		pinThread(pDnsSMThr[sessionCount] , SMGlobal::DNS_SM_CORE[sessionCount] );

		printf("  *** [%.01f] Dns SM Instance: %02d| Core: %02d\n", no, sessionCount, SMGlobal::DNS_SM_CORE[sessionCount]);
		TheLog_nc_v3(Log::Info, name(),"  *** [%.01f] Dns SM Instance: %02d| Core: %02d", no, sessionCount, SMGlobal::DNS_SM_CORE[sessionCount]);

		while(!pDnsSM[sessionCount]->isInitialized())
			sleep(1);
	}
	printf("\n");
}

void SpectaProbe::spawnAaaSessionManager(float no)
{
	for (uint16_t sessionCount = 0; sessionCount < SMGlobal::AAA_SM_INSTANCES; sessionCount ++)
	{
//		sleep(1);
		pAaaSM[sessionCount] = new AaaSM(sessionCount);
		SMGlobal::AAA_SM_RUNNING_STATUS[sessionCount] = true;

		pthread_create(&pAaaSMThr[sessionCount] , NULL , aaaSMThread , pAaaSM[sessionCount]);
		pinThread(pAaaSMThr[sessionCount] , SMGlobal::AAA_SM_CORE[sessionCount] );

		printf("  *** [%.01f] Aaa SM Instance: %02d| Core: %02d\n", no, sessionCount, SMGlobal::AAA_SM_CORE[sessionCount]);
		TheLog_nc_v3(Log::Info, name(),"  *** [%.01f] Aaa SM Instance: %02d| Core: %02d", no, sessionCount, SMGlobal::AAA_SM_CORE[sessionCount]);

		while(!pAaaSM[sessionCount]->isAaaSMReady())
			sleep(1);
	}
	printf("\n");
}

void SpectaProbe::spawnUnMappedSessionManager(float no)
{
	for (uint16_t sessionCount = 0; sessionCount < SMGlobal::UNM_SM_INSTANCES; sessionCount ++)
	{
//		sleep(1);
		pUnSM[sessionCount] = new UnSM(sessionCount);
		SMGlobal::UNM_SM_RUNNING_STATUS[sessionCount] = true;

		pthread_create(&pUnSMThr[sessionCount] , NULL , unSMThread , pUnSM[sessionCount]);
		pinThread(pUnSMThr[sessionCount] , SMGlobal::UNM_SM_CORE[sessionCount] );

		printf("  *** [%.01f] Unm SM Instance: %02d| Core: %02d\n", no, sessionCount, SMGlobal::UNM_SM_CORE[sessionCount]);
		TheLog_nc_v3(Log::Info, name(),"  *** [%.01f] Unm SM Instance: %02d| Core: %02d", no, sessionCount, SMGlobal::UNM_SM_CORE[sessionCount]);

		while(!pUnSM[sessionCount]->isUnknownSMReady())
			sleep(1);
	}
	printf("\n");
}

/* Start Flusher */

void SpectaProbe::spawnTcpFlusher(float no)
{
//	sleep(1);
	for(uint16_t i = 0; i < SMGlobal::TCP_FLUSHER_INSTANCE; i++)
	{
		sleep(1);
		SMGlobal::TCP_FLUSHER_RUNNING_STATUS[i] = true;
		pTcpFlusher = new TcpFlusher(i);
		pthread_create(&pTcpFlusherThr, NULL, tcpFlusherThread, pTcpFlusher);
		pinThread(pTcpFlusherThr, SMGlobal::TCP_FLUSHER_CORE[i]);

		printf("  *** [%.01f] Tcp Flusher Instance: %02d| Core: %2d\n", no, i, SMGlobal::TCP_FLUSHER_CORE[i]);
		TheLog_nc_v3(Log::Info, name(),"  *** [%.01f] Tcp Flusher Instance: %02d| Core: %2d", no, i, SMGlobal::TCP_FLUSHER_CORE[i]);

		while(!pTcpFlusher->isTcpFlusherReady())
			sleep(1);
	}
	printf("\n");
}

void SpectaProbe::spawnUdpFlusher(float no)
{
//	sleep(1);
	for(uint16_t i = 0; i < SMGlobal::UDP_FLUSHER_INSTANCE; i++)
	{
		SMGlobal::UDP_FLUSHER_RUNNING_STATUS[i] = true;
		pUdpFlusher = new UdpFlusher(i);
		pthread_create(&pUdpFlusherThr, NULL, udpFlusherThread, pUdpFlusher);
		pinThread(pUdpFlusherThr, SMGlobal::UDP_FLUSHER_CORE[i]);

		printf("  *** [%.01f] Udp Flusher Instance: %02d| Core: %2d\n", no, i, SMGlobal::UDP_FLUSHER_CORE[i]);
		TheLog_nc_v3(Log::Info, name(),"  *** [%.01f] Udp Flusher Instance: %02d| Core: %2d", no, i, SMGlobal::UDP_FLUSHER_CORE[i]);

		while(!pUdpFlusher->isUdpFlusherReady())
			sleep(1);
	}
	printf("\n");
}

void SpectaProbe::spawnDnsFlusher(float no)
{
//	sleep(1);
	for(uint16_t i = 0; i < SMGlobal::DNS_FLUSHER_INSTANCE; i++)
	{
		SMGlobal::DNS_FLUSHER_RUNNING_STATUS[i] = true;
		pDnsFlusher = new DnsFlusher(i);
		pthread_create(&pDnsFlusherThr, NULL, dnsFlusherThread, pDnsFlusher);
		pinThread(pDnsFlusherThr, SMGlobal::DNS_FLUSHER_CORE[i]);

		printf("  *** [%.01f] Dns Flusher Instance: %02d| Core: %2d\n", no, i, SMGlobal::DNS_FLUSHER_CORE[i]);
		TheLog_nc_v3(Log::Info, name(),"  *** [%.01f] Dns Flusher Instance: %02d| Core: %2d", no, i, SMGlobal::DNS_FLUSHER_CORE[i]);

		while(!pDnsFlusher->isDnsFlusherReady())
			sleep(1);
	}
	printf("\n");
}

void SpectaProbe::spawnAaaFlusher(float no)
{
	for(uint16_t i = 0; i < SMGlobal::AAA_FLUSHER_INSTANCE; i++)
	{
//		sleep(1);
		SMGlobal::AAA_FLUSHER_RUNNING_STATUS[i] = true;
		pAaaFlusher = new AaaFlusher(i);
		pthread_create(&pAaaFlusherThr, NULL, aaaFlusherThread, pAaaFlusher);
		pinThread(pAaaFlusherThr, SMGlobal::AAA_FLUSHER_CORE[i]);

		printf("  *** [%.01f] Aaa Flusher Instance: %02d| Core: %2d\n", no, i, SMGlobal::AAA_FLUSHER_CORE[i]);
		TheLog_nc_v3(Log::Info, name(),"  *** [%.01f] Aaa Flusher Instance: %02d| Core: %2d", no, i, SMGlobal::AAA_FLUSHER_CORE[i]);

		while(!pAaaFlusher->isAaaFlusherReady())
			sleep(1);
	}
	printf("\n");
	return (void)0;
}

void SpectaProbe::spawnUnMappedFlusher(float no)
{
//	sleep(1);
	SMGlobal::UNM_FLUSHER_RUNNING_STATUS = true;
	pUnFlusher = new UnFlusher;
	pthread_create(&pUnFlusherThr, NULL, unFlusherThread, pUnFlusher);
	pinThread(pUnFlusherThr, SMGlobal::UNM_FLUSHER_CORE);

	printf("  *** [%.01f] Unm Flusher Instance: %02d| Core: %02d\n", no, 0, SMGlobal::UNM_FLUSHER_CORE);
	TheLog_nc_v3(Log::Info, name(),"  *** [%.01f] Unm Flusher Instance: %02d| Core: %02d", no, 0, SMGlobal::UNM_FLUSHER_CORE);

	while(!pUnFlusher->isUnFlusherReady())
		sleep(1);

	printf("\n");
}

/* Start Stats */
void SpectaProbe::spawnWriteStats(uint16_t no)
{
	if(SMGlobal::PRINT_STATS)
	{
		SMGlobal::STATSLOG_RUNNING_STATUS = true;
		ps = new ProbeStats();
		pthread_create(&psThread, NULL, startStatsThread, ps);
		pinThread(psThread, SMGlobal::UNM_FLUSHER_CORE);
	}
	printf("  *** [%02d] Started Statistic Thread. \n", no);
	TheLog_nc_v1(Log::Info, name(),"  *** [%02d] Started Statistic Thread. ", no);
	return (void)0;
}

/* Start Probe Log */
void SpectaProbe::spawnWriteLog(uint16_t no)
{
	if(SMGlobal::PRINT_STATS)
	{
		SMGlobal::LOG_RUNNING_STATUS = true;
		psLog = new ProbeStatsLog();
		pthread_create(&psLogThread, NULL, startLogThread, psLog);
		pinThread(psLogThread, SMGlobal::UNM_FLUSHER_CORE);
	}
	printf("  *** [%02d] Started Log Thread. \n", no);
	TheLog_nc_v1(Log::Info, name(),"  *** [%02d] Started Log Thread. ", no);
	return (void)0;
}

void SpectaProbe::pinThread(pthread_t th, int i)
{
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(i,&cpuset);

	int s = pthread_setaffinity_np(th, sizeof(cpu_set_t), &cpuset);
	if (s != 0)
		handle_error_en(s, "ERROR!!! pthread_setaffinity_np");

	/* Check the actual affinity mask assigned to the thread */
	s = pthread_getaffinity_np(th, sizeof(cpu_set_t), &cpuset);
	if (s != 0)
		handle_error_en(s, "ERROR!!! pthread_getaffinity_np");

	if (!CPU_ISSET(i, &cpuset))
	{
		printf("CPU pinning failed at core :: %d\n", i);
		TheLog_nc_v1(Log::Info, name(),"  CPU pinning failed at core :: %d",i);
	}
	return (void)0;
}

void SpectaProbe::initializeReadAgentRepo()
{
	for(uint16_t intf = 0; intf < SMGlobal::NO_OF_INTERFACE_SUPPORT; intf++)
	{
		uint32_t maxBucket = SMGlobal::TCP_PPS_PER_INTERFACE[intf]/SMGlobal::ROUTER_PER_INTERFACE[intf];

		printf(" TcpAgentStore for Interface [%02d] Bucket [%'d] Size per Router x 10 x Total [%02d] Router ... ", intf, maxBucket, SMGlobal::ROUTER_PER_INTERFACE[intf]);
		TheLog_nc_v3(Log::Info, name()," TcpAgentStore for Interface [%02d] Bucket [%'d] Size per Router x 10 x Total [%02d] Router ... ", intf, maxBucket, SMGlobal::ROUTER_PER_INTERFACE[intf]);

		for(uint16_t router = 0; router < SMGlobal::ROUTER_PER_INTERFACE[intf]; router++)
		{
			for(uint16_t ti = 0; ti < 10; ti++)
			{
				agentStore::tcpACnt[intf][router][ti] = 0;
				agentStore::tcpABusy[intf][router][ti] = false;

				for(uint32_t bucket = 0; bucket < maxBucket; bucket++)
					agentStore::tcpAStore[intf][router][ti][bucket] = new TcpPacket();
			}
		}
		printf("Completed.\n");
		TheLog_nc_v1(Log::Info, name(),"  %s ", "Completed.");

		maxBucket = SMGlobal::UDP_PPS_PER_INTERFACE[intf]/SMGlobal::ROUTER_PER_INTERFACE[intf];

		printf(" UdpAgentStore for Interface [%02d] Bucket [%'d] Size per Router x 10 x Total [%02d] Router ... ", intf, maxBucket, SMGlobal::ROUTER_PER_INTERFACE[intf]);
		TheLog_nc_v3(Log::Info, name()," UdpAgentStore for Interface [%02d] Bucket [%'d] Size per Router x 10 x Total [%02d] Router ... ", intf, maxBucket, SMGlobal::ROUTER_PER_INTERFACE[intf]);

		for(uint16_t router = 0; router < SMGlobal::ROUTER_PER_INTERFACE[intf]; router++)
		{
			for(uint16_t ti = 0; ti < 10; ti++)
			{
				agentStore::udpACnt[intf][router][ti] = 0;
				agentStore::udpABusy[intf][router][ti] = false;

				for(uint32_t bucket = 0; bucket < maxBucket; bucket++)
					agentStore::udpAStore[intf][router][ti][bucket] = new UdpPacket();
			}
		}
		printf("Completed.\n");
		TheLog_nc_v1(Log::Info, name(),"  %s ", "Completed.");

		maxBucket = SMGlobal::DNS_PPS_PER_INTERFACE[intf]/SMGlobal::ROUTER_PER_INTERFACE[intf];

		printf(" DnsAgentStore for Interface [%02d] Bucket [%'d] Size per Router x 10 x Total [%02d] Router ... ", intf, maxBucket, SMGlobal::ROUTER_PER_INTERFACE[intf]);
		TheLog_nc_v3(Log::Info, name()," DnsAgentStore for Interface [%02d] Bucket [%'d] Size per Router x 10 x Total [%02d] Router ... ", intf, maxBucket, SMGlobal::ROUTER_PER_INTERFACE[intf]);

		for(uint16_t router = 0; router < SMGlobal::ROUTER_PER_INTERFACE[intf]; router++)
		{
			for(uint16_t ti = 0; ti < 10; ti++)
			{
				agentStore::dnsACnt[intf][router][ti] = 0;
				agentStore::dnsABusy[intf][router][ti] = false;

				for(uint32_t bucket = 0; bucket < maxBucket; bucket++)
					agentStore::dnsAStore[intf][router][ti][bucket] = new DnsPacket();
			}
		}
		printf("Completed.\n");
		TheLog_nc_v1(Log::Info, name(),"  %s ", "Completed.");

		maxBucket = SMGlobal::AAA_PPS_PER_INTERFACE[intf]/SMGlobal::ROUTER_PER_INTERFACE[intf];

		printf(" AaaAgentStore for Interface [%02d] Bucket [%'d] Size per Router x 10 x Total [%02d] Router ... ", intf, maxBucket, SMGlobal::ROUTER_PER_INTERFACE[intf]);
		TheLog_nc_v3(Log::Info, name()," AaaAgentStore for Interface [%02d] Bucket [%'d] Size per Router x 10 x Total [%02d] Router ... ", intf, maxBucket, SMGlobal::ROUTER_PER_INTERFACE[intf]);

		for(uint16_t router = 0; router < SMGlobal::ROUTER_PER_INTERFACE[intf]; router++)
		{
			for(uint16_t ti = 0; ti < 10; ti++)
			{
				agentStore::aaaACnt[intf][router][ti] = 0;
				agentStore::aaaABusy[intf][router][ti] = false;

				for(uint32_t bucket = 0; bucket < maxBucket; bucket++)
					agentStore::aaaAStore[intf][router][ti][bucket] = new AAAPacket();
			}
		}
		printf("Completed.\n");
		TheLog_nc_v1(Log::Info, name(),"  %s ", "Completed.");
	}
	return (void)0;
}

void SpectaProbe::initialize_sm_maps()
{
	printf("  *** Initialized TCP SM Queue\n");
	TheLog_nc_v1(Log::Info, name(),"  *** Initialized TCP SM Queue %s", "");

	for(uint16_t sm = 0; sm < SMGlobal::TCP_SM_INSTANCES; sm++)
	{
		for(uint16_t intf = 0; intf < SMGlobal::NO_OF_INTERFACE_SUPPORT; intf++)
		{
			for(uint16_t router = 0; router < SMGlobal::ROUTER_PER_INTERFACE[intf]; router++)
			{
				for(uint16_t ti = 0; ti < 10; ti++)
				{
					smStore::tcpSmBusy[sm][intf][router][ti] = false;
					smStore::tcpSmCnt[sm][intf][router][ti]  = 0;
				}
			}
		}
	}

	printf("  *** Initialized UDP SM Queue\n");
	TheLog_nc_v1(Log::Info, name(),"  *** Initialized UDP SM Queue %s", "");

	for(uint16_t sm = 0; sm < SMGlobal::UDP_SM_INSTANCES; sm++)
	{
		for(uint16_t intf = 0; intf < SMGlobal::NO_OF_INTERFACE_SUPPORT; intf++)
		{
			for(uint16_t router = 0; router < SMGlobal::ROUTER_PER_INTERFACE[intf]; router++)
			{
				for(uint16_t ti = 0; ti < 10; ti++)
				{
					smStore::udpSmBusy[sm][intf][router][ti] = false;
					smStore::udpSmCnt[sm][intf][router][ti]  = 0;
				}
			}
		}
	}

	printf("  *** Initialized DNS SM Queue\n");
	TheLog_nc_v1(Log::Info, name(),"  *** Initialized DNS SM Queue %s", "");

	for(uint16_t sm = 0; sm < SMGlobal::DNS_SM_INSTANCES; sm++)
	{
		for(uint16_t intf = 0; intf < SMGlobal::NO_OF_INTERFACE_SUPPORT; intf++)
		{
			for(uint16_t router = 0; router < SMGlobal::ROUTER_PER_INTERFACE[intf]; router++)
			{
				for(uint16_t ti = 0; ti < 10; ti++)
				{
					smStore::dnsSmBusy[sm][intf][router][ti] = false;
					smStore::dnsSmCnt[sm][intf][router][ti]  = 0;
				}
			}
		}
	}

	printf("  *** Initialized AAA SM Queue\n");
	TheLog_nc_v1(Log::Info, name(),"  *** Initialized AAA SM Queue %s", "");

	for(uint16_t sm = 0; sm < SMGlobal::AAA_SM_INSTANCES; sm++)
	{
		for(uint16_t intf = 0; intf < SMGlobal::NO_OF_INTERFACE_SUPPORT; intf++)
		{
			for(uint16_t router = 0; router < SMGlobal::ROUTER_PER_INTERFACE[intf]; router++)
			{
				for(uint16_t ti = 0; ti < 10; ti++)
				{
					smStore::aaaSmBusy[sm][intf][router][ti] = false;
					smStore::aaaSmCnt[sm][intf][router][ti]  = 0;
				}
			}
		}
	}
}

