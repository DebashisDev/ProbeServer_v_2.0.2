/*
 * ProbeStats.cpp
 *
 *  Created on: Feb 1, 2017
 *      Author: Debashis
 */

#include "ProbeStats.h"

ProbeStats::ProbeStats()
{
	isStatsInitialized = false;
	this->_name = "ProbeStats";
	this->setLogLevel(Log::theLog().level());
}

ProbeStats::~ProbeStats()
{}

bool ProbeStats::isProbeStatInitialized()
{ return isStatsInitialized; }

void ProbeStats::run()
{
	struct tm *now_tm;
	char buffer[80];
	long startTime 	= 0, runTime = 0, currentHH = 0;
	int dd = 0, hh = 0, mm = 0, ss = 0, printloopCnt = 0;

	gettimeofday(&curTime, NULL);
	startTime = curTime.tv_sec;

	isStatsInitialized = true;

	while(SMGlobal::STATSLOG_RUNNING_STATUS)
	{
	  sleep(1);

	  if(SMGlobal::PRINT_STATS_FREQ_SEC > 0 && SMGlobal::PRINT_STATS)
	  {
	  	printloopCnt++;

	  	gettimeofday(&curTime, NULL);
	  	now_tm = localtime(&curTime.tv_sec);
	  	runTime = curTime.tv_sec - startTime;

	  	dd = (int)(runTime / 84600);
	  	hh = (int)((runTime - (dd * 84600)) / 3600);
	  	mm = (int)((runTime - ((dd * 84600) + (hh * 3600))) / 60);
	  	ss = (int)(runTime - ((dd * 84600) + (hh * 3600) + (mm * 60)));
	  	sprintf(buffer, "%02d:%02d:%02d",dd,hh,mm);

	    if(printloopCnt >= SMGlobal::PRINT_STATS_FREQ_SEC)
	  	{
	      printPktReceived();

	      printloopCnt = 0;

		  printf("\n      %s                                      T0       T1       T2       T3       T4       T5       T6       T7       T8       T9\n", buffer);

//		  printRouterCount(buffer);
		  printSessionStoreCount();

		  printIpXdrFlushStats();
		  printf("\n\n");
	  	}
	  }
	}
	printf("  ProbeStats Stopped...\n");
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void ProbeStats::printPktReceived()
{
	for (uint16_t interface = 0; interface < SMGlobal::NO_OF_INTERFACE_SUPPORT; interface++)
	{
		printf("  Interface [%02d] Pkt Count TCP|%12llu    UDP|%12llu    DNS|%12llu    AAA|%12llu\n", interface,
				SMGlobal::TCP_PACKET_PER_DAY[interface],
				SMGlobal::UDP_PACKET_PER_DAY[interface],
				SMGlobal::DNS_PACKET_PER_DAY[interface],
				SMGlobal::AAA_PACKET_PER_DAY[interface]);
	}
}

void ProbeStats::printRouterCount(char *runTime)
{
	uint16_t ti;
	uint32_t value = 0;

	for(uint16_t link = 0; link < SMGlobal::NO_OF_INTERFACE_SUPPORT; link ++)
	{
		printf("  TCP Router [%6s]                          ", SMGlobal::INTERFACE_NAME[link].c_str());
		for(ti = 0; ti < 10; ti++)
		{
			  for(uint16_t router = 0; router < SMGlobal::ROUTER_PER_INTERFACE[link]; router++)
			  {
				  value += agentStore::tcpACnt[link][router][ti];
			  }
			  printf("  %07d", value);
			  value = 0;
		  }
		  printf("\n");

		  printf("  UDP Router [%6s]                          ", SMGlobal::INTERFACE_NAME[link].c_str());

		  for(ti = 0; ti < 10; ti++)
		  {
			  for(uint16_t router = 0; router < SMGlobal::ROUTER_PER_INTERFACE[link]; router++)
			  {
				  value += agentStore::udpACnt[link][router][ti];
			  }
			  printf("  %07d", value);
			  value = 0;
		  }
		  printf("\n");

		  printf("  DNS Router [%6s]                          ", SMGlobal::INTERFACE_NAME[link].c_str());

		  for(ti = 0; ti < 10; ti++)
		  {
			  for(uint16_t router = 0; router < SMGlobal::ROUTER_PER_INTERFACE[link]; router++)
				  value += agentStore::dnsACnt[link][router][ti];

			  printf("  %07d", value);
			  value = 0;
		  }
		  printf("\n");

		  printf("  AAA Router [%6s]                          ", SMGlobal::INTERFACE_NAME[link].c_str());

		  for(ti = 0; ti < 10; ti++)
		  {
			  for(uint16_t router = 0; router < SMGlobal::ROUTER_PER_INTERFACE[link]; router++)
				  value += agentStore::aaaACnt[link][router][ti];

			  printf("  %07d", value);
			  value = 0;
		  }
		  printf("\n\n");
	  }
	  printf("\n");
}

void ProbeStats::printTcpSessionStoreCount()
{
	uint16_t tIdx = 0;
	uint32_t t_cnt[10];

	for(tIdx = 0; tIdx < 10; tIdx++)
		t_cnt[tIdx] = 0;

	for(uint16_t sm = 0; sm < SMGlobal::TCP_SM_INSTANCES; sm++)
	{
		printf("  TCP %07d  %07d  %07d        SM-%02d ->", (IPStats::tcpV4SessionTotalCnt[sm] + IPStats::tcpV6SessionTotalCnt[sm]), IPStats::tcpV4SessionScanned[sm], IPStats::tcpV4SessionCleaned[sm], sm);

		for(tIdx = 0; tIdx < 10; tIdx++)
		{
			for(uint16_t interface = 0; interface < SMGlobal::NO_OF_INTERFACE_SUPPORT; interface++)
				for(uint16_t router = 0; router < SMGlobal::ROUTER_PER_INTERFACE[interface]; router++)
					t_cnt[tIdx] += smStore::tcpSmCnt[sm][interface][router][tIdx];

			printf("  %07d",	t_cnt[tIdx]);
			t_cnt[tIdx] = 0;
		}
		printf("\n");
	}
}

void ProbeStats::printUdpSessionStoreCount()
{
	uint16_t tIdx = 0;
	uint32_t t_cnt[10];

	for(tIdx = 0; tIdx < 10; tIdx++)
		t_cnt[tIdx] = 0;

	for(uint16_t sm = 0; sm < SMGlobal::UDP_SM_INSTANCES; sm++)
	{
		printf("  UDP %07d  %07d  %07d        SM-%02d ->", (IPStats::udpV4SessionTotalCnt[sm] + IPStats::udpV6SessionTotalCnt[sm]), IPStats::udpV4SessionScanned[sm], IPStats::udpV4SessionCleaned[sm], sm);

		for(tIdx = 0; tIdx < 10; tIdx++)
		{
			for(uint16_t interface = 0; interface < SMGlobal::NO_OF_INTERFACE_SUPPORT; interface++)
				for(uint16_t router = 0; router < SMGlobal::ROUTER_PER_INTERFACE[interface]; router++)
					t_cnt[tIdx] += smStore::udpSmCnt[sm][interface][router][tIdx];

			printf("  %07d",	t_cnt[tIdx]);
			t_cnt[tIdx] = 0;
		}
		printf("\n");
	}
}

void ProbeStats::printAaaSessionStoreCount()
{
	uint16_t tIdx = 0;
	uint32_t t_cnt[10];

	for(tIdx = 0; tIdx < 10; tIdx++)
		t_cnt[tIdx] = 0;

	for(uint16_t sm = 0; sm < SMGlobal::AAA_SM_INSTANCES; sm++)
	{
		printf("  AAA %07d  %07d  %07d        SM-%02d ->", (IPStats::aaaAccessSessionCnt[sm] + IPStats::aaaAccounSessionCnt[sm]),
															(IPStats::aaaAccessSessionScanned[sm] + IPStats::aaaAccounSessionScanned[sm]),
															(IPStats::aaaAccessSessionCleaned[sm] + IPStats::aaaAccounSessionCleaned[sm]), sm);

		for(tIdx = 0; tIdx < 10; tIdx++)
		{
			for(uint16_t interface = 0; interface < SMGlobal::NO_OF_INTERFACE_SUPPORT; interface++)
				for(uint16_t router = 0; router < SMGlobal::ROUTER_PER_INTERFACE[interface]; router++)
					t_cnt[tIdx] += smStore::aaaSmCnt[sm][interface][router][tIdx];

			printf("  %07d",	t_cnt[tIdx]);
			t_cnt[tIdx] = 0;
		}
		printf("\n");
	}
}

void ProbeStats::printIpXdrFlushStats()
{
	uint16_t ti;
	uint32_t value = 0;
	char space[500] = "                               ";

	printf("\n%sTCP XDR Flush ->", space);

	for(ti = 0; ti < 10; ti++)
	{
		for(uint16_t flusher = 0; flusher < SMGlobal::TCP_FLUSHER_INSTANCE ; flusher++)
			for(uint16_t sm = 0; sm < SMGlobal::TCP_SM_INSTANCES; sm++)
				value += FlushRepo::tcpFlCnt[flusher][sm][ti];

		printf("  %07d", value);
		value = 0;
	}

	printf("\n%sUDP XDR Flush ->", space);

	for(ti = 0; ti < 10; ti++)
	{
		for(int flusher = 0; flusher < SMGlobal::UDP_FLUSHER_INSTANCE ; flusher++)
			for(int sm = 0; sm < SMGlobal::UDP_SM_INSTANCES; sm++)
				value += FlushRepo::udpFlCnt[flusher][sm][ti];

		printf("  %07d", value);
		value = 0;
	}

	printf("\n%sDNS XDR Flush ->", space);

	for(ti = 0; ti < 10; ti++)
	{
		for(int flusher = 0; flusher < SMGlobal::DNS_FLUSHER_INSTANCE ; flusher++)
			for(int sm = 0; sm < SMGlobal::UDP_SM_INSTANCES; sm++)
				value += FlushRepo::dnsFlCnt[flusher][sm][ti];

		printf("  %07d", value);
		value = 0;
	}

	printf("\n%sAAA XDR Flush ->", space);

	for(ti = 0; ti < 10; ti++)
	{
		for(int flusher = 0; flusher < SMGlobal::AAA_FLUSHER_INSTANCE ; flusher++)
			for(int sm = 0; sm < SMGlobal::AAA_SM_INSTANCES; sm++)
				value += FlushRepo::aaaFlCnt[flusher][sm][ti];

		printf("  %07d", value);
		value = 0;
	}
	printf("\n");
}

void ProbeStats::printSessionStoreCount()
{
	uint16_t i = 0;
	uint32_t t_cnt[10];
	uint64_t totalPkts = 0;

	for(i=0; i<10; i++)
		t_cnt[i] = 0;

	int SessionCount = 0,  SessionScanned = 0, SessionCleaned = 0;

	printf("      T.Pkt        S.Cnt    S.Scn    S.Cln \n");

	for(uint16_t sm = 0; sm < SMGlobal::TCP_SM_INSTANCES; sm++)
	{
		SessionCount += (IPStats::tcpV4SessionTotalCnt[sm] + IPStats::tcpV6SessionTotalCnt[sm]);
		SessionScanned += (IPStats::tcpV4SessionScanned[sm] + IPStats::tcpV6SessionScanned[sm]);
		SessionCleaned += (IPStats::tcpV4SessionCleaned[sm] + IPStats::tcpV6SessionCleaned[sm]);
		totalPkts += SMGlobal::TCP_SM_PKT_RECEIVED[sm];
	}

	printf("  TCP %012llu %07d  %07d  %07d   ", totalPkts, SessionCount, SessionScanned, SessionCleaned);

	for(i=0; i<10; i++)
	{
		for(uint16_t sm = 0; sm < SMGlobal::TCP_SM_INSTANCES; sm++)
			for(uint16_t j = 0; j < SMGlobal::NO_OF_INTERFACE_SUPPORT; j++)
				for(uint16_t k = 0; k < SMGlobal::ROUTER_PER_INTERFACE[j]; k++)
					t_cnt[i] += smStore::tcpSmCnt[sm][j][k][i];

		printf("  %07d",	t_cnt[i]);
		t_cnt[i] = 0;
	}
	printf("\n\n");

	for(i=0; i<10; i++)
		t_cnt[i] = 0;

	totalPkts = 0, SessionCount = 0,  SessionScanned = 0, SessionCleaned = 0;

	for(uint16_t sm = 0; sm < SMGlobal::UDP_SM_INSTANCES; sm++)
	{
		SessionCount += (IPStats::udpV4SessionTotalCnt[sm] + IPStats::udpV6SessionTotalCnt[sm]);
		SessionScanned += (IPStats::udpV4SessionScanned[sm] + IPStats::udpV6SessionScanned[sm]);
		SessionCleaned += (IPStats::udpV4SessionCleaned[sm] + IPStats::udpV6SessionCleaned[sm]);
		totalPkts += SMGlobal::UDP_SM_PKT_RECEIVED[sm];
	}

	printf("  UDP %012llu %07d  %07d  %07d   ", totalPkts, SessionCount, SessionScanned, SessionCleaned);

	for(i=0; i<10; i++)
	{
		for(uint16_t sm = 0; sm < SMGlobal::UDP_SM_INSTANCES; sm++)
			for(uint16_t j = 0; j < SMGlobal::NO_OF_INTERFACE_SUPPORT; j++)
				for(uint16_t k = 0; k < SMGlobal::ROUTER_PER_INTERFACE[j]; k++)
					t_cnt[i] += smStore::udpSmCnt[sm][j][k][i];

		printf("  %07d",	t_cnt[i]);
		t_cnt[i] = 0;
	}
	printf("\n\n");

	for(i=0; i<10; i++)
		t_cnt[i] = 0;

	totalPkts = 0, SessionCount = 0,  SessionScanned = 0, SessionCleaned = 0;

	for(uint16_t sm = 0; sm < SMGlobal::DNS_SM_INSTANCES; sm++)
	{
		SessionCount += (IPStats::dnsV4SessionTotalCnt[sm] + IPStats::dnsV6SessionTotalCnt[sm]);
		SessionScanned += (IPStats::dnsV4SessionScanned[sm] + IPStats::dnsV6SessionScanned[sm]);
		SessionCleaned += (IPStats::dnsV4SessionCleaned[sm] + IPStats::dnsV6SessionCleaned[sm]);
		totalPkts += SMGlobal::DNS_SM_PKT_RECEIVED[sm];
	}

	printf("  DNS %012llu %07d  %07d  %07d   ", totalPkts, SessionCount, SessionScanned, SessionCleaned);

	for(i=0; i<10; i++)
	{
		for(uint16_t sm = 0; sm < SMGlobal::DNS_SM_INSTANCES; sm++)
			for(uint16_t j = 0; j < SMGlobal::NO_OF_INTERFACE_SUPPORT; j++)
				for(uint16_t k = 0; k < SMGlobal::ROUTER_PER_INTERFACE[j]; k++)
					t_cnt[i] += smStore::dnsSmCnt[sm][j][k][i];

		printf("  %07d",	t_cnt[i]);
		t_cnt[i] = 0;
	}
	printf("\n\n");


	for(i=0; i<10; i++)
		t_cnt[i] = 0;

	totalPkts = 0, SessionCount = 0,  SessionScanned = 0, SessionCleaned = 0;

	for(uint16_t sm = 0; sm < SMGlobal::AAA_SM_INSTANCES; sm++)
	{
		SessionCount 	+= (IPStats::aaaAccessSessionCnt[sm] + IPStats::aaaAccounSessionCnt[sm]);
		SessionScanned 	+= (IPStats::aaaAccessSessionScanned[sm] + IPStats::aaaAccounSessionScanned[sm]);
		SessionCleaned 	+= (IPStats::aaaAccessSessionCleaned[sm] + IPStats::aaaAccounSessionCleaned[sm]);
		totalPkts += SMGlobal::AAA_SM_PKT_RECEIVED[sm];
	}

	printf("  AAA %012llu %07d  %07d  %07d   ", totalPkts, SessionCount, SessionScanned, SessionCleaned);

	for(i=0; i<10; i++)
	{
		for(uint16_t sm = 0; sm < SMGlobal::AAA_SM_INSTANCES; sm++)
			for(uint16_t j = 0; j < SMGlobal::NO_OF_INTERFACE_SUPPORT; j++)
				for(uint16_t k = 0; k < SMGlobal::ROUTER_PER_INTERFACE[j]; k++)
					t_cnt[i] += smStore::aaaSmCnt[sm][j][k][i];

		printf("  %07d",	t_cnt[i]);
		t_cnt[i] = 0;
	}
	printf("\n\n");

	totalPkts = 0, SessionCount = 0,  SessionScanned = 0, SessionCleaned = 0;

	for(uint16_t sm = 0; sm < SMGlobal::UNM_SM_INSTANCES; sm++)
	{
		SessionCount += (IPStats::unTcpSessionCnt[sm] + IPStats::unUdpSessionCnt[sm]);
		SessionScanned += (IPStats::unTcpSessionScanned[sm] + IPStats::unUdpSessionScanned[sm]);
		SessionCleaned += (IPStats::unTcpSessionCleaned[sm] + IPStats::unUdpSessionCleaned[sm]);

		totalPkts += SMGlobal::UNM_SM_PKT_RECEIVED[sm];
	}

	printf("  UNK %012llu %07d  %07d  %07d   ", totalPkts, SessionCount, SessionScanned, SessionCleaned);
}

