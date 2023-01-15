/*
 * Initialize.cpp
 *
 *  Created on: 18 Mar 2021
 *      Author: Debashis
 */

#include "Initialize.h"

void Initialize::lockDnsMap()
{
	pthread_mutex_lock(&mapDnsLock::lockCount);
	while (mapDnsLock::count == 0)
		pthread_cond_wait(&mapDnsLock::nonzero, &mapDnsLock::lockCount);
	mapDnsLock::count = mapDnsLock::count - 1;
	pthread_mutex_unlock(&mapDnsLock::lockCount);
}

void Initialize::unLockDnsMap()
{
    pthread_mutex_lock(&mapDnsLock::lockCount);
    if (mapDnsLock::count == 0)
        pthread_cond_signal(&mapDnsLock::nonzero);
    mapDnsLock::count = mapDnsLock::count + 1;
    pthread_mutex_unlock(&mapDnsLock::lockCount);
}

void Initialize::lockAAAMap()
{
	pthread_mutex_lock(&mapAAALock::lockCount);
	while (mapAAALock::count == 0)
		pthread_cond_wait(&mapAAALock::nonzero, &mapAAALock::lockCount);
	mapAAALock::count = mapAAALock::count - 1;
	pthread_mutex_unlock(&mapAAALock::lockCount);
}

void Initialize::unLockAAAMap()
{
    pthread_mutex_lock(&mapAAALock::lockCount);
    if (mapAAALock::count == 0)
        pthread_cond_signal(&mapAAALock::nonzero);
    mapAAALock::count = mapAAALock::count + 1;
    pthread_mutex_unlock(&mapAAALock::lockCount);
}


Initialize::Initialize()
{
	_thisLogLevel = 0;
	this->_name = "Initialize";
	this->setLogLevel(Log::theLog().level());

	initProtocolName();
	dnsErrorCode();
	tcpPorts();
	aaaInitCode();
	aaaServiceType();
	aaaProtocol();
	aaaAcctAuthentication();
	aaaAccTerminationCause();
	aaaAccStatus();
	aaaNasPortType();
	ipSubNet();

	printf("  *** [01] Initialization Completed... \n");
}

Initialize::~Initialize()
{ }

void Initialize::initProtocolName()
{
	initSection::protocolName.insert(std::pair<uint32_t, std::string>(6, "TCP"));
	initSection::protocolName.insert(std::pair<uint32_t, std::string>(17, "UDP"));
	initSection::protocolName.insert(std::pair<uint32_t, std::string>(1812, "RADIUS-AUTH"));
	initSection::protocolName.insert(std::pair<uint32_t, std::string>(2812, "RADIUS-AUTH"));
	initSection::protocolName.insert(std::pair<uint32_t, std::string>(1813, "RADIUS-ACCO"));
	initSection::protocolName.insert(std::pair<uint32_t, std::string>(2813, "RADIUS-ACCO"));
}


void Initialize::dnsErrorCode()
{
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(0, "No Error"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(1, "Format Error"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(2, "Server Failure"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(3, "No Such Name"));	/* Non-Existent Domain Name */
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(4, "Not Implemented"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(5, "Query Refused"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(6, "Name Exists when it should not"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(7, "RR Set Exists when it should not"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(8, "RR Set that should exist does not"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(9, "Server Not Authoritative for zone"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(10, "Name not contained in zone"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(11, "UNASSIGNED"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(12, "UNASSIGNED"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(13, "UNASSIGNED"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(14, "UNASSIGNED"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(15, "UNASSIGNED"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(16, "Bad OPT Version"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(17, "Key not recognized"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(18, "Signature out of time window"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(19, "Bad TKEY Mode"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(20, "Duplicate key name"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(21, "Algorithm not supported"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(22, "Bad Truncation"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(23, "Bad/missing server cookie"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(24, "Bad Address"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(25, "No Answer Count"));
	initSection::dnsErrorCode.insert(std::pair<uint32_t, std::string>(26, "Name Server"));
}

void Initialize::tcpPorts()
{
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(1, "TCPMUX"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(5, "Remote Job Entry"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(7, "ECHO"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(18, "Message Send Protocol"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(20, "FTP-Data"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(21, "FTP-Control"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(22, "SSH"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(23, "TELNET"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(25, "SMTP"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(29, "MSG ICP"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(37, "Time"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(42, "Host Name Server"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(43, "WhoIs"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(53, "DNS"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(69, "TFTP"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(70, "Gopher Services"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(79, "Finger"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(80, "HTTP"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(103, "X.400 Standard"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(108, "SNA Gateway Access Server"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(109, "POP2"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(110, "POP3"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(115, "SFTP"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(118, "SQL Services"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(119, "Newsgroup"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(137, "NetBIOS Name Service"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(139, "NetBIOS Datagram Service"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(143, "IMAP"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(150, "NetBIOS Session Service"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(156, "SQL Server"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(161, "SNMP"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(179, "Border Gateway Protocol"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(190, "GACP"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(194, "Internet Relay Chat"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(197, "DLS"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(389, "LDAP"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(396, "Novell Netware over IP"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(443, "HTTPS"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(444, "SNPP"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(445, "Microsoft-DS"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(458, "Apple QuickTime"));
	initSection::tcpPorts.insert(std::pair<uint32_t, std::string>(546, "DHCP Client"));
}

void Initialize::aaaInitCode()
{
	initSection::radiusCodeMap.insert(std::pair<int, std::string>(0, "NA"));
	initSection::radiusCodeMap.insert(std::pair<int, std::string>(1, "Access-Request"));
	initSection::radiusCodeMap.insert(std::pair<int, std::string>(2, "Access-Accept"));
	initSection::radiusCodeMap.insert(std::pair<int, std::string>(3, "Access-Reject"));
	initSection::radiusCodeMap.insert(std::pair<int, std::string>(4, "Accounting-Request"));
	initSection::radiusCodeMap.insert(std::pair<int, std::string>(5, "Accounting-Response"));
}

void Initialize::aaaServiceType()
{
	initSection::serviceTypeMap.insert(std::pair<int, std::string>(0, "NA"));
	initSection::serviceTypeMap.insert(std::pair<int, std::string>(1, "Login"));
	initSection::serviceTypeMap.insert(std::pair<int, std::string>(2, "Framed"));
	initSection::serviceTypeMap.insert(std::pair<int, std::string>(3, "Callback Login"));
	initSection::serviceTypeMap.insert(std::pair<int, std::string>(4, "Callback Framed"));
	initSection::serviceTypeMap.insert(std::pair<int, std::string>(5, "Outbound"));
	initSection::serviceTypeMap.insert(std::pair<int, std::string>(6, "Administrative"));
	initSection::serviceTypeMap.insert(std::pair<int, std::string>(7, "NAS Prompt"));
	initSection::serviceTypeMap.insert(std::pair<int, std::string>(8, "Authenticate Only"));
	initSection::serviceTypeMap.insert(std::pair<int, std::string>(9, "Callback NAS Prompt"));
	initSection::serviceTypeMap.insert(std::pair<int, std::string>(10, "Call Check"));
	initSection::serviceTypeMap.insert(std::pair<int, std::string>(11, "Callback Administrative"));
}

void Initialize::aaaProtocol()
{
	initSection::framedProtocolMap.insert(std::pair<int, std::string>(0, "NA"));
	initSection::framedProtocolMap.insert(std::pair<int, std::string>(1, "PPP"));
	initSection::framedProtocolMap.insert(std::pair<int, std::string>(2, "SLIP"));
	initSection::framedProtocolMap.insert(std::pair<int, std::string>(3, "ARAP"));
	initSection::framedProtocolMap.insert(std::pair<int, std::string>(4, "SingleLink/MultiLink Protocol"));
	initSection::framedProtocolMap.insert(std::pair<int, std::string>(5, "IPX/SLIP"));
	initSection::framedProtocolMap.insert(std::pair<int, std::string>(6, "X.75 Synchronous"));
}

void Initialize::aaaAcctAuthentication()
{
	initSection::acctAuthenticMap.insert(std::pair<int, std::string>(0, "NA"));
	initSection::acctAuthenticMap.insert(std::pair<int, std::string>(1, "Radius"));
	initSection::acctAuthenticMap.insert(std::pair<int, std::string>(2, "Local"));
	initSection::acctAuthenticMap.insert(std::pair<int, std::string>(3, "Remote"));
}

void Initialize::aaaAccTerminationCause()
{
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(0, "NA"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(1, "User Request"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(2, "Lost Carrier"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(3, "Lost Service"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(4, "Idle Timeout"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(5, "Session Timeout"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(6, "Admin Reset"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(7, "Admin Reboot"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(8, "Port Error"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(9, "NAS Error"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(10, "NAS Request"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(11, "NAS Reboot"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(12, "Port Unneeded"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(13, "Port Preempted"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(14, "Port Suspended"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(15, "Service Unavailable"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(16, "Callback"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(17, "User Error"));
	initSection::acctTeminateMap.insert(std::pair<int, std::string>(18, "Host Request"));
}

void Initialize::aaaAccStatus()
{
	initSection::acctStatusMap.insert(std::pair<int, std::string>(0, "NA"));
	initSection::acctStatusMap.insert(std::pair<int, std::string>(1, "Start"));
	initSection::acctStatusMap.insert(std::pair<int, std::string>(2, "Stop"));
	initSection::acctStatusMap.insert(std::pair<int, std::string>(3, "Interim-Update"));
	initSection::acctStatusMap.insert(std::pair<int, std::string>(7, "Accounting-On"));
	initSection::acctStatusMap.insert(std::pair<int, std::string>(8, "Accounting-Off"));
	initSection::acctStatusMap.insert(std::pair<int, std::string>(9, "Reserved for Tunnel Accounting"));
	initSection::acctStatusMap.insert(std::pair<int, std::string>(10, "Reserved for Tunnel Accounting"));
	initSection::acctStatusMap.insert(std::pair<int, std::string>(11, "Reserved for Tunnel Accounting"));
	initSection::acctStatusMap.insert(std::pair<int, std::string>(12, "Reserved for Tunnel Accounting"));
	initSection::acctStatusMap.insert(std::pair<int, std::string>(13, "Reserved for Tunnel Accounting"));
	initSection::acctStatusMap.insert(std::pair<int, std::string>(14, "Reserved for Tunnel Accounting"));
	initSection::acctStatusMap.insert(std::pair<int, std::string>(15, "Reserved for Failed"));
}

void Initialize::aaaNasPortType()
{
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(-1, "NA"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(0, "Async"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(1, "Sync"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(2, "ISDN Sync"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(3, "ISDN Async V.120"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(4, "ISDN Async V.110"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(5, "Virtual"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(6, "PIAFS"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(7, "HDLC Clear Channel"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(8, "X.25"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(9, "X.75"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(10, "G.3 Fax"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(11, "SDSL - Symmetric DSL"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(12, "ADSL-CAP"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(13, "ADSL-DMT"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(14, "IDSL"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(15, "Ethernet"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(16, "xDSL"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(17, "Cable"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(18, "Wireless - Others"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(19, "Wireless - IEEE 802.11"));
	initSection::nasPortTypeMap.insert(std::pair<int, std::string>(33, "PPPoEoVLAN"));
}

void Initialize::ipSubNet()
{
	printf("ipSubNet Initialized....\n");

	initSection::ipSubNetMap.insert(std::pair<int, std::string>(0, "0.0.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(1, "128.0.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(2, "192.0.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(3, "224.0.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(4, "240.0.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(5, "248.0.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(6, "252.0.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(7, "254.0.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(8, "255.0.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(9, "255.128.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(10, "255.192.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(11, "255.224.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(12, "255.240.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(13, "255.248.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(14, "255.252.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(15, "255.254.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(16, "255.255.0.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(17, "255.255.128.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(18, "255.255.192.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(19, "255.255.224.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(20, "255.255.240.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(21, "255.255.248.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(22, "255.255.252.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(23, "255.255.254.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(24, "255.255.255.0"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(25, "255.255.255.128"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(26, "255.255.255.192"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(27, "255.255.255.224"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(28, "255.255.255.240"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(29, "255.255.255.248"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(30, "255.255.255.252"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(31, "255.255.255.254"));
	initSection::ipSubNetMap.insert(std::pair<int, std::string>(32, "255.255.255.255"));
}

void Initialize::load1ResolvedIpv4()
{
	string filePath = SMGlobal::DATA_DIR[0] + "dnsIpv4data.csv";

	std::string line;
	std::string url;
	uint32_t ip;
	uint32_t loadedCnt = 0;
	char* pch;

	ifstream fp;

	fp.open(filePath.c_str());

	lockDnsMap();

	if(fp.fail())
	{
		TheLog_nc_v1(Log::Info, name(),"SpectaProbe Error in Loading IPv4 DNS Lookup file [%s]", filePath.c_str());
	}
	else
	{
		while(!fp.eof())
		{
			getline(fp, line);
			if(!line.empty() && line.length() > 0)
			{
				pch = strtok ((char *)line.c_str(), ",");

				if(pch == NULL) break;
				ip = atol(pch);

				pch = strtok (NULL, ",");  // Error in Here

				if(pch == NULL) break;

				url = std::string(pch);

				if(ip > 0)
				{
					loadedCnt++;
					DNSGlobal::dnsLookUpMap[ip % 10][ip] = url;
				}
			}
		}
		fp.close();
	}
	unLockDnsMap();

//	TheLog_nc_v2(Log::Info, name(),"     Loaded [01] [ %06u ] Records in Dns IPv4 Lookup Map from file [%s]", loadedCnt, filePath.c_str());
//	printf("     Loaded [ %06u ] Records in Dns IPv4 Lookup Map from file [%s].\n", loadedCnt, filePath.c_str());
}

void Initialize::load2ResolvedIpv4()
{
	string filePath = SMGlobal::DATA_DIR[1] + "dnsIpv4data.csv";

	std::string line;
	std::string url;
	uint32_t ip;
	uint32_t loadedCnt = 0;
	char* pch;

	ifstream fp;

	fp.open(filePath.c_str());

	lockDnsMap();

	if(fp.fail())
	{
		TheLog_nc_v1(Log::Info, name(),"SpectaProbe Error in Loading IPv4 DNS Lookup file [%s]", filePath.c_str());
	}
	else
	{
		while(!fp.eof())
		{
			getline(fp, line);
			if(!line.empty() && line.length() > 0)
			{
				pch = strtok ((char *)line.c_str(), ",");

				if(pch == NULL) break;
				ip = atol(pch);

				pch = strtok (NULL, ",");  // Error in Here

				if(pch == NULL) break;

				url = std::string(pch);

				if(ip > 0)
				{
					loadedCnt++;
					DNSGlobal::dnsLookUpMap[ip % 10][ip] = url;
				}
			}
		}
		fp.close();
	}
	unLockDnsMap();

//	TheLog_nc_v2(Log::Info, name(),"     Loaded [02] [ %06u ] Records in Dns IPv4 Lookup Map from file [%s]", loadedCnt, filePath.c_str());
//	printf("     Loaded [ %06u ] Records in Dns IPv4 Lookup Map from file [%s].\n", loadedCnt, filePath.c_str());
}

void Initialize::loadAAAData()
{
	TheLog_nc(Log::Info, name(),"SpectaProbe Read AAA Data ... ");

	std::string filePath = "/data/SpectaProbe/aaaUserIpv4.csv";
	std::string line;
	uint32_t userIp;
	std::string userId;

	int lineCnt = 0, loadedCnt = 0;
	char* 		pch;

	ifstream fp;

	lockAAAMap();

	fp.open(filePath.c_str());

	if(fp.fail())
	{
		printf("Error in Reading IPv4 User file [%s]\n", filePath.c_str());
		TheLog_nc_v1(Log::Warn, name()," Error in Loading IPv4 User file [%s]", filePath.c_str());
	}
	else
	{
		while(!fp.eof())
		{
			getline(fp,line);
			lineCnt++;

			userId.clear();
			userIp = 0;

			if(!line.empty() && line.length() > 0)
			{
				pch = strtok ((char *)line.c_str(), ",");
				userIp = atol(pch);

				if(userIp == 0) continue;

				pch = strtok (NULL, ",");
				userId = std::string(pch);

				if(userId.length() == 0) continue;

				if(userId.length() > 2)
				{
					strcpy(aaaGlbMap::aaaGlbUserIpMap[userIp].userName, userId.c_str());
					aaaGlbMap::aaaGlbUserIpMap[userIp].allocatedIpLong = (long)userIp;

					strcpy(aaaGlbMap::aaaGlbUserIdMap[userId].userName, userId.c_str());
					aaaGlbMap::aaaGlbUserIdMap[userId].allocatedIpLong = (long)userIp;
				}
				loadedCnt++;
			}
		}
		fp.close();
	}
	unLockAAAMap();

	printf(" Loaded [%d] records in IPv4 User Map from file [%s] reading [%d] lines\n", loadedCnt, filePath.c_str(), lineCnt);
	TheLog_nc_v3(Log::Info, name()," Loaded [%d] records in IPv4 User Map from file [%s] reading [%d] lines", loadedCnt, filePath.c_str(), lineCnt);
}

void Initialize::dumpAAAData()
{
	char 		fileName[50] = "";
	ofstream 	fileHandler;
	bool		dumpFlag = true;
	uint16_t 	len = 0;


	string filePath 		= "/data/SpectaProbe/aaaUserIpv4.csv.in";
	string finalFilePath 	= "/data/SpectaProbe/aaaUserIpv4.csv";

	userInfo	*pUserInfo;
	int 		noOfRecords = 0;

	/* Current Dumping File */
	fileHandler.open(filePath.c_str(), ios :: out);

	lockAAAMap();

	if(fileHandler.fail())
	{
		printf(" Error in Dumping Current Radius IPv4 User Store File : %s\n", filePath.c_str());
		TheLog_nc_v1(Log::Warn, name()," Error in Dumping Current Radius IPv4 User Store to file [%s]...", filePath.c_str());
	}
	else
	{
		for(auto elem : aaaGlbMap::aaaGlbUserIpMap)
		{
			dumpFlag = true;
			for(len = 0; len < strlen(elem.second.userName); len++)
			{
				if(!isprint(elem.second.userName[len]))
				{
					dumpFlag = false;
					break;
				}
			}
			if(strlen(elem.second.userName) > 2 && dumpFlag == true)
			{
				fileHandler << elem.first << ","					// User IP
						    << elem.second.userName <<  endl;		// User Id
			}
		}

		fileHandler.close();
		printf(" Dumping Current Radius IPv4 User to file [%s]...Completed\n", filePath.c_str());
		TheLog_nc_v1(Log::Info, name()," Dumping Current Radius IPv4 User to file [%s]...Completed", filePath.c_str());

		rename(filePath.c_str(), finalFilePath.c_str());

		filePath.clear();
		finalFilePath.clear();
	}
	unLockAAAMap();
}

void Initialize::clearAAAData()
{
	lockAAAMap();
	aaaGlbMap::aaaGlbUserIpMap.clear();

	if(aaaGlbMap::aaaGlbUserIpMap.size() == 0 )
		TheLog_nc_v1(Log::Info, name()," aaaGlbUserIpMap Size is %d", 0);

	unLockAAAMap();
}
