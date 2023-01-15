/*
 * ProbeStatsLog.cpp
 *
 *  Created on: Jul 21, 2017
 *      Author: Debashis
 */

#include "ProbeStatsLog.h"

#include <ctime>

ProbeStatsLog::ProbeStatsLog()
{
	this->_name = "ProbeStatsLog";
	this->setLogLevel(Log::theLog().level());
}

ProbeStatsLog::~ProbeStatsLog()
{ }

void ProbeStatsLog::run()
{
	uint16_t printloopCnt = 0;
	char buffer[80];
	long startTime = 0;
	long runTime = 0;
	uint16_t dd = 0, hh = 0, mm = 0, ss = 0;

	startTime = Global::CURRENT_EPOCH_SEC;

	while(SMGlobal::LOG_RUNNING_STATUS)
	{
		  sleep(1);
		  printloopCnt++;

		  runTime = Global::CURRENT_EPOCH_SEC - startTime;

		  dd = (int)(runTime / 84600);
		  hh = (int)((runTime - (dd * 84600)) / 3600);
		  mm = (int)((runTime - ((dd * 84600) + (hh * 3600))) / 60);
		  ss = (int)(runTime - ((dd * 84600) + (hh * 3600) + (mm * 60)));
		  sprintf(buffer, "%02d:%02d:%02d:%02d", dd, hh, mm, ss);

		  if(printloopCnt >= SMGlobal::LOG_STATS_FREQ_SEC)
		  {
			  TheLog_nc_v1(Log::Info, name(),"%s", "");
			  TheLog_nc_v1(Log::Info, name(),"    ** Running Time: %s", buffer);
			  printPktReceived();
			  printSMReport();
			  printfFlusherReport();
			  printloopCnt = 0;
		  }
	}
	printf("  Log Thread Stopped...\n");
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void ProbeStatsLog::printPktReceived()
{
	for (uint16_t interface = 0; interface < SMGlobal::NO_OF_INTERFACE_SUPPORT; interface++)
	{
		TheLog_nc_v4(Log::Info, name(),"    Interface [%02d] Pck Count TCP|%12llu    UDP|%12llu    AAA|%12llu", interface,
				SMGlobal::TCP_PACKET_PER_DAY[interface],
				SMGlobal::UDP_PACKET_PER_DAY[interface],
				SMGlobal::AAA_PACKET_PER_DAY[interface]);
	}
	TheLog_nc_v1(Log::Info, name(),"%s", "");
}

void ProbeStatsLog::printSMReport()
{
	char buffer[500];
	uint16_t sm = 0;

	uint64_t totalPkts = 0, dnsTotalPkts = 0;

	uint32_t SessionV4Count = 0,  SessionV4Scanned = 0, SessionV4Cleaned = 0;
	uint32_t SessionV6Count = 0, SessionV6Scanned = 0, SessionV6Cleaned = 0;

	uint32_t accessSession = 0, accountingSession = 0;
	uint32_t udpIv4SessionCnt = 0, udpIv6SessionCnt = 0;
	uint32_t dnsIv4SessionCnt = 0, dnsIv6SessionCnt = 0;

	uint32_t unTcpIv4SessionCnt = 0, unTcpIv6SessionCnt = 0;
	uint32_t unUdpIv4SessionCnt = 0, unUdpIv6SessionCnt = 0;

	buffer[0] = 0;

	TheLog_nc_v1(Log::Info, name(),"     Total Packet           Session            Scan              Clean %s", "");
	TheLog_nc_v1(Log::Info, name(),"  %s", "==========================================================================================");
	TheLog_nc_v1(Log::Info, name(),"         Protocol         V4       V6       V4       V6       V4       V6    %s", "");
	TheLog_nc_v1(Log::Info, name(),"  %s", "------------------------------------------------------------------------------------------");

	/* ********************** T C P ********************** */
	for(sm = 0; sm < SMGlobal::TCP_SM_INSTANCES; sm++)
	{
		SessionV4Count 		+= IPStats::tcpV4SessionTotalCnt[sm];
		SessionV6Count 		+= IPStats::tcpV6SessionTotalCnt[sm];

		SessionV4Scanned 	+= IPStats::tcpV4SessionScanned[sm];
		SessionV6Scanned 	+= IPStats::tcpV6SessionScanned[sm];

		SessionV4Cleaned 	+= IPStats::tcpV4SessionCleaned[sm];
		SessionV6Cleaned 	+= IPStats::tcpV6SessionCleaned[sm];

		totalPkts 			+= SMGlobal::TCP_SM_PKT_RECEIVED[sm];
	}

	sprintf(buffer, "    TCP %012llu    %07u  %07u  %07u  %07u  %07u  %07u   ", totalPkts, SessionV4Count, SessionV6Count,
																						SessionV4Scanned, SessionV6Scanned,
																						SessionV4Cleaned, SessionV6Cleaned);
	TheLog_nc_v1(Log::Info, name(),"%s", buffer);

	totalPkts = 0;
	SessionV4Count = 0,  SessionV6Count = 0;
	SessionV4Scanned = 0, SessionV6Scanned = 0;
	SessionV4Cleaned = 0, SessionV4Cleaned = 0;
	buffer[0] = 0;

	/* ********************** UDP ********************** */
	for(sm = 0; sm < SMGlobal::UDP_SM_INSTANCES; sm++)
	{
		SessionV4Count 		+= IPStats::udpV4SessionTotalCnt[sm];
		SessionV6Count 		+= IPStats::udpV6SessionTotalCnt[sm];

		SessionV4Scanned 	+= IPStats::udpV4SessionScanned[sm];
		SessionV6Scanned 	+= IPStats::udpV6SessionScanned[sm];

		SessionV4Cleaned 	+= IPStats::udpV4SessionCleaned[sm];
		SessionV6Cleaned 	+= IPStats::udpV6SessionCleaned[sm];

 		totalPkts 			+= SMGlobal::UDP_SM_PKT_RECEIVED[sm];
	}
	sprintf(buffer, "    UDP %012llu    %07u  %07u  %07u  %07u  %07u  %07u   ", totalPkts, SessionV4Count, SessionV6Count,
																						SessionV4Scanned, SessionV6Scanned,
																						SessionV4Cleaned, SessionV6Cleaned);
	TheLog_nc_v1(Log::Info, name(),"%s", buffer);

	totalPkts = 0;
	SessionV4Count = 0,  SessionV6Count = 0;
	SessionV4Scanned = 0, SessionV6Scanned = 0;
	SessionV4Cleaned = 0, SessionV4Cleaned = 0;
	buffer[0] = 0;

	/* ********************** DNS ********************** */
	for(sm = 0; sm < SMGlobal::DNS_SM_INSTANCES; sm++)
	{
		SessionV4Count 		+= IPStats::dnsV4SessionTotalCnt[sm];
		SessionV6Count 		+= IPStats::dnsV6SessionTotalCnt[sm];

		SessionV4Scanned 	+= IPStats::dnsV4SessionScanned[sm];
		SessionV6Scanned 	+= IPStats::dnsV6SessionScanned[sm];

		SessionV4Cleaned 	+= IPStats::dnsV4SessionCleaned[sm];
		SessionV6Cleaned 	+= IPStats::dnsV6SessionCleaned[sm];

 		totalPkts 			+= SMGlobal::DNS_SM_PKT_RECEIVED[sm];
	}
	sprintf(buffer, "    DNS %012llu    %07u  %07u  %07u  %07u  %07u  %07u   ", totalPkts, SessionV4Count, SessionV6Count,
																						SessionV4Scanned, SessionV6Scanned,
																						SessionV4Cleaned, SessionV6Cleaned);
	TheLog_nc_v1(Log::Info, name(),"%s", buffer);
	TheLog_nc_v1(Log::Info, name(),"%s", "");

	totalPkts = 0;
	SessionV4Count = 0,  SessionV6Count = 0;
	SessionV4Scanned = 0, SessionV6Scanned = 0;
	SessionV4Cleaned = 0, SessionV4Cleaned = 0;
	buffer[0] = 0;

	/* ********************** UNKNOWN ********************** */
	for(sm = 0; sm < SMGlobal::UNM_SM_INSTANCES; sm++)
	{
		SessionV4Count 		+= IPStats::unTcpSessionCnt[sm];
		SessionV6Count 		+= IPStats::unUdpSessionCnt[sm] ;

		SessionV4Scanned 		+= IPStats::unTcpSessionScanned[sm];
		SessionV6Scanned 		+= IPStats::unUdpSessionScanned[sm];

		SessionV4Cleaned 		+= IPStats::unTcpSessionCleaned[sm];
		SessionV6Cleaned 		+= IPStats::unUdpSessionCleaned[sm];

		totalPkts 			+= SMGlobal::UNM_SM_PKT_RECEIVED[sm];

	}
	TheLog_nc_v1(Log::Info, name(),"     Total Packet           Session                  Scan                 Clean %s", "");
	TheLog_nc_v1(Log::Info, name(),"  %s", "==========================================================================================");
	TheLog_nc_v1(Log::Info, name(),"         UNM              TCP       UDP         TCP        UDP        TCP       UDP    %s", "");
	TheLog_nc_v1(Log::Info, name(),"  %s", "------------------------------------------------------------------------------------------");

	sprintf(buffer, "        %012llu    %07u    %07u    %07u    %07u    %07u    %07u   ", totalPkts, SessionV4Count, SessionV6Count,
																						SessionV4Scanned, SessionV6Scanned,
																						SessionV4Cleaned, SessionV6Cleaned);
	TheLog_nc_v1(Log::Info, name(),"%s", buffer);
	TheLog_nc_v1(Log::Info, name(),"%s", "");

	totalPkts = 0;
	SessionV4Count = 0,  SessionV6Count = 0;
	SessionV4Scanned = 0, SessionV6Scanned = 0;
	SessionV4Cleaned = 0, SessionV4Cleaned = 0;
	buffer[0] = 0;

	/* ********************** A A A ********************** */

	for(sm = 0; sm < SMGlobal::AAA_SM_INSTANCES; sm++)
	{
		SessionV4Count   		+= IPStats::aaaAccessSessionCnt[sm];
		SessionV6Count   		+= IPStats::aaaAccounSessionCnt[sm];

		SessionV4Scanned 		+= IPStats::aaaAccessSessionScanned[sm];
		SessionV6Scanned 		+= IPStats::aaaAccounSessionScanned[sm];

		SessionV4Cleaned 		+= IPStats::aaaAccessSessionCleaned[sm];
		SessionV6Cleaned 		+= IPStats::aaaAccounSessionCleaned[sm];

		totalPkts += SMGlobal::AAA_SM_PKT_RECEIVED[sm];

	}
	TheLog_nc_v1(Log::Info, name(),"     Total Packet             Session                Scan                  Clean %s", "");
	TheLog_nc_v1(Log::Info, name(),"  %s", "==========================================================================================");
	TheLog_nc_v1(Log::Info, name(),"         AAA              Acca       Acco       Acca       Acco       Acca       Acco    %s", "");
	TheLog_nc_v1(Log::Info, name(),"  %s", "------------------------------------------------------------------------------------------");

	sprintf(buffer, "        %012llu    %07u    %07u    %07u    %07u    %07u    %07u   ", totalPkts, SessionV4Count, SessionV6Count,
																						SessionV4Scanned, SessionV6Scanned,
																						SessionV4Cleaned, SessionV6Cleaned);
	TheLog_nc_v1(Log::Info, name(),"%s", buffer);
	TheLog_nc_v1(Log::Info, name(),"%s", "");
	TheLog_nc_v1(Log::Info, name(),"    DnsLookUp Size       IPv4: %08u", IPStats::dnsLookupMapSize);
	TheLog_nc_v1(Log::Info, name(),"%s", "");
	totalPkts = 0;
}

void ProbeStatsLog::printfFlusherReport()
{
	uint16_t tcpValue = 0, udpValue = 0, dnsValue = 0, aaaValue = 0;

	for(uint16_t flusher = 0; flusher < SMGlobal::TCP_FLUSHER_INSTANCE ; flusher++)
		for(uint16_t sm = 0; sm < SMGlobal::TCP_SM_INSTANCES; sm++)
			for(uint16_t tdx = 0; tdx < 10; tdx++)
				tcpValue += FlushRepo::tcpFlCnt[flusher][sm][tdx];

	for(uint16_t flusher = 0; flusher < SMGlobal::UDP_FLUSHER_INSTANCE ; flusher++)
		for(uint16_t sm = 0; sm < SMGlobal::UDP_SM_INSTANCES; sm++)
			for(uint16_t tdx = 0; tdx < 10; tdx++)
				udpValue += FlushRepo::udpFlCnt[flusher][sm][tdx];

	for(uint16_t flusher = 0; flusher < SMGlobal::DNS_FLUSHER_INSTANCE ; flusher++)
		for(uint16_t sm = 0; sm < SMGlobal::UDP_SM_INSTANCES; sm++)
			for(uint16_t tdx = 0; tdx < 10; tdx++)
				dnsValue += FlushRepo::dnsFlCnt[flusher][sm][tdx];

	for(uint16_t flusher = 0; flusher < SMGlobal::AAA_FLUSHER_INSTANCE ; flusher++)
		for(uint16_t sm = 0; sm < SMGlobal::AAA_SM_INSTANCES; sm++)
			for(uint16_t tdx = 0; tdx < 10; tdx++)
				aaaValue += FlushRepo::aaaFlCnt[flusher][sm][tdx];

	TheLog_nc_v4(Log::Info, name(),"    Flusher TCP|%07d    UDP|%07d    DNS|%07d     AAA|%07d",
			tcpValue, udpValue, dnsValue, aaaValue);


}
