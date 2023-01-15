/*
 * GConfig.cpp
 *
 *  Created on: 26-Jul-2016
 *      Author: Debashis
 */

#include "GConfig.h"

GConfig::GConfig(char *fileName)
{
	loadInterfaceSettings(fileName);	/* ** Interface Settings ** */
	loadAgentsSettings(fileName);		/* ** Agents Setting ** */
	loadRoutersSettings(fileName);		/* ** Router Settings ** */
	loadSmSettings(fileName);			/* ** Session Manager Settings ** */
	loadFlushersSettings(fileName); 	/* ** Flusher Settings ** */
	loadFilePrefix(fileName);			/* ** XDR File Name Prefix ** */
	loadCleanUpSettings(fileName);		/* ** Cleanup Settings ** */
	loadEndOfDayCleanTime(fileName);	/* ** End of the Day Settings ** */
	loadTimerCore(fileName);			/* ** Timer Thread Core ** */
	loadDirSettings(fileName);			/* ** Directories Settings ** */
	loadTcpSettings(fileName);			/* ** TCP Settings ** */
	loadUdpSettings(fileName);			/* ** UDP Settings ** */
	loadAdminSettings(fileName);		/* ** Admin Settings ** */
	loadLogSettings(fileName);			/* ** Log Settings ** */
	loadPeeringIps(fileName);			/* ** I3D_NET ** */

}

GConfig::~GConfig()
{ }

void GConfig::loadInterfaceSettings(char *fileName)
{
	printf(" --------------------------- Interface Settings ----------------------------\n");
	openConfigFile(fileName);

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_noOfInterface(Key);		/* NO_OF_INTERFACE_SUPPORT */
		get_InterfaceName(Key);		/* INTERFACE_NAME */
		get_instanceId(Key);		/* ** INSTANCE_ID ** */ /* Instance Id = 1 (Will Dump AAA data) Instance Id = 2 (Will Read AAA Data) */
	}
	closeConfigFile();
	printf("\t NO_OF_INTERFACE_SUPPORT: %02d", SMGlobal::NO_OF_INTERFACE_SUPPORT);
	printf("\t INSTANCE_ID            : %02d", SMGlobal::INSTANCE_ID);
	printf("\t INTERFACE_NAME: ");

	for(uint16_t count = 0; count < SMGlobal::NO_OF_INTERFACE_SUPPORT; count++)
		printf("%s ", SMGlobal::INTERFACE_NAME[count].c_str());

	printf("\n\n");
}

void GConfig::loadAgentsSettings(char *fileName)
{
	openConfigFile(fileName);

	printf(" ------------------------------- Agents Settings -------------------------------\n");
	printf(" --------                        Tcp                         Udp                  Aaa            Dns            --------\n");

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_serverTcpAddress(Key);		/* TCP_SERVER_ADDRESS */
		get_serverUdpAddress(Key);		/* UDP_SERVER_ADDRESS */
		get_serverAaaAddress(Key);		/* AAA_SERVER_ADDRESS */
		get_serverDnsAddress(Key);		/* DNS_SERVER_ADDRESS */

		get_tcpPPSPerInterface(Key);	/* TCP_PPS_PER_INTERFACE */
		get_udpPPSPerInterface(Key);	/* UDP_PPS_PER_INTERFACE */
		get_aaaPPSPerInterface(Key);	/* AAA_PPS_PER_INTERFACE */
		get_dnsPPSPerInterface(Key);	/* DNS_PPS_PER_INTERFACE */

		get_tcpAgentCpuCore(Key);		/* TCP_AGENT_CORE */
		get_udpAgentCpuCore(Key);		/* UDP_AGENT_CORE */
		get_aaaAgentCpuCore(Key);		/* AAA_AGENT_CORE */
		get_dnsAgentCpuCore(Key);		/* DNS_AGENT_CORE */
	}

	for(uint16_t count = 0; count < SMGlobal::NO_OF_INTERFACE_SUPPORT; count ++)
	{
		printf(" [%02d] SERVER_ADDRESS:       %s               %s           %s       %s\n",  count + 1, SMGlobal::TCP_SERVER_ADDRESS[count].c_str(), SMGlobal::UDP_SERVER_ADDRESS[count].c_str(),
																		  SMGlobal::AAA_SERVER_ADDRESS[count].c_str(), SMGlobal::DNS_SERVER_ADDRESS[count].c_str());

		printf(" [%02d] PPS           :      %12d                %12d       %12d    %12d\n",  count + 1, SMGlobal::TCP_PPS_PER_INTERFACE[count], SMGlobal::UDP_PPS_PER_INTERFACE[count],
																									     SMGlobal::AAA_PPS_PER_INTERFACE[count], SMGlobal::DNS_PPS_PER_INTERFACE[count]);

		printf(" [%02d] CORE          :           %02d                          %02d                    %02d                %02d\n",  count + 1, SMGlobal::TCP_AGENT_CORE[count], SMGlobal::UDP_AGENT_CORE[count],
																																		SMGlobal::AAA_AGENT_CORE[count], SMGlobal::DNS_AGENT_CORE[count]);
	}
	closeConfigFile();

	printf("\n\n");
}

void GConfig::loadRoutersSettings(char *fileName)
{
	printf(" ------------------------------- Router Settings -------------------------------\n");
	printf(" --------  No. Of Router         Core         --------\n");

	openConfigFile(fileName);

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_routerPerInterface(Key);	/* ROUTER_PER_INTERFACE */
		get_routerCPU(Key);				/* ROUTER_CORE */
	}

	closeConfigFile();

	for(uint16_t count = 0; count < SMGlobal::NO_OF_INTERFACE_SUPPORT; count ++)
	{
		printf(" [%02d]      %02d              ",  count + 1, SMGlobal::ROUTER_PER_INTERFACE[count]);

		for(uint16_t rCount = 0; rCount < MAX_ROUTER_PER_INTERFACE; rCount ++)
			printf("  %02d",  SMGlobal::ROUTER_CORE[count][rCount]);

		printf("\n");
	}

	printf("\n\n");
}

void GConfig::loadSmSettings(char *fileName)
{
	printf(" ------------------------------- SM Settings -------------------------------\n");
	printf(" --------  No. Of SM                 Core         --------\n");


	openConfigFile(fileName);

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_noOfTcpSm(Key);		/* TCP_SM_INSTANCES */
		get_noOfUdpSm(Key);		/* UDP_SM_INSTANCES */
		get_noOfDnsSm(Key);		/* DNS_SM_INSTANCES */
		get_noOfAaaSm(Key);		/* AAA_SM_INSTANCES */
		get_noOfUnmSm(Key);		/* UNM_SM_INSTANCES */

		get_tcpSmCore(Key);		/* TCP_SM_CORE */
		get_udpSmCore(Key);		/* UDP_SM_CORE */
		get_dnsSmCore(Key);		/* UDP_SM_CORE */
		get_aaaSmCore(Key);		/* AAA_SM_CORE */
		get_unmSmCore(Key);		/* UNM_SM_CORE */
	}
	closeConfigFile();

	printf("          TCP  %02d           ",  SMGlobal::TCP_SM_INSTANCES);
	for(uint16_t count = 0; count < SMGlobal::TCP_SM_INSTANCES; count++)
		printf("  %02d", SMGlobal::TCP_SM_CORE[count]);

	printf("\n          UDP  %02d           ",  SMGlobal::UDP_SM_INSTANCES);
	for(uint16_t count = 0; count < SMGlobal::UDP_SM_INSTANCES; count++)
		printf("  %02d", SMGlobal::UDP_SM_CORE[count]);

	printf("\n          DNS  %02d           ",  SMGlobal::DNS_SM_INSTANCES);
	for(uint16_t count = 0; count < SMGlobal::DNS_SM_INSTANCES; count++)
		printf("  %02d", SMGlobal::DNS_SM_CORE[count]);

	printf("\n          AAA  %02d           ",  SMGlobal::AAA_SM_INSTANCES);
	for(uint16_t count = 0; count < SMGlobal::AAA_SM_INSTANCES; count++)
		printf("  %02d", SMGlobal::AAA_SM_CORE[count]);

	printf("\n          UNM  %02d           ",  SMGlobal::UNM_SM_INSTANCES);
	for(uint16_t count = 0; count < SMGlobal::UNM_SM_INSTANCES; count++)
		printf("  %02d", SMGlobal::UNM_SM_CORE[count]);

	printf("\n\n");
}

void GConfig::loadFlushersSettings(char *fileName)
{
	printf(" ------------------------------- Flusher Settings -------------------------------\n");
	printf(" --------  No. Of Flusher          Core         --------\n");

	openConfigFile(fileName);

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_noOfTcpFlusher(Key);	/* TCP_FLUSHER_INSTANCE */
		get_noOfUdpFlusher(Key);	/* UDP_FLUSHER_INSTANCE */
		get_noOfDnsFlusher(Key);	/* DNS_FLUSHER_INSTANCE */
		get_noOfAaaFlusher(Key);	/* AAA_FLUSHER_INSTANCE */

		get_tcpFlusherCore(Key);	/* TCP_FLUSHER_CORE */
		get_udpFlusherCore(Key);	/* UDP_FLUSHER_CORE */
		get_dnsFlusherCore(Key);	/* DNS_FLUSHER_CORE */
		get_aaaFlusherCore(Key);	/* AAA_FLUSHER_CORE */
		get_unmFlusherCore(Key);	/* UNM_FLUSHER_CORE */
	}
	closeConfigFile();

	printf("          TCP  %02d           ",  SMGlobal::TCP_FLUSHER_INSTANCE);
	for(uint16_t count = 0; count < SMGlobal::TCP_FLUSHER_INSTANCE; count++)
		printf("  %02d", SMGlobal::TCP_FLUSHER_CORE[count]);

	printf("\n          UDP  %02d           ",  SMGlobal::UDP_FLUSHER_INSTANCE);
	for(uint16_t count = 0; count < SMGlobal::UDP_FLUSHER_INSTANCE; count++)
		printf("  %02d", SMGlobal::UDP_FLUSHER_CORE[count]);

	printf("\n          DNS  %02d           ",  SMGlobal::DNS_FLUSHER_INSTANCE);
	for(uint16_t count = 0; count < SMGlobal::DNS_FLUSHER_INSTANCE; count++)
		printf("  %02d", SMGlobal::DNS_FLUSHER_CORE[count]);

	printf("\n          AAA  %02d           ",  SMGlobal::AAA_FLUSHER_INSTANCE);
	for(uint16_t count = 0; count < SMGlobal::AAA_FLUSHER_INSTANCE; count++)
		printf("  %02d", SMGlobal::AAA_FLUSHER_CORE[count]);

	printf("\n          UNM  %02d           ",  SMGlobal::UNM_FLUSHER_INSTANCE);
	for(uint16_t count = 0; count < SMGlobal::UNM_FLUSHER_INSTANCE; count++)
		printf("  %02d", SMGlobal::UNM_FLUSHER_CORE);

	printf("\n\n");
}

void GConfig::loadFilePrefix(char *fileName)
{
	openConfigFile(fileName);

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_tcpFilePrefix(Key);		/* TCP_FILE_PREFIX */
		get_udpFilePrefix(Key);		/* UDP_FILE_PREFIX */
		get_dnsFilePrefix(Key);		/* DNS_FILE_PREFIX */
		get_aaaFilePrefix(Key);		/* AAA_FILE_PREFIX */
	}
	closeConfigFile();
}

void GConfig::loadCleanUpSettings(char *fileName)
{
	printf(" ----- File Prefix --------- TimeOut --------------------------\n");
	openConfigFile(fileName);

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_tcpCleanUpTimeSec(Key);		/* TCP_CLEAN_UP_TIMEOUT_SEC */
  		get_udpCleanUpTimeSec(Key);		/* UDP_CLEAN_UP_TIMEOUT_SEC */
		get_dnsCleanUpTimeSec(Key);		/* DNS_CLEAN_UP_TIMEOUT_SEC */
		get_aaaCleanUpTimeSec(Key);		/* AAA_CLEAN_UP_TIMEOUT_SEC */
	}
	closeConfigFile();

	printf("          TCP : %s          %02d\n", SMGlobal::TCP_FILE_PREFIX.c_str(), SMGlobal::TCP_CLEAN_UP_TIMEOUT_SEC);
	printf("          UDP : %s          %02d\n", SMGlobal::UDP_FILE_PREFIX.c_str(), SMGlobal::UDP_CLEAN_UP_TIMEOUT_SEC);
	printf("          DNS : %s          %02d\n", SMGlobal::DNS_FILE_PREFIX.c_str(), SMGlobal::DNS_CLEAN_UP_TIMEOUT_SEC);
	printf("          AAA : %s          %02d\n", SMGlobal::AAA_FILE_PREFIX.c_str(), SMGlobal::AAA_CLEAN_UP_TIMEOUT_SEC);

	printf("\n\n");
}

void GConfig::loadEndOfDayCleanTime(char *fileName)
{
	printf(" ------------------ Loading End of The Day Clean Timer -------------------------------\n");
	openConfigFile(fileName);

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_endOfDayCleanTime(Key);						/* END_OF_DAY_CLEAN */
	}
	closeConfigFile();

	printf(" Hours: %02d| Minutes: %02d| Second: %02d", SMGlobal::DAY_CLEAN_HOUR, SMGlobal::DAY_CLEAN_MIN, SMGlobal::DAY_CLEAN_SEC);

	printf("\n\n");
}

void GConfig::loadTimerCore(char *fileName)
{
	openConfigFile(fileName);

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_timerThrCore(Key);							/* TIMER_THREAD_CORE */
	}
	printf(" Timer Core: %02d", SMGlobal::TIMER_THREAD_CORE);
	printf("\n\n");

	closeConfigFile();
}

void GConfig::loadDirSettings(char *fileName)
{
	printf(" ------------------ Loading Dir Settings -------------------------------\n");
	openConfigFile(fileName);

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_logDir(Key);								/* LOG_DIR */
		get_xdrDir(Key);								/* XDR_DIR */
		get_unmDir(Key);								/* UNM_DIR */
		get_dataDir(Key);								/* DATA_DIR */
	}
	closeConfigFile();

	printf("          LOG_DIR : %s\n", SMGlobal::LOG_DIR.c_str());
	printf("          XDR_DIR : %s\n", SMGlobal::XDR_DIR.c_str());
	printf("          UNM_DIR : %s\n", SMGlobal::UNM_DIR.c_str());
	printf("        DATA_DIR_1: %s\n", SMGlobal::DATA_DIR[0].c_str());
	printf("        DATA_DIR_2: %s\n", SMGlobal::DATA_DIR[1].c_str());
	printf("\n\n");
}

void GConfig::loadTcpSettings(char *fileName)
{
	printf(" ------------------ Loading Tcp Settings -------------------------------\n");
	openConfigFile(fileName);

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_vpsPacketPerSec(Key);		/* VPS_PACKET_PER_SEC */
		get_sessionPacketLimit(Key);	/* SESSION_PKT_LIMIT */
		get_sessionTimeLimit(Key);		/* SESSION_TIME_LIMIT */
		get_checkDuplicatePkt(Key);		/* CHECK_DUPLICATE_PKT */
		get_checkProcessAck(Key);		/* PROCESS_ACK */
		get_checkAckCreateSession(Key);	/* ACK_CREATE_SESSION */
	}
	closeConfigFile();

	printf("    VPS_PACKET_PER_SEC: %08d| SESSION_PKT_LIMIT: %08d| SESSION_TIME_LIMIT: %08d\n",
							SMGlobal::VPS_PACKET_PER_SEC, SMGlobal::SESSION_PKT_LIMIT, SMGlobal::SESSION_TIME_LIMIT);
	printf("   CHECK_DUPLICATE_PKT: %08d| PROCESS_ACK      : %08d| ACK_CREATE_SESSION: %08d\n",
							SMGlobal::CHECK_DUPLICATE_PKT, SMGlobal::PROCESS_ACK, SMGlobal::ACK_CREATE_SESSION);
	printf("\n\n");
}

void GConfig::loadUdpSettings(char *fileName)
{
	printf(" ------------------ Loading Udp Settings -------------------------------\n");
	openConfigFile(fileName);

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_udpXdrForDns(Key);							/* UDP_XDR_FOR_DNS */
	}
	closeConfigFile();

	printf("   UDP_XDR_FOR_DNS  : %08d\n", SMGlobal::UDP_XDR_FOR_DNS);
	printf("\n\n");

}

void GConfig::loadAdminSettings(char *fileName)
{
	openConfigFile(fileName);

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_adminFlag(Key);		/* ADMIN_FLAG */
		get_adminPort(Key);		/* ADMIN_PORT */
		get_dnsLoadFreq(Key);	/* DNS_LOAD_FREQ */
		get_aaaLoadFreq(Key);	/* AAA_LOAD_FREQ */
		get_aaaDumpFreq(Key);	/* AAA_DUMP_FREQ */

	}
	closeConfigFile();
}

void GConfig::loadLogSettings(char *fileName)
{
	printf(" ------------------ Loading log Settings -------------------------------\n");
	openConfigFile(fileName);

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_printStats(Key);	/* PRINT_STATS */
		get_probeId(Key);		/* PROBE_ID */
		get_logLevel(Key);		/* LOG_LEVEL */
		get_printStatsFreq(Key);/* PRINT_STATS_FREQ_SEC */
		get_logStatsFreq(Key);	/* LOG_STATS_FREQ_SEC */
	}
	closeConfigFile();

	printf("   Admin Flag: %08d| Admin Port   : %10s\n", SMGlobal::ADMIN_FLAG, SMGlobal::ADMIN_PORT.c_str());
	printf("   Probe Id  : %08d| Log Level    : %08d| Timer Core   : %08d\n", SMGlobal::PROBE_ID, SMGlobal::LOG_LEVEL, SMGlobal::TIMER_THREAD_CORE);
	printf("   Print Flag: %08d| Stats Freq   : %08d| Log Freq     : %08d\n", SMGlobal::PRINT_STATS, SMGlobal::PRINT_STATS_FREQ_SEC, SMGlobal::LOG_STATS_FREQ_SEC);
	printf("DNS Load Freq: %08d| AAA Load Freq: %08d| AAA Dump Freq: %08d\n", SMGlobal::DNS_LOAD_FREQ_SEC, SMGlobal::AAA_LOAD_FREQ_SEC, SMGlobal::AAA_DUMP_FREQ_SEC);

	printf("\n\n");
}

void GConfig::loadPeeringIps(char *fileName)
{
	printf(" ------------------ Loading log Settings -------------------------------\n");
	openConfigFile(fileName);

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_I3DNET(Key);				/* PEERING_I3DNET 				*/
		get_VERISIGN(Key);				/* PEERING_VERISIGN 			*/
		get_SUBSPACE(Key);				/* PEERING_SUBSPACE 			*/
		get_EDGECAST(Key);				/* PEERING_EDGECAST 			*/
		get_FASTLY(Key);				/* PEERING_FASTLY 				*/
		get_PKT_CLEARING_HOUSE(Key);	/* PEERING_PKT_CLEARING_HOUSE 	*/
		get_HUAWEI_CLOUD(Key);			/* PEERING_HUAWEI_CLOUD 		*/
		get_PEERING_TENCENT(Key);		/* PEERING_TENCENT  			*/
		get_ALIBABA(Key);				/* PEERING_ALIBABA 				*/
		get_APPLE(Key);					/* PEERING_QUANTIL 				*/
		get_MICROSOFT(Key);				/* PEERING_MICROSOFT 			*/
		get_LIMELIGHT(Key);				/* PEERING_LIMELIGHT 			*/
		get_ZENLAYER(Key);				/* PEERING_ZENLAYER 			*/
		get_CLOUDFLARE(Key);			/* PEERING_CLOUDFLARE 			*/
		get_DE_CIX(Key);				/* PEERING_DE_CIX 				*/
		get_AKAMAI(Key);				/* PEERING_AKAMAI 				*/
		get_NETFLIX(Key);				/* PEERING_NETFLIX 				*/
		get_GOOGLE(Key);				/* PEERING_GOOGLE 				*/
		get_FACEBOOK(Key);				/* PEERING_FACEBOOK 			*/
		get_AMAZON(Key);				/* PEERING_AMAZON 				*/
		get_UnknownSourcePool(Key);		/* UNKNOWN_SOURCE_POOL 			*/
		get_CacheIpPool(Key);			/* CACHE_IP_POOL 				*/
	}
	closeConfigFile();
}

void GConfig::get_I3DNET(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_I3DNET") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_I3DNET[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_I3DNET[cnt][1] = ipToLong(endIp, &x);

			SMGlobal::PEERING_I3DNET_COUNT++;

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_I3DNET_COUNT = cnt;
		printf("PEERING_I3DNET_COUNT = %d\n", SMGlobal::PEERING_I3DNET_COUNT);
	}
}

void GConfig::get_VERISIGN(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_VERISIGN") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_VERISIGN[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_VERISIGN[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_VERISIGN_COUNT = cnt;
		printf("PEERING_VERISIGN_COUNT = %d\n", SMGlobal::PEERING_VERISIGN_COUNT);
	}
}

void GConfig::get_SUBSPACE(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_SUBSPACE") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_SUBSPACE[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_SUBSPACE[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_SUBSPACE_COUNT = cnt;
		printf("PEERING_SUBSPACE_COUNT = %d\n", SMGlobal::PEERING_SUBSPACE_COUNT);
	}
}

void GConfig::get_EDGECAST(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_EDGECAST") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_EDGECAST[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_EDGECAST[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_EDGECAST_COUNT = cnt;
		printf("PEERING_EDGECAST_COUNT = %d\n", SMGlobal::PEERING_EDGECAST_COUNT);
	}
}

void GConfig::get_FASTLY(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_FASTLY") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_FASTLY[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_FASTLY[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_FASTLY_COUNT = cnt;
		printf("PEERING_FASTLY_COUNT = %d\n", SMGlobal::PEERING_FASTLY_COUNT);
	}
}

void GConfig::get_PKT_CLEARING_HOUSE(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_PKT_CLEARING_HOUSE") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_PKT_CLEARING_HOUSE[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_PKT_CLEARING_HOUSE[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_PKT_CLEARING_HOUSE_COUNT = cnt;
		printf("PEERING_PKT_CLEARING_HOUSE_COUNT = %d\n", SMGlobal::PEERING_PKT_CLEARING_HOUSE_COUNT);
	}
}

void GConfig::get_HUAWEI_CLOUD(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_HUAWEI_CLOUD") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_HUAWEI_CLOUD[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_HUAWEI_CLOUD[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_HUAWEI_CLOUD_COUNT = cnt;
		printf("PEERING_HUAWEI_CLOUD_COUNT = %d\n", SMGlobal::PEERING_HUAWEI_CLOUD_COUNT);
	}
}

void GConfig::get_PEERING_TENCENT(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_TENCENT") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_TENCENT[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_TENCENT[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_TENCENT_COUNT = cnt;
		printf("PEERING_TENCENT_COUNT = %d\n", SMGlobal::PEERING_TENCENT_COUNT);
	}
}

void GConfig::get_ALIBABA(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_ALIBABA") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_ALIBABA[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_ALIBABA[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_ALIBABA_COUNT = cnt;
		printf("PEERING_ALIBABA_COUNT = %d\n", SMGlobal::PEERING_ALIBABA_COUNT);
	}
}

void GConfig::get_APPLE(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_QUANTIL") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_QUANTIL[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_QUANTIL[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_QUANTIL_COUNT = cnt;
		printf("PEERING_QUANTIL_COUNT = %d\n", SMGlobal::PEERING_QUANTIL_COUNT);
	}
}

void GConfig::get_MICROSOFT(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_MICROSOFT") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_MICROSOFT[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_MICROSOFT[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_MICROSOFT_COUNT = cnt;
		printf("PEERING_MICROSOFT_COUNT = %d\n", SMGlobal::PEERING_MICROSOFT_COUNT);
	}
}

void GConfig::get_LIMELIGHT(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_LIMELIGHT") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_LIMELIGHT[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_LIMELIGHT[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_LIMELIGHT_COUNT = cnt;
		printf("PEERING_LIMELIGHT_COUNT = %d\n", SMGlobal::PEERING_LIMELIGHT_COUNT);
	}
}

void GConfig::get_ZENLAYER(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_ZENLAYER") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_ZENLAYER[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_ZENLAYER[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_ZENLAYER_COUNT = cnt;
		printf("PEERING_ZENLAYER_COUNT = %d\n", SMGlobal::PEERING_ZENLAYER_COUNT);
	}
}

void GConfig::get_CLOUDFLARE(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_CLOUDFLARE") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_CLOUDFLARE[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_CLOUDFLARE[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_CLOUDFLARE_COUNT = cnt;
		printf("PEERING_CLOUDFLARE_COUNT = %d\n", SMGlobal::PEERING_CLOUDFLARE_COUNT);
	}
}

void GConfig::get_DE_CIX(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_DE_CIX") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_DECIX[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_DECIX[cnt][1] = ipToLong(endIp, &x);;

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_DECIX_COUNT = cnt;
		printf("PEERING_DECIX_COUNT = %d\n", SMGlobal::PEERING_DECIX_COUNT);
	}
}

void GConfig::get_AKAMAI(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_AKAMAI") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_AKAMAI[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_AKAMAI[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_AKAMAI_COUNT = cnt;
		printf("PEERING_AKAMAI_COUNT = %d\n", SMGlobal::PEERING_AKAMAI_COUNT);
	}
}

void GConfig::get_NETFLIX(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_NETFLIX") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_NETFLIX[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_NETFLIX[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}

		SMGlobal::PEERING_NETFLIX_COUNT = cnt;
		printf("PEERING_NETFLIX_COUNT = %d\n", SMGlobal::PEERING_NETFLIX_COUNT);
	}
}

void GConfig::get_GOOGLE(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_GOOGLE") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_GOOGLE[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_GOOGLE[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_GOOGLE_COUNT = cnt;
		printf("PEERING_GOOGLE_COUNT = %d\n", SMGlobal::PEERING_GOOGLE_COUNT);
	}
}

void GConfig::get_FACEBOOK(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_FACEBOOK") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_FACEBOOK[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_FACEBOOK[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_FACEBOOK_COUNT = cnt;
		printf("PEERING_FACEBOOK_COUNT = %d\n", SMGlobal::PEERING_FACEBOOK_COUNT);
	}
}

void GConfig::get_AMAZON(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_AMAZON") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::PEERING_AMAZON[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::PEERING_AMAZON[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::PEERING_AMAZON_COUNT = cnt;
		printf("PEERING_AMAZON_COUNT = %d\n", SMGlobal::PEERING_AMAZON_COUNT);
	}
}

void GConfig::get_UnknownSourcePool(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("UNKNOWN_SOURCE_POOL") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::UNKNOWN_SOURCE_POOL[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::UNKNOWN_SOURCE_POOL[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::UNKNOWN_SOURCE_POOL_COUNT = cnt;
		printf("UNKNOWN_SOURCE_POOL_COUNT = %d\n", SMGlobal::UNKNOWN_SOURCE_POOL_COUNT);
	}
}

void GConfig::get_CacheIpPool(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("CACHE_IP_POOL") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			SMGlobal::CACHE_IP_POOL[cnt][0] = ipToLong(startIp, &x);
			SMGlobal::CACHE_IP_POOL[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		SMGlobal::CACHE_IP_POOL_COUNT = cnt;
		printf("CACHE_IP_POOL_COUNT = %d\n", SMGlobal::CACHE_IP_POOL_COUNT);
	}
}

void GConfig::get_instanceId(std::string& Key)
{
	Value.clear();

	if(Key.compare("INSTANCE_ID") == 0)
	{
		fp >> Value;
		SMGlobal::INSTANCE_ID = atol(Value.c_str());
	}
}

void GConfig::get_probeId(std::string& Key)
{
	Value.clear();

	if(Key.compare("PROBE_ID") == 0)
	{
		fp >> Value;
		SMGlobal::PROBE_ID = atol(Value.c_str());
//		printf("\tPROBE_ID               			:: %d\n", SMGlobal::PROBE_ID);
	}
}

void GConfig::get_logLevel(std::string& Key)
{
	Value.clear();

	if(Key.compare("LOG_LEVEL") == 0)
	{
		fp >> Value;
		SMGlobal::LOG_LEVEL = atoi(Value.c_str());
//		printf("\tLOG_LEVEL               		:: %d\n", SMGlobal::LOG_LEVEL);
	}
}

void GConfig::get_printStats(std::string& Key)
{
	Value.clear();

	if(Key.compare("PRINT_STATS") == 0)
	{
		fp >> Value;
		SMGlobal::PRINT_STATS = Value.compare("true") == 0 ? true : false;
//		printf("	PRINT_STATS				:: [%d] [%s]\n", SMGlobal::PRINT_STATS, Value.c_str());
	}
}

void GConfig::get_printStatsFreq(std::string& Key)
{
	Value.clear();

	if(Key.compare("PRINT_STATS_FREQ_SEC") == 0)
	{
		fp >> Value;
		SMGlobal::PRINT_STATS_FREQ_SEC = atoi(Value.c_str());
//		printf("	PRINT_STATS_FREQ_SEC			:: %d\n", SMGlobal::PRINT_STATS_FREQ_SEC);
	}
}

void GConfig::get_logStatsFreq(std::string& Key)
{
	Value.clear();

	if(Key.compare("LOG_STATS_FREQ_SEC") == 0)
	{
		fp >> Value;
		SMGlobal::LOG_STATS_FREQ_SEC = atoi(Value.c_str());
//		printf("	LOG_STATS_FREQ_SEC			:: %d\n", SMGlobal::LOG_STATS_FREQ_SEC);
	}
}

void GConfig::get_logDir(std::string& Key)
{
	Value.clear();

	if(Key.compare("LOG_DIR") == 0)
	{
		fp >> Value;
		SMGlobal::LOG_DIR = Value;
//		printf("\tLOG_DIR               			:: %s\n", SMGlobal::LOG_DIR.c_str());
	}
}

void GConfig::get_xdrDir(std::string& Key)
{
	Value.clear();

	if(Key.compare("XDR_DIR") == 0)
	{
		fp >> Value;
		SMGlobal::XDR_DIR = Value;
	}
}

void GConfig::get_unmDir(std::string& Key)
{
	Value.clear();

	if(Key.compare("UNM_DIR") == 0)
	{
		fp >> Value;
		SMGlobal::UNM_DIR = Value;
	}
}

void GConfig::get_dataDir(std::string& Key)
{
	Value.clear();

	if(Key.compare("DATA_DIR") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			SMGlobal::DATA_DIR[cnt] = std::string(pch);
			pch = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_adminFlag(std::string& Key)
{
	Value.clear();

	if(Key.compare("ADMIN_FLAG") == 0)
	{
		fp >> Value;
		SMGlobal::ADMIN_FLAG = Value.compare("true") == 0 ? 1 : 0;
	}
}

void GConfig::get_adminPort(std::string& Key)
{
	Value.clear();

	if(Key.compare("ADMIN_PORT") == 0)
	{
			fp >> Value;
			SMGlobal::ADMIN_PORT = Value;
	}
}

void GConfig::get_dnsLoadFreq(std::string& Key)
{
	Value.clear();

	if(Key.compare("DNS_LOAD_FREQ_SEC") == 0)
	{
		fp >> Value;
		SMGlobal::DNS_LOAD_FREQ_SEC = atol(Value.c_str());
	}
}

void GConfig::get_aaaLoadFreq(std::string& Key)
{
	Value.clear();

	if(Key.compare("AAA_LOAD_FREQ_SEC") == 0)
	{
		fp >> Value;
		SMGlobal::AAA_LOAD_FREQ_SEC = atol(Value.c_str());
	}
}

void GConfig::get_aaaDumpFreq(std::string& Key)
{
	Value.clear();

	if(Key.compare("AAA_DUMP_FREQ_SEC") == 0)
	{
		fp >> Value;
		SMGlobal::AAA_DUMP_FREQ_SEC = atol(Value.c_str());
	}
}


void GConfig::get_timerThrCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("TIMER_THREAD_CORE") == 0)
	{
			fp >> Value;
			SMGlobal::TIMER_THREAD_CORE = atoi(Value.c_str());
	}
}

void GConfig::get_noOfInterface(std::string& Key)
{
	Value.clear();

	if(Key.compare("NO_OF_INTERFACE_SUPPORT") == 0)
	{
			fp >> Value;
			SMGlobal::NO_OF_INTERFACE_SUPPORT = atoi(Value.c_str());
	}
}

void GConfig::get_InterfaceName(std::string& Key)
{
	Value.clear();

	if(Key.compare("INTERFACE_NAME") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			SMGlobal::INTERFACE_NAME[cnt] = std::string(pch);
			pch = strtok (NULL, ",");
			cnt++;
		}
	}
}


void GConfig::get_tcpPPSPerInterface(std::string& Key)
{
	Value.clear();

	if(Key.compare("TCP_PPS_PER_INTERFACE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::TCP_PPS_PER_INTERFACE[cnt] = atoi(pch1);
			pch1 = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::NO_OF_INTERFACE_SUPPORT)
				break;
		}
	}
}

void GConfig::get_udpPPSPerInterface(std::string& Key)
{
	Value.clear();

	if(Key.compare("UDP_PPS_PER_INTERFACE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::UDP_PPS_PER_INTERFACE[cnt] = atoi(pch1);
//			printf("\tUDP_PPS_PER_INTERFACE[%d]     		:: %d\n", cnt, SMGlobal::UDP_PPS_PER_INTERFACE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_aaaPPSPerInterface(std::string& Key)
{
	Value.clear();

	if(Key.compare("AAA_PPS_PER_INTERFACE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::AAA_PPS_PER_INTERFACE[cnt] = atoi(pch1);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_dnsPPSPerInterface(std::string& Key)
{
	Value.clear();

	if(Key.compare("DNS_PPS_PER_INTERFACE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::DNS_PPS_PER_INTERFACE[cnt] = atoi(pch1);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_serverTcpAddress(std::string& Key)
{
	Value.clear();

	if(Key.compare("TCP_SERVER_ADDRESS") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			SMGlobal::TCP_SERVER_ADDRESS[cnt] = std::string(pch);
			pch = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::NO_OF_INTERFACE_SUPPORT)
				break;
		}
	}
}

void GConfig::get_serverUdpAddress(std::string& Key)
{
	Value.clear();

	if(Key.compare("UDP_SERVER_ADDRESS") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			SMGlobal::UDP_SERVER_ADDRESS[cnt] = std::string(pch);
			pch = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::NO_OF_INTERFACE_SUPPORT)
				break;
		}
	}
}

void GConfig::get_serverAaaAddress(std::string& Key)
{
	Value.clear();

	if(Key.compare("AAA_SERVER_ADDRESS") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			SMGlobal::AAA_SERVER_ADDRESS[cnt] = std::string(pch);
			pch = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::NO_OF_INTERFACE_SUPPORT)
				break;
		}
	}
}

void GConfig::get_serverDnsAddress(std::string& Key)
{
	Value.clear();

	if(Key.compare("DNS_SERVER_ADDRESS") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			SMGlobal::DNS_SERVER_ADDRESS[cnt] = std::string(pch);
			pch = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::NO_OF_INTERFACE_SUPPORT)
				break;
		}
	}
}

void GConfig::get_tcpAgentCpuCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("TCP_AGENT_CORE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::TCP_AGENT_CORE[cnt] = atoi(pch1);
			pch1 = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::NO_OF_INTERFACE_SUPPORT)
				break;

		}
	}
}

void GConfig::get_udpAgentCpuCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("UDP_AGENT_CORE") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			SMGlobal::UDP_AGENT_CORE[cnt] = atoi(pch);
			pch = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::NO_OF_INTERFACE_SUPPORT)
				break;
		}
	}
}

void GConfig::get_aaaAgentCpuCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("AAA_AGENT_CORE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::AAA_AGENT_CORE[cnt] = atoi(pch1);
			pch1 = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::NO_OF_INTERFACE_SUPPORT)
				break;
		}
	}
}

void GConfig::get_dnsAgentCpuCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("DNS_AGENT_CORE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::DNS_AGENT_CORE[cnt] = atoi(pch1);
			pch1 = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::NO_OF_INTERFACE_SUPPORT)
				break;
		}
	}
}

void GConfig::get_routerPerInterface(std::string& Key)
{
	Value.clear();

	if(Key.compare("ROUTER_PER_INTERFACE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::ROUTER_PER_INTERFACE[cnt] = atoi(pch1);
			pch1 = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::NO_OF_INTERFACE_SUPPORT)
				break;
		}
	}
}

void GConfig::get_routerCPU(std::string& Key)
{
	Value.clear();

	if(Key.compare("ROUTER_CORE") == 0)
	{
		fp >> Value;
		char *pchHash, *pchComma;
		int cnt, cnt1;

		cnt = cnt1 = 0;
		size_t pos = 0;
		std::string token;

		while ((pos = Value.find("-")) != std::string::npos)
		{
		    token = Value.substr(0, pos);
		    pchHash = strtok((char *)token.c_str(),",");

		    while (pchHash != NULL)
			{
				SMGlobal::ROUTER_CORE[cnt1][cnt] = atoi(pchHash);
				pchHash = strtok (NULL, ",");
				cnt++;
			}
			cnt1++;

			if(cnt1 >= SMGlobal::NO_OF_INTERFACE_SUPPORT)
				return;

			cnt = 0;
		    Value.erase(0, pos + 1);
		}
		cnt = 0;
		pchComma = strtok((char *)Value.c_str(),",");

		while (pchComma != NULL)
		{
			SMGlobal::ROUTER_CORE[cnt1][cnt] = atoi(pchComma);
			pchComma = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_sessionPacketLimit(std::string& Key)
{
	Value.clear();

	if(Key.compare("SESSION_PKT_LIMIT") == 0)
	{
		fp >> Value;
		SMGlobal::SESSION_PKT_LIMIT = atoi(Value.c_str());
	}
}

void GConfig::get_sessionTimeLimit(std::string& Key)
{
	Value.clear();

	if(Key.compare("SESSION_TIME_LIMIT") == 0)
	{
		fp >> Value;
		SMGlobal::SESSION_TIME_LIMIT = atoi(Value.c_str());
	}
}

void GConfig::get_vpsPacketPerSec(std::string& Key)
{
	Value.clear();

	if(Key.compare("VPS_PACKET_PER_SEC") == 0)
	{
			fp >> Value;
			SMGlobal::VPS_PACKET_PER_SEC = atoi(Value.c_str());
	}
}

void GConfig::get_endOfDayCleanTime(std::string& Key)
{
	Value.clear();

	if(Key.compare("DAY_CLEAN_HOUR") == 0)
	{
		fp >> Value;
		SMGlobal::DAY_CLEAN_HOUR = atoi(Value.c_str());
	}

	Value.clear();

	if(Key.compare("DAY_CLEAN_MIN") == 0)
	{
		fp >> Value;
		SMGlobal::DAY_CLEAN_MIN = atoi(Value.c_str());
	}

	Value.clear();

	if(Key.compare("DAY_CLEAN_SEC") == 0)
	{
		fp >> Value;
		SMGlobal::DAY_CLEAN_SEC = atoi(Value.c_str());
	}
}


/*
 * TCP Configuration Parameters
 */

void GConfig::get_noOfTcpSm(std::string& Key)
{
	Value.clear();

	if(Key.compare("TCP_SM_INSTANCES") == 0)
	{
			fp >> Value;
			SMGlobal::TCP_SM_INSTANCES = atoi(Value.c_str());;
	}
}

void GConfig::get_tcpSmCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("TCP_SM_CORE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::TCP_SM_CORE[cnt] = atoi(pch1);
			pch1 = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::TCP_SM_INSTANCES)
				break;
		}
	}
}

void GConfig::get_noOfTcpFlusher(std::string& Key)
{
	Value.clear();

	if(Key.compare("TCP_FLUSHER_INSTANCE") == 0)
	{
		fp >> Value;
		SMGlobal::TCP_FLUSHER_INSTANCE = atoi(Value.c_str());
	}
}

void GConfig::get_tcpFlusherCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("TCP_FLUSHER_CORE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::TCP_FLUSHER_CORE[cnt] = atoi(pch1);
			pch1 = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::TCP_FLUSHER_INSTANCE)
				break;
		}
	}
}

void GConfig::get_tcpFilePrefix(std::string& Key)
{
	Value.clear();

	if(Key.compare("TCP_FILE_PREFIX") == 0)
	{
			fp >> Value;
			SMGlobal::TCP_FILE_PREFIX = Value;
//			printf("\tTCP_FILE_PREFIX\t\t\t\t:: %s\n", SMGlobal::TCP_FILE_PREFIX.c_str());
	}
}

void GConfig::get_checkDuplicatePkt(std::string& Key)
{
	Value.clear();
	if(Key.compare("CHECK_DUPLICATE_PKT") == 0)
	{
		fp >> Value;
		SMGlobal::CHECK_DUPLICATE_PKT = Value.compare("true") == 0 ? true : false;
//		printf("	CHECK_DUPLICATE PKT			:: %s\n", Value.c_str());
	}
}

void GConfig::get_checkProcessAck(std::string& Key)
{
	Value.clear();

	if(Key.compare("PROCESS_ACK") == 0)
	{
		fp >> Value;
		SMGlobal::PROCESS_ACK = Value.compare("true") == 0 ? true : false;
//		printf("	PROCESS_ACK 				:: %s\n", Value.c_str());
	}
}

void GConfig::get_checkAckCreateSession(std::string& Key)
{
	Value.clear();
	if(Key.compare("ACK_CREATE_SESSION") == 0)
	{
		fp >> Value;
		SMGlobal::ACK_CREATE_SESSION = Value.compare("true") == 0 ? true : false;
//		printf("	ACK_CREATE_SESSION 			:: %s\n", Value.c_str());
	}
}

void GConfig::get_tcpCleanUpTimeSec(std::string& Key)
{
	Value.clear();

	if(Key.compare("TCP_CLEAN_UP_TIMEOUT_SEC") == 0)
	{
		fp >> Value;
		SMGlobal::TCP_CLEAN_UP_TIMEOUT_SEC = atoi(Value.c_str());
//		printf("\tTCP_CLEAN_UP_TIMEOUT_SEC\t:: %d\n", SMGlobal::TCP_CLEAN_UP_TIMEOUT_SEC);
	}
}

/*
 * UDP Configuration Parameters
 */

void GConfig::get_noOfUdpSm(std::string& Key)
{
	Value.clear();

	if(Key.compare("UDP_SM_INSTANCES") == 0)
	{
			fp >> Value;
			SMGlobal::UDP_SM_INSTANCES = atoi(Value.c_str());;
//			printf("\tUDP_SM_INSTANCES\t\t:: %d\n", SMGlobal::UDP_SM_INSTANCES);
	}
}

void GConfig::get_udpSmCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("UDP_SM_CORE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::UDP_SM_CORE[cnt] = atoi(pch1);
//			printf("\tUDP_SM_CORE[%d]		:: %d\n", cnt, SMGlobal::UDP_SM_CORE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::UDP_SM_INSTANCES)
				break;
		}
	}
}

void GConfig::get_noOfUdpFlusher(std::string& Key)
{
	Value.clear();

	if(Key.compare("UDP_FLUSHER_INSTANCE") == 0)
	{
		fp >> Value;
		SMGlobal::UDP_FLUSHER_INSTANCE = atoi(Value.c_str());
//		printf("\tUDP_FLUSHER_INSTANCE			:: %d\n", SMGlobal::UDP_FLUSHER_INSTANCE);
	}
}

void GConfig::get_udpFlusherCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("UDP_FLUSHER_CORE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::UDP_FLUSHER_CORE[cnt] = atoi(pch1);
//			printf("\tUDP_FLUSHER_CORE[%d]     		:: %d\n", cnt, SMGlobal::UDP_FLUSHER_CORE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::UDP_FLUSHER_INSTANCE)
				break;
		}
	}
}

void GConfig::get_udpFilePrefix(std::string& Key)
{
	Value.clear();

	if(Key.compare("UDP_FILE_PREFIX") == 0)
	{
			fp >> Value;
			SMGlobal::UDP_FILE_PREFIX = Value;
//			printf("\tUDP_FILE_PREFIX\t\t\t\t:: %s\n", SMGlobal::UDP_FILE_PREFIX.c_str());
	}
}

void GConfig::get_udpXdrForDns(std::string& Key)
{
	Value.clear();

	if(Key.compare("UDP_XDR_FOR_DNS") == 0)
	{
		fp >> Value;
		if(Value.compare("true") == 0)
			SMGlobal::UDP_XDR_FOR_DNS = true;
		else
			SMGlobal::UDP_XDR_FOR_DNS = false;
//		printf("\tUDP_WRITE_XDR\t\t\t\t:: %s\n", Value.c_str());
	}

}

void GConfig::get_udpCleanUpTimeSec(std::string& Key)
{
	Value.clear();

	if(Key.compare("UDP_CLEAN_UP_TIMEOUT_SEC") == 0)
	{
		fp >> Value;
		SMGlobal::UDP_CLEAN_UP_TIMEOUT_SEC = atoi(Value.c_str());
//		printf("\tUDP_CLEAN_UP_TIMEOUT_SEC\t:: %d\n", SMGlobal::UDP_CLEAN_UP_TIMEOUT_SEC);
	}
}

/*
 * DNS Configuration Parameters
 */

void GConfig::get_noOfDnsSm(std::string& Key)
{
	Value.clear();

	if(Key.compare("DNS_SM_INSTANCES") == 0)
	{
			fp >> Value;
			SMGlobal::DNS_SM_INSTANCES = atoi(Value.c_str());;
//			printf("\tDNS_SM_INSTANCES\t\t:: %d\n", SMGlobal::DNS_SM_INSTANCES);
	}
}

void GConfig::get_dnsSmCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("DNS_SM_CORE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::DNS_SM_CORE[cnt] = atoi(pch1);
//			printf("\tDNS_SM_CORE[%d]		:: %d\n", cnt, SMGlobal::DNS_SM_CORE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::DNS_SM_INSTANCES)
				break;
		}
	}
}

void GConfig::get_noOfDnsFlusher(std::string& Key)
{
	Value.clear();

	if(Key.compare("DNS_FLUSHER_INSTANCE") == 0)
	{
		fp >> Value;
		SMGlobal::DNS_FLUSHER_INSTANCE = atoi(Value.c_str());
//		printf("\tDNS_FLUSHER_INSTANCE			:: %d\n", SMGlobal::DNS_FLUSHER_INSTANCE);
	}
}

void GConfig::get_dnsFlusherCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("DNS_FLUSHER_CORE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::DNS_FLUSHER_CORE[cnt] = atoi(pch1);
//			printf("\tDNS_FLUSHER_CORE[%d]     		:: %d\n", cnt, SMGlobal::DNS_FLUSHER_CORE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::DNS_FLUSHER_INSTANCE)
				break;
		}
	}
}

void GConfig::get_dnsFilePrefix(std::string& Key)
{
	Value.clear();

	if(Key.compare("DNS_FILE_PREFIX") == 0)
	{
			fp >> Value;
			SMGlobal::DNS_FILE_PREFIX = Value;
//			printf("\tDNS_FILE_PREFIX\t\t\t\t:: %s\n", SMGlobal::DNS_FILE_PREFIX.c_str());
	}
}

void GConfig::get_dnsCleanUpTimeSec(std::string& Key)
{
	Value.clear();

	if(Key.compare("DNS_CLEAN_UP_TIMEOUT_SEC") == 0)
	{
		fp >> Value;
		SMGlobal::DNS_CLEAN_UP_TIMEOUT_SEC = atoi(Value.c_str());
//		printf("\tDNS_CLEAN_UP_TIMEOUT_SEC\t:: %d\n", SMGlobal::DNS_CLEAN_UP_TIMEOUT_SEC);
	}
}

/*
 * AAA Configuration Parameters
 */

void GConfig::get_noOfAaaSm(std::string& Key)
{
	Value.clear();

	if(Key.compare("AAA_SM_INSTANCES") == 0)
	{
			fp >> Value;
			SMGlobal::AAA_SM_INSTANCES = atoi(Value.c_str());;
//			printf("\tAAA_SM_INSTANCES\t\t:: %d\n", SMGlobal::AAA_SM_INSTANCES);
	}
}

void GConfig::get_aaaSmCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("AAA_SM_CORE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::AAA_SM_CORE[cnt] = atoi(pch1);
//			printf("\tAAA_SM_CORE[%d]		:: %d\n", cnt, SMGlobal::AAA_SM_CORE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::AAA_SM_INSTANCES)
				break;
		}
	}
}

void GConfig::get_noOfAaaFlusher(std::string& Key)
{
	Value.clear();

	if(Key.compare("AAA_FLUSHER_INSTANCE") == 0)
	{
		fp >> Value;
		SMGlobal::AAA_FLUSHER_INSTANCE = atoi(Value.c_str());
//		printf("\tAAA_FLUSHER_INSTANCE			:: %d\n", SMGlobal::AAA_FLUSHER_INSTANCE);
	}
}

void GConfig::get_aaaFlusherCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("AAA_FLUSHER_CORE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::AAA_FLUSHER_CORE[cnt] = atoi(pch1);
//			printf("\tAAA_FLUSHER_CORE[%d]     		:: %d\n", cnt, SMGlobal::AAA_FLUSHER_CORE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::AAA_FLUSHER_INSTANCE)
				break;
		}
	}
}

void GConfig::get_aaaFilePrefix(std::string& Key)
{
	Value.clear();

	if(Key.compare("AAA_FILE_PREFIX") == 0)
	{
			fp >> Value;
			SMGlobal::AAA_FILE_PREFIX = Value;
//			printf("\tAAA_FILE_PREFIX\t\t\t\t:: %s\n", SMGlobal::AAA_FILE_PREFIX.c_str());
	}
}

void GConfig::get_aaaCleanUpTimeSec(std::string& Key)
{
	Value.clear();

	if(Key.compare("AAA_CLEAN_UP_TIMEOUT_SEC") == 0)
	{
		fp >> Value;
		SMGlobal::AAA_CLEAN_UP_TIMEOUT_SEC = atoi(Value.c_str());
//		printf("\tAAA_CLEAN_UP_TIMEOUT_SEC\t:: %d\n", SMGlobal::AAA_CLEAN_UP_TIMEOUT_SEC);
	}
}

/***** UNKNOWN PARAMETERS *****/

void GConfig::get_noOfUnmSm(std::string& Key)
{
	Value.clear();

	if(Key.compare("UNM_SM_INSTANCES") == 0)
	{
			fp >> Value;
			SMGlobal::UNM_SM_INSTANCES = atoi(Value.c_str());;
//			printf("\tUNM_SM_INSTANCES\t\t:: %d\n", SMGlobal::UNM_SM_INSTANCES);
	}
}

void GConfig::get_unmSmCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("UNM_SM_CORE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			SMGlobal::UNM_SM_CORE[cnt] = atoi(pch1);
//			printf("\tUNM_SM_CORE[%d]		:: %d\n", cnt, SMGlobal::UNM_SM_CORE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;

			if(cnt >= SMGlobal::UNM_SM_INSTANCES)
				break;
		}
	}
}

void GConfig::get_unmFlusherCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("UNM_FLUSHER_CORE") == 0)
	{
		fp >> Value;
		SMGlobal::UNM_FLUSHER_CORE = atoi(Value.c_str());
//		printf("\tUNM_FLUSHER_CORE     		:: %d\n", SMGlobal::UNM_FLUSHER_CORE);
	}
}

void GConfig::openConfigFile(char *fileName)
{
	char probeConfigBaseDir[100];
	char *probeConfigDir;
	char *probeRootEnv;

	probeConfigDir = getenv("PROBE_CONF");
	probeRootEnv = getenv("PROBE_ROOT");

	if(probeConfigDir == NULL || probeRootEnv == NULL)
	{
		printf("\n\n\n  !!! ******* SpectaProbe Environment NOT Set ******* !!! \n\n\n");
		exit(1);
	}
	sprintf(probeConfigBaseDir, "%s/%s", probeConfigDir, fileName);
	fp.open(probeConfigBaseDir);


	if(fp.fail())
	{
		printf("  Error in Opening Configuration File : %s\n", probeConfigBaseDir);
		exit(1);
	}
}

void GConfig::closeConfigFile()
{ fp.close(); }

void GConfig::converSubNetToRange(char *ipr, char *Start, char *End)
{
	string str1 = "";
	string str2 = "";

	int idx = 0;
	int len = strlen(ipr) - 3;

	while(len--)
	{
		str1 += ipr[idx];
		idx++;
	}

	strcpy(Start, str1.c_str());

	idx++;
	str2 += ipr[idx];
	idx++;
	str2 += ipr[idx];

	strcpy(End, initSection::ipSubNetMap[atoi(str2.c_str())].c_str());
}

uint32_t GConfig::ipToLong(char *ip, uint32_t *plong)
{
	char *next = NULL;
	const char *curr = ip;
	uint32_t tmp;
	int i, err = 0;

	*plong = 0;

	for (i = 0; i < NUM_OCTETTS; i++)
	{
		tmp = strtoul(curr, &next, 10);
		if (tmp >= 256 || (tmp == 0 && next == curr))
		{
			err++;
			break;
		}
		*plong = (*plong << 8) + tmp;
		curr = next + 1;
	}

	if (err)
		return 1;
	else
		return *plong;
}
