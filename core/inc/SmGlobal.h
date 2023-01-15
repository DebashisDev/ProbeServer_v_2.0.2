/*
 * SMGlobal.h
 *
 *  Created on: Nov 14, 2015
 *      Author: Debashis
 */

#ifndef INC_IPGLOBAL_H_
#define INC_IPGLOBAL_H_

#include <pthread.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <queue>
#include <array>
#include <stdlib.h>
#include <stdint.h>

#include "GConfig.h"
#include "SpectaTypedef.h"

using namespace std;

#define FLUSH_REQ_RSP			30
#define FLUSH_RSP_REQ			31
#define FLUSH_DUPLICATE			32
#define FLUSH_CLEANUP			33

#define MAX_INTERFACE_SUPPORT		16
#define MAX_ROUTER_PER_INTERFACE	8

#define MAX_TCP_SM_SUPPORT 	25
#define MAX_UDP_SM_SUPPORT 	25
#define MAX_DNS_SM_SUPPORT 	15
#define MAX_AAA_SM_SUPPORT 	5
#define	MAX_UNM_SM_SUPPORT 	4

#define MAX_PEERING_IP	2000

/* 0.3 Million Sessions / Session Manager */
#define TCP_SESSION_POOL_ARRAY_ELEMENTS		100
#define TCP_SESSION_POOL_ARRAY_SIZE			1000

#define UDP_SESSION_POOL_ARRAY_ELEMENTS		100
#define UDP_SESSION_POOL_ARRAY_SIZE			1000

#define DNS_SESSION_POOL_ARRAY_ELEMENTS		100
#define DNS_SESSION_POOL_ARRAY_SIZE			1000

#define AAA_SESSION_POOL_ARRAY_ELEMENTS		100
#define AAA_SESSION_POOL_ARRAY_SIZE			1000

#define UNM_SESSION_POOL_ARRAY_ELEMENTS		100
#define UNM_SESSION_POOL_ARRAY_SIZE			1000

#define MAX_FLUSHER_SUPPORT					8
#define MAX_UNM_FLUSHER_SUPPORT				2

//#define IP_POOL_ARRAY_ELEMENTS				100		//Poosible values 10, 100, 1000, 10000, 100000....
//#define IP_FLUSH_POOL_ARRAY_ELEMENTS		100		//Poosible values 10, 100, 1000, 10000, 100000....
//#define IP_FLUSH_POOL_ARRAY_SIZE			5000
//#define DNS_FLUSH_POOL_ARRAY_ELEMENTS		100		//Poosible values 10, 100, 1000, 10000, 100000....
//#define DNS_FLUSH_POOL_ARRAY_SIZE			3000


#define AAA_ACCESS_REQUEST			1
#define AAA_ACCESS_ACCEPT			2
#define AAA_ACCESS_REJECT			3

#define AAA_ACCOUNTING_REQUEST		4
#define AAA_ACCOUNTING_RESPONSE		5

#define	AAA_ACCOUNTING_START		1
#define	AAA_ACCOUNTING_STOP			2
#define AAA_ACCOUNTING_UPDATE		3

#define DNS_HDR_LEN			12

#define XDR_MAX_LEN					32000
#define VPS_MAX_LEN					14000
#define VPS_SINGLE_ELEMENT_SIZE		30
#define XDR_RECORDS_BATCH_SIZE		1000

#define SESSION_ID_LEN  	40		//sip_sp_dip_dp_appid
#define APN_LEN		 		50
#define URL_LEN		 		50
#define HTTP_AGENT_LEN		100
#define DESC_LEN			100

#define MAC_ADDR_LEN		18
#define IPV6_ADDR_LEN 		46
#define SESSION_KEY_LEN		100
#define DNS_RESOLVED_IP		100

#define AAA_USER_NAME_LEN	33
#define AAA_USER_ID_LEN		50
#define AAA_USER_OLT_LEN	100
#define AAA_USER_POLICY_LEN	50
#define AAA_USER_PLAN_LEN	50

#define XDR_ID_IP 			10
#define XDR_ID_DNS			12
#define XDR_ID_AAA 			30

#define FLUSH_TYPE_CLEANUP 			40

enum dnsResponse
{
	QUERY 		= 0,
	RESPONSE 	= 1,
	STATUS 		= 2,
	UNASSIGNED 	= 3,
	NOTIFY 		= 4,
	UPDATE 		= 5,
	SUCCESS		= 6
};

typedef enum
{
	SYSTEM_CLEANUP_TCP_CONN_DATA		= 10,
	SYSTEM_CLEANUP_TCP_CONN_NODATA		= 11,
	SYSTEM_CLEANUP_TCP_NOCONN_DATA		= 12,
	SYSTEM_CLEANUP_TCP_NOCONN_NODATA	= 13,
	SYSTEM_CLEANUP_UDP_DATA				= 14,
	SYSTEM_CLEANUP_LONG_SESSION			= 16,
	SYSTEM_CLEANUP_TCP_DATA				= 17,
	SYSTEM_CLEANUP_END_OF_DAY_TCP_DATA	= 18,
	SYSTEM_CLEANUP_END_OF_DAY_UDP_DATA	= 19,

	SESSION_TERM_TCP_FIN_RECEIVED		= 20,
	SESSION_TERM_TCP_CONN_NODATA		= 21,
	SESSION_TERM_TCP_NOCONN_DATA		= 22,
	SESSION_TERM_TCP_NOCONN_NODATA		= 23,
	SESSION_TERM_TCP_OVERWRITE			= 24,
	SESSION_TERM_DNS_QUERY_SUCCESS		= 25,


	SYSTEM_PKTLIMIT_TCP_CONN_DATA		= 30,
	SYSTEM_PKTLIMIT_TCP_NOCONN_DATA		= 31,
	SYSTEM_PKTLIMIT_UDP_DATA			= 32,

	SYSTEM_TIMEOUT_TCP_CONN_DATA		= 33,
	SYSTEM_TIMEOUT_TCP_NOCONN_DATA		= 34,
	SYSTEM_TIMEOUT_UDP_DATA				= 35,

	DUPLICATE_SYN						= 40,
	FIN_NO_SESSION						= 50,

	SYSTEM_CLEANUP_DNS_QUERY			= 99,

}causeCode;

typedef enum
{
	TCP_FIN	= 1,
	TCP_LIMIT = 2,
	TCP_CLEAN = 3
}flushType;

typedef enum
{
	UD_SYN_TSVAL = 1,
	UD_SYSACK_TSVAL,
	UD_SYN_LATENCY,
	UD_TCP_DATA,
	UD_TCP_DISCONN,
	UD_UDP_DATA,
	CR_TCP_SESSION,
	CR_UDP_SESSION,
	UD_HTTP_DATA_REQ,
	UD_HTTP_DATA_RSP,
	UP_TCP_DATA_SLICE,
	TCP_UNKNOWN_PACKET_TYPE
}tcp_udp_commands;

typedef enum
{
	SYN_RCV = 1,
	SYN_ACK_RCV,
	ACK_RCV,
	CONNECTED,
	DATA_RCV,
	FIN_RCV,
}TcpState;

typedef enum {
	DNS_PORT 		= 53,
	HTTP_PORT 		= 80,
	SYSLOG_PORT		= 514,
	HTTPS_PORT 		= 443,
	GTPU_PORT 		= 2152,
	GTPC_PORT 		= 2123,
	GTPC_PORT1 		= 3386,
	HTTP_PORT1 		= 8080,
	GX_PORT			= 3868,
	RADIUS_AUTH		= 1812,
	RADIUS_ACCO 	= 1813,
	RADIUS_AUTH1	= 31812,
	RADIUS_ACCO1 	= 31813
};

//typedef struct _dnsUrl
//{
//	uint64_t	pckLastTimeEpcohSec;
//	char 		URL[URL_LEN];
//
//	~_dnsUrl(){}
//
//	_dnsUrl()
//	{
//		pckLastTimeEpcohSec = 0;
//		URL[0] = 0;
//	}
//}dnsUrl;
//
//typedef struct _dnsV6Url
//{
//	uint64_t	pckLastTimeEpcohSec;
//	char 		URL[URL_LEN];
//	char 		address[IPV6_ADDR_LEN];
//
//	_dnsV6Url()
//	{
//		pckLastTimeEpcohSec = 0;
//		URL[0] = 0;
//		address[0] = 0;
//	}
//}dnsV6Url;

typedef struct _fData
{
	uint32_t totalVolume;
	uint32_t upPackets;
	uint32_t upVolume;
	uint32_t dnPackets;
	uint32_t dnVolume;

    _fData()
	{ reset(); }

    void reset()
    {
    	totalVolume = 0;
    	upPackets = 0;
    	upVolume = 0;
    	dnPackets = 0;
    	dnVolume = 0;
    }
}fData;

typedef struct _dupInfo
{
	uint16_t ipId;
	uint8_t  ttl;
	uint8_t	direction;
	_dupInfo()
	{ reset(); }

    void reset()
    {
    	ipId = 0;
    	ttl = 0;
    	direction = 0;
    }
}dupInfo;

typedef struct _dnsSession
{
	uint8_t		ipVer;
	uint8_t		errorCode;
	uint16_t	sPort;
	uint16_t	dPort;
	uint16_t	state;
	uint16_t	flushType;
	uint32_t	transactionId;
	uint32_t 	sIpv4;
	uint32_t 	dIpv4;
	uint32_t	causeCode;
	uint32_t	poolIndex;
	uint64_t 	queryStartEpochSec;
	uint64_t	queryEndEpochSec;
	uint64_t 	queryStartEpochNanoSec;
	uint64_t	queryEndEpochNanoSec;
	uint64_t	sessionV4Key;
	char		sIpv6[IPV6_ADDR_LEN];
	char		dIpv6[IPV6_ADDR_LEN];
	char 		url[URL_LEN];
	char 		errorDesc[DESC_LEN];
	string		sessionV6Key;

	_dnsSession()
	{ reset(); }

	void set(const _dnsSession *obj)
	{
		this->ipVer 		= obj->ipVer;
		this->errorCode 	= obj->errorCode;
		this->sPort 		= obj->sPort;
		this->dPort 		= obj->dPort;
		this->state			= obj->state;
		this->flushType 	= obj->flushType;
		this->transactionId = obj->transactionId;
		this->sIpv4 		= obj->sIpv4;
		this->dIpv4 		= obj->dIpv4;
		this->causeCode 	= obj->causeCode;
		this->poolIndex 	= obj->poolIndex;
		this->queryStartEpochSec 	= obj->queryStartEpochSec;
		this->queryEndEpochSec 		= obj->queryEndEpochSec;
		this->queryStartEpochNanoSec= obj->queryStartEpochNanoSec;
		this->queryEndEpochNanoSec 	= obj->queryEndEpochNanoSec;
		this->sessionV4Key 	= obj->sessionV4Key;
		strcpy(this->sIpv6, obj->sIpv6);
		strcpy(this->dIpv6, obj->dIpv6);
		strcpy(this->url, obj->url);
		strcpy(this->errorDesc, obj->errorDesc);
		this->sessionV6Key 	= obj->sessionV6Key;
	}

	void copy(const _dnsSession* obj)
	{
		this->ipVer 		= obj->ipVer;
		this->errorCode 	= obj->errorCode;
		this->sPort 		= obj->sPort;
		this->dPort 		= obj->dPort;
		this->state			= obj->state;
		this->flushType 	= obj->flushType;
		this->transactionId = obj->transactionId;
		this->sIpv4 		= obj->sIpv4;
		this->dIpv4 		= obj->dIpv4;
		this->causeCode 	= obj->causeCode;
		this->poolIndex 	= obj->poolIndex;
		this->queryStartEpochSec 	= obj->queryStartEpochSec;
		this->queryEndEpochSec 		= obj->queryEndEpochSec;
		this->queryStartEpochNanoSec= obj->queryStartEpochNanoSec;
		this->queryEndEpochNanoSec 	= obj->queryEndEpochNanoSec;
		this->sessionV4Key 	= obj->sessionV4Key;
		strcpy(this->sIpv6, obj->sIpv6);
		strcpy(this->dIpv6, obj->dIpv6);
		strcpy(this->url, obj->url);
		strcpy(this->errorDesc, obj->errorDesc);
		this->sessionV6Key 	= obj->sessionV6Key;
	}
	void reset()
	{
		this->ipVer 		= 0;
		this->errorCode 	= 0;
		this->sPort 		= 0;
		this->dPort 		= 0;
		this->state			= 0;
		this->flushType 	= 0;
		this->transactionId = 0;
		this->sIpv4 		= 0;
		this->dIpv4 		= 0;
		this->causeCode 	= 0;
		this->poolIndex 	= 0;
		this->queryStartEpochSec 	= 0;
		this->queryEndEpochSec 		= 0;
		this->queryStartEpochNanoSec= 0;
		this->queryEndEpochNanoSec 	= 0;
		this->sessionV4Key 	= 0;
		this->sIpv6[0]		= 0;
		this->dIpv6[0]		= 0;
		this->url[0]		= 0;
		this->errorDesc[0]	= 0;
		this->sessionV6Key.clear();
	}
}dnsSession;

typedef struct _tcpSession
{
    bool		synRcv;
    bool		synAckRcv;
    bool		ackRcv;
    bool		dataRcv;
    bool		finRcv;
    bool		firstDataFlag;
	bool		activeState;
    uint8_t 	TTL;
    uint8_t		ipVer;
    uint8_t		isUpDir;
	uint8_t		causeCode;
	uint8_t		pType;
	uint16_t	vLanId;
    uint16_t 	state;
    uint16_t 	sPort;
    uint16_t 	dPort;
    uint16_t 	pLoadPkt;
    uint16_t 	upPLoadPkt;
    uint16_t 	dnPLoadPkt;
    uint16_t 	totalFrCount;
    uint16_t 	frCount;
    uint16_t 	upFrCount;
    uint16_t 	dnFrCount;
    uint16_t 	sliceCounter;
    uint16_t 	pckTotalTimeSec;
    uint16_t	flushOrgId;
	uint16_t	smInstanceId;
    uint32_t	sIpv4;
    uint32_t	dIpv4;
    uint32_t	pLoadSize;
    uint32_t	upPLoadSize;
    uint32_t	dnPLoadSize;
    uint32_t	frSize;
    uint32_t	upFrSize;
    uint32_t	dnFrSize;
    uint32_t 	sessionTP;
    uint32_t	peakSessionTP;
    uint32_t 	upSessionTP;
    uint32_t 	dnSessionTP;
    uint32_t	upPeakSessionTP;
    uint32_t 	dnPeakSessionTP;
    uint32_t	mapIndex;
    uint32_t	poolIndex;
	uint32_t	reTransmissionCnt;
	uint32_t	layer3LoopCnt;
	uint32_t	duplicateCnt;
    uint64_t 	pckArivalTimeEpochSec;
    uint64_t 	pckLastTimeEpochSec;
    uint64_t 	pckLastTimeEpochNanoSec;
    uint64_t	startTimeEpochSec;
    uint64_t 	startTimeEpochNanoSec;
    uint64_t	endTimeEpochNanoSec;
	uint64_t	sessionIpV4Key;
	uint64_t	synTimeEpochNanoSec;
	uint64_t	synAckTimeEpochNanoSec;
	uint64_t	ackTimeEpochNanoSec;
	uint64_t	finTimeEpochNanoSec;
    uint64_t	firstDataTimeEpochNanoSec;
	uint64_t 	flushTime;
	uint64_t 	lastActivityTimeEpohSec;
    char		userId[IPV6_ADDR_LEN];
    char		httpAgent[HTTP_AGENT_LEN];

	std::map<uint64_t, fData> packTimeMap;		//EpochTimeSec & fData
	std::map<uint32_t, dupInfo> dupMap;

	~_tcpSession(){}

	_tcpSession()
	{ reset(); }

	void reset()
	{
	    this->synRcv		= false;
	    this->synAckRcv		= false;
	    this->ackRcv		= false;
	    this->dataRcv		= false;
	    this->finRcv		= false;
	    this->firstDataFlag	= false;
	    this->activeState	= false;
	    this->TTL			= 0;
	    this->ipVer			= 0;
	    this->isUpDir		= 0;
	    this->causeCode		= 0;
	    this->pType			= 0;
	    this->vLanId		= 0;
	    this->state			= 0;
	    this->sPort			= 0;
	    this->dPort			= 0;
	    this->pLoadPkt		= 0;
	    this->upPLoadPkt	= 0;
	    this->dnPLoadPkt	= 0;
	    this->totalFrCount	= 0;
	    this->frCount		= 0;
	    this->upFrCount		= 0;
	    this->dnFrCount		= 0;
	    this->sliceCounter	= 0;
	    this->pckTotalTimeSec	= 0;
	    this->flushOrgId		= 0;
	    this->smInstanceId	= 0;
	    this->sIpv4			= 0;
	    this->dIpv4			= 0;
	    this->pLoadSize		= 0;
	    this->upPLoadSize	= 0;
	    this->dnPLoadSize	= 0;
	    this->frSize		= 0;
	    this->upFrSize		= 0;
	    this->dnFrSize		= 0;
	    this->sessionTP		= 0;
	    this->peakSessionTP	= 0;
	    this->upSessionTP	= 0;
	    this->dnSessionTP	= 0;
	    this->upPeakSessionTP	= 0;
	    this->dnPeakSessionTP	= 0;
	    this->mapIndex		= 0;
	    this->poolIndex		= 0;
	    this->reTransmissionCnt = 0;
	    this->layer3LoopCnt	= 0;
	    this->duplicateCnt	= 0;
	    this->pckArivalTimeEpochSec 	= 0;
	    this->pckLastTimeEpochSec		= 0;
	    this->pckLastTimeEpochNanoSec	= 0;
	    this->startTimeEpochSec			= 0;
	    this->startTimeEpochNanoSec		= 0;
	    this->endTimeEpochNanoSec		= 0;
	    this->sessionIpV4Key			= 0;
	    this->synTimeEpochNanoSec		= 0;
	    this->synAckTimeEpochNanoSec	= 0;
	    this->ackTimeEpochNanoSec		= 0;
	    this->finTimeEpochNanoSec		= 0;
	    this->firstDataTimeEpochNanoSec = 0;
	    this->flushTime					= 0;
	    this->lastActivityTimeEpohSec	= 0;
	    this->userId[0] 	= 0;
	    this->httpAgent[0] 	= 0;

	    this->packTimeMap.clear();		//EpochTimeSec & fData
	    this->dupMap.clear();
	}

	void reuse()
	{
		this->packTimeMap.clear();
		this->dupMap.clear();

		this->totalFrCount 	= 0;
		this->frCount 		= 0;
		this->upFrCount 	= 0;
		this->dnFrCount 	= 0;

		this->frSize 		= 0;
		this->upFrSize 		= 0;
		this->dnFrSize 		= 0;

		this->pLoadPkt 		= 0;
		this->upPLoadPkt	= 0;
		this->dnPLoadPkt	= 0;

		this->pLoadSize 	= 0;
		this->upPLoadSize 	= 0;
		this->dnPLoadSize 	= 0;

		this->pckArivalTimeEpochSec = 0;
		this->startTimeEpochSec 	= pckLastTimeEpochSec;
		this->startTimeEpochNanoSec = pckLastTimeEpochNanoSec;
		this->endTimeEpochNanoSec 	= 0;

		this->pckTotalTimeSec 	= 0;
		this->sessionTP 		= 0;
		this->peakSessionTP 	= 0;
		this->upSessionTP 		= 0;
		this->upPeakSessionTP	= 0;
		this->dnSessionTP 		= 0;
		this->dnPeakSessionTP	= 0;

		this->pckLastTimeEpochSec 		= 0;
		this->pckLastTimeEpochNanoSec 	= 0;

		this->reTransmissionCnt = 0;
		this->layer3LoopCnt 	= 0;
		this->duplicateCnt 		= 0;
	}

	_tcpSession(const _tcpSession& obj)
	{
	    this->synRcv		= obj.synRcv;
	    this->synAckRcv		= obj.synAckRcv;
	    this->ackRcv		= obj.ackRcv;
	    this->dataRcv		= obj.dataRcv;
	    this->finRcv		= obj.finRcv;
	    this->firstDataFlag	= obj.firstDataFlag;
	    this->activeState	= obj.activeState;
	    this->TTL			= obj.TTL;
	    this->ipVer			= obj.ipVer;
	    this->isUpDir		= obj.isUpDir;
	    this->causeCode		= obj.causeCode;
	    this->pType			= obj.pType;
	    this->vLanId		= obj.vLanId;
	    this->state			= obj.state;
	    this->sPort			= obj.sPort;
	    this->dPort			= obj.dPort;
	    this->pLoadPkt		= obj.pLoadPkt;
	    this->upPLoadPkt	= obj.upPLoadPkt;
	    this->dnPLoadPkt	= obj.dnPLoadPkt;
	    this->totalFrCount	= obj.totalFrCount;
	    this->frCount		= obj.frCount;
	    this->upFrCount		= obj.upFrCount;
	    this->dnFrCount		= obj.dnFrCount;
	    this->sliceCounter	= obj.sliceCounter;
	    this->pckTotalTimeSec	= obj.pckTotalTimeSec;
	    this->flushOrgId		= obj.flushOrgId;
	    this->smInstanceId	= obj.smInstanceId;
	    this->sIpv4			= obj.sIpv4;
	    this->dIpv4			= obj.dIpv4;
	    this->pLoadSize		= obj.pLoadSize;
	    this->upPLoadSize	= obj.upPLoadSize;
	    this->dnPLoadSize	= obj.dnPLoadSize;
	    this->frSize		= obj.frSize;
	    this->upFrSize		= obj.upFrSize;
	    this->dnFrSize		= obj.dnFrSize;
	    this->sessionTP		= obj.sessionTP;
	    this->peakSessionTP	= obj.peakSessionTP;
	    this->upSessionTP	= obj.upSessionTP;
	    this->dnSessionTP	= obj.dnSessionTP;
	    this->upPeakSessionTP	= obj.upPeakSessionTP;
	    this->dnPeakSessionTP	= obj.dnPeakSessionTP;
	    this->mapIndex		= obj.mapIndex;
	    this->poolIndex		= obj.poolIndex;
	    this->reTransmissionCnt = obj.reTransmissionCnt;
	    this->layer3LoopCnt	= obj.layer3LoopCnt;
	    this->duplicateCnt	= obj.duplicateCnt;
	    this->pckArivalTimeEpochSec 	= obj.pckArivalTimeEpochSec;
	    this->pckLastTimeEpochSec		= obj.pckLastTimeEpochSec;
	    this->pckLastTimeEpochNanoSec	= obj.pckLastTimeEpochNanoSec;
	    this->startTimeEpochSec			= obj.startTimeEpochSec;
	    this->startTimeEpochNanoSec		= obj.startTimeEpochNanoSec;
	    this->endTimeEpochNanoSec		= obj.endTimeEpochNanoSec;
	    this->sessionIpV4Key			= obj.sessionIpV4Key;
	    this->synTimeEpochNanoSec		= obj.synTimeEpochNanoSec;
	    this->synAckTimeEpochNanoSec	= obj.synAckTimeEpochNanoSec;
	    this->ackTimeEpochNanoSec		= obj.ackTimeEpochNanoSec;
	    this->finTimeEpochNanoSec		= obj.finTimeEpochNanoSec;
	    this->firstDataTimeEpochNanoSec = obj.firstDataTimeEpochNanoSec;
	    this->flushTime					= obj.flushTime;
	    this->lastActivityTimeEpohSec	= obj.lastActivityTimeEpohSec;
	    strcpy(this->userId, obj.userId);
	    strcpy(this->httpAgent, obj.httpAgent);

	    this->packTimeMap 	= obj.packTimeMap;
	    this->dupMap 		= obj.dupMap;
	}

	void copy(const _tcpSession* obj)
	{
	    this->synRcv		= obj->synRcv;
	    this->synAckRcv		= obj->synAckRcv;
	    this->ackRcv		= obj->ackRcv;
	    this->dataRcv		= obj->dataRcv;
	    this->finRcv		= obj->finRcv;
	    this->firstDataFlag	= obj->firstDataFlag;
	    this->activeState	= obj->activeState;
	    this->TTL			= obj->TTL;
	    this->ipVer			= obj->ipVer;
	    this->isUpDir		= obj->isUpDir;
	    this->causeCode		= obj->causeCode;
	    this->pType			= obj->pType;
	    this->vLanId		= obj->vLanId;
	    this->state			= obj->state;
	    this->sPort			= obj->sPort;
	    this->dPort			= obj->dPort;
	    this->pLoadPkt		= obj->pLoadPkt;
	    this->upPLoadPkt	= obj->upPLoadPkt;
	    this->dnPLoadPkt	= obj->dnPLoadPkt;
	    this->totalFrCount	= obj->totalFrCount;
	    this->frCount		= obj->frCount;
	    this->upFrCount		= obj->upFrCount;
	    this->dnFrCount		= obj->dnFrCount;
	    this->sliceCounter	= obj->sliceCounter;
	    this->pckTotalTimeSec	= obj->pckTotalTimeSec;
	    this->flushOrgId		= obj->flushOrgId;
	    this->smInstanceId	= obj->smInstanceId;
	    this->sIpv4			= obj->sIpv4;
	    this->dIpv4			= obj->dIpv4;
	    this->pLoadSize		= obj->pLoadSize;
	    this->upPLoadSize	= obj->upPLoadSize;
	    this->dnPLoadSize	= obj->dnPLoadSize;
	    this->frSize		= obj->frSize;
	    this->upFrSize		= obj->upFrSize;
	    this->dnFrSize		= obj->dnFrSize;
	    this->sessionTP		= obj->sessionTP;
	    this->peakSessionTP	= obj->peakSessionTP;
	    this->upSessionTP	= obj->upSessionTP;
	    this->dnSessionTP	= obj->dnSessionTP;
	    this->upPeakSessionTP	= obj->upPeakSessionTP;
	    this->dnPeakSessionTP	= obj->dnPeakSessionTP;
	    this->mapIndex		= obj->mapIndex;
	    this->poolIndex		= obj->poolIndex;
	    this->reTransmissionCnt = obj->reTransmissionCnt;
	    this->layer3LoopCnt	= obj->layer3LoopCnt;
	    this->duplicateCnt	= obj->duplicateCnt;
	    this->pckArivalTimeEpochSec 	= obj->pckArivalTimeEpochSec;
	    this->pckLastTimeEpochSec		= obj->pckLastTimeEpochSec;
	    this->pckLastTimeEpochNanoSec	= obj->pckLastTimeEpochNanoSec;
	    this->startTimeEpochSec			= obj->startTimeEpochSec;
	    this->startTimeEpochNanoSec		= obj->startTimeEpochNanoSec;
	    this->endTimeEpochNanoSec		= obj->endTimeEpochNanoSec;
	    this->sessionIpV4Key			= obj->sessionIpV4Key;
	    this->synTimeEpochNanoSec		= obj->synTimeEpochNanoSec;
	    this->synAckTimeEpochNanoSec	= obj->synAckTimeEpochNanoSec;
	    this->ackTimeEpochNanoSec		= obj->ackTimeEpochNanoSec;
	    this->finTimeEpochNanoSec		= obj->finTimeEpochNanoSec;
	    this->firstDataTimeEpochNanoSec = obj->firstDataTimeEpochNanoSec;
	    this->flushTime					= obj->flushTime;
	    this->lastActivityTimeEpohSec	= obj->lastActivityTimeEpohSec;
	    strcpy(this->userId, obj->userId);
	    strcpy(this->httpAgent, obj->httpAgent);

	    this->packTimeMap 	= obj->packTimeMap;
	    this->dupMap 		= obj->dupMap;
	}
}tcpSession;

typedef struct _udpSession
{
    uint8_t		ipVer;
    uint8_t 	sliceCounter;
    uint8_t		isUpDir;
	uint8_t		causeCode;
	uint8_t		pType;
	uint16_t	vLanId;
    uint16_t 	state;
    uint16_t 	sPort;
    uint16_t 	dPort;
    uint16_t 	pLoadPkt;
    uint16_t 	upPLoadPkt;
    uint16_t 	dnPLoadPkt;
    uint16_t 	totalFrCount;
    uint16_t 	frCount;
    uint16_t 	upFrCount;
    uint16_t 	dnFrCount;
    uint16_t 	pckTotalTimeSec;
	uint16_t	smInstanceId;
	uint16_t	flushOrgId;
    uint32_t	sIpv4;
    uint32_t	dIpv4;
    uint32_t	pLoadSize;
    uint32_t	upPLoadSize;
    uint32_t	dnPLoadSize;
    uint32_t	frSize;
    uint32_t	upFrSize;
    uint32_t	dnFrSize;
    uint32_t 	sessionTP;
    uint32_t	peakSessionTP;
    uint32_t 	upSessionTP;
    uint32_t 	dnSessionTP;
    uint32_t	upPeakSessionTP;
    uint32_t 	dnPeakSessionTP;
	uint32_t	mapIndex;
	uint32_t	poolIndex;

    uint64_t 	pckArivalTimeEpochSec;
    uint64_t 	pckArivalTimeEpochNanoSec;

    uint64_t 	pckLastTimeEpochSec;
    uint64_t 	pckLastTimeEpochNanoSec;

    uint64_t	startTimeEpochSec;
    uint64_t 	startTimeEpochNanoSec;

    uint64_t	endTimeEpochNanoSec;

    uint64_t	sessionIpV4Key;
	uint64_t 	flushTime;
    char 		userId[IPV6_ADDR_LEN];
	std::map<uint64_t, fData> packTimeMap;		//EpochTimeSec & fData

	~_udpSession(){}

	_udpSession()
	{ reset(); }

	void reset()
	{
		this->ipVer 		= 0;
		this->sliceCounter 	= 0;
		this->isUpDir		= 0;
		this->causeCode		= 0;
		this->pType			= 0;
		this->vLanId		= 0;
		this->state			= 0;
		this->sPort			= 0;
		this->dPort			= 0;
		this->pLoadPkt		= 0;
		this->upPLoadPkt	= 0;
		this->dnPLoadPkt	= 0;
		this->totalFrCount	= 0;
		this->frCount		= 0;
		this->upFrCount		= 0;
		this->dnFrCount		= 0;
		this->pckTotalTimeSec	= 0;
		this->smInstanceId	= 0;
		this->flushOrgId	= 0;
		this->sIpv4			= 0;
		this->dIpv4			= 0;
		this->pLoadSize		= 0;
		this->upPLoadSize	= 0;
		this->dnPLoadSize	= 0;
		this->frSize		= 0;
		this->upFrSize		= 0;
		this->dnFrSize		= 0;
		this->sessionTP		= 0;
		this->peakSessionTP	= 0;
		this->upSessionTP	= 0;
		this->dnSessionTP	= 0;
	    this->upPeakSessionTP	= 0;
	    this->dnPeakSessionTP	= 0;
	    this->mapIndex		= 0;
	    this->poolIndex		= 0;

	    this->pckArivalTimeEpochSec		= 0;
	    this->pckArivalTimeEpochNanoSec	= 0;
	    this->pckLastTimeEpochSec		= 0;
	    this->pckLastTimeEpochNanoSec	= 0;
	    this->startTimeEpochSec			= 0;
	    this->startTimeEpochNanoSec		= 0;
	    this->endTimeEpochNanoSec		= 0;

	    this->sessionIpV4Key			= 0;
	    this->flushTime					= 0;
	    this->userId[0] 				= 0;
	    this->packTimeMap.clear();
	}

	void reuse()
	{
		this->packTimeMap.clear();

		this->totalFrCount 	= 0;
		this->frCount 		= 0;
		this->upFrCount 	= 0;
		this->dnFrCount 	= 0;

		this->frSize 		= 0;
		this->upFrSize 		= 0;
		this->dnFrSize 		= 0;

		this->pLoadPkt 		= 0;
		this->upPLoadPkt	= 0;
		this->dnPLoadPkt	= 0;

		this->pLoadSize 	= 0;
		this->upPLoadSize 	= 0;
		this->dnPLoadSize 	= 0;

		this->pckArivalTimeEpochSec		= 0;
		this->pckArivalTimeEpochNanoSec	= 0;
		this->startTimeEpochSec			= pckLastTimeEpochSec;
		this->startTimeEpochNanoSec		= pckLastTimeEpochNanoSec;
		this->pckLastTimeEpochSec		= 0;
		this->pckLastTimeEpochNanoSec	= 0;
		this->endTimeEpochNanoSec		= 0;


		this->pckTotalTimeSec 	= 0;
		this->sessionTP 		= 0;
		this->peakSessionTP 	= 0;

		this->upSessionTP 		= 0;
		this->upPeakSessionTP	= 0;
		this->dnSessionTP 		= 0;
		this->dnPeakSessionTP	= 0;

		this->pckLastTimeEpochSec 		= 0;
		this->pckLastTimeEpochNanoSec 	= 0;
	}

	_udpSession(const _udpSession& obj)
	{
		this->ipVer 		= obj.ipVer;
		this->sliceCounter 	= obj.sliceCounter;
		this->isUpDir		= obj.isUpDir;
		this->causeCode		= obj.causeCode;
		this->pType			= obj.pType;
		this->vLanId		= obj.vLanId;
		this->state			= obj.state;
		this->sPort			= obj.sPort;
		this->dPort			= obj.dPort;
		this->pLoadPkt		= obj.pLoadPkt;
		this->upPLoadPkt	= obj.upPLoadPkt;
		this->dnPLoadPkt	= obj.dnPLoadPkt;
		this->totalFrCount	= obj.totalFrCount;
		this->frCount		= obj.frCount;
		this->upFrCount		= obj.upFrCount;
		this->dnFrCount		= obj.dnFrCount;
		this->pckTotalTimeSec	= obj.pckTotalTimeSec;
		this->smInstanceId	= obj.smInstanceId;
		this->flushOrgId	= obj.flushOrgId;
		this->sIpv4			= obj.sIpv4;
		this->dIpv4			= obj.dIpv4;
		this->pLoadSize		= obj.pLoadSize;
		this->upPLoadSize	= obj.upPLoadSize;
		this->dnPLoadSize	= obj.dnPLoadSize;
		this->frSize		= obj.frSize;
		this->upFrSize		= obj.upFrSize;
		this->dnFrSize		= obj.dnFrSize;
		this->sessionTP		= obj.sessionTP;
		this->peakSessionTP	= obj.peakSessionTP;
		this->upSessionTP	= obj.upSessionTP;
		this->dnSessionTP	= obj.dnSessionTP;
	    this->upPeakSessionTP	= obj.upPeakSessionTP;
	    this->dnPeakSessionTP	= obj.dnPeakSessionTP;
	    this->mapIndex		= obj.mapIndex;
	    this->poolIndex		= obj.poolIndex;

	    this->pckArivalTimeEpochSec		= obj.pckArivalTimeEpochSec;
	    this->pckArivalTimeEpochNanoSec	= obj.pckArivalTimeEpochNanoSec;
	    this->pckLastTimeEpochSec		= obj.pckLastTimeEpochSec;
	    this->pckLastTimeEpochNanoSec	= obj.pckLastTimeEpochNanoSec;
	    this->startTimeEpochSec			= obj.startTimeEpochSec;
	    this->startTimeEpochNanoSec		= obj.startTimeEpochNanoSec;
	    this->endTimeEpochNanoSec		= obj.endTimeEpochNanoSec;

	    this->sessionIpV4Key			= obj.sessionIpV4Key;
	    this->flushTime					= obj.flushTime;
	    strcpy(this->userId, obj.userId);
	    this->packTimeMap				= obj.packTimeMap;
	}

	void copy(const _udpSession* obj)
	{
		this->ipVer 		= obj->ipVer;
		this->sliceCounter 	= obj->sliceCounter;
		this->isUpDir		= obj->isUpDir;
		this->causeCode		= obj->causeCode;
		this->pType			= obj->pType;
		this->vLanId		= obj->vLanId;
		this->state			= obj->state;
		this->sPort			= obj->sPort;
		this->dPort			= obj->dPort;
		this->pLoadPkt		= obj->pLoadPkt;
		this->upPLoadPkt	= obj->upPLoadPkt;
		this->dnPLoadPkt	= obj->dnPLoadPkt;
		this->totalFrCount	= obj->totalFrCount;
		this->frCount		= obj->frCount;
		this->upFrCount		= obj->upFrCount;
		this->dnFrCount		= obj->dnFrCount;
		this->pckTotalTimeSec	= obj->pckTotalTimeSec;
		this->smInstanceId	= obj->smInstanceId;
		this->flushOrgId	= obj->flushOrgId;
		this->sIpv4			= obj->sIpv4;
		this->dIpv4			= obj->dIpv4;
		this->pLoadSize		= obj->pLoadSize;
		this->upPLoadSize	= obj->upPLoadSize;
		this->dnPLoadSize	= obj->dnPLoadSize;
		this->frSize		= obj->frSize;
		this->upFrSize		= obj->upFrSize;
		this->dnFrSize		= obj->dnFrSize;
		this->sessionTP		= obj->sessionTP;
		this->peakSessionTP	= obj->peakSessionTP;
		this->upSessionTP	= obj->upSessionTP;
		this->dnSessionTP	= obj->dnSessionTP;
	    this->upPeakSessionTP	= obj->upPeakSessionTP;
	    this->dnPeakSessionTP	= obj->dnPeakSessionTP;
	    this->mapIndex		= obj->mapIndex;
	    this->poolIndex		= obj->poolIndex;

	    this->pckArivalTimeEpochSec		= obj->pckArivalTimeEpochSec;
	    this->pckArivalTimeEpochNanoSec	= obj->pckArivalTimeEpochNanoSec;
	    this->pckLastTimeEpochSec		= obj->pckLastTimeEpochSec;
	    this->pckLastTimeEpochNanoSec	= obj->pckLastTimeEpochNanoSec;
	    this->startTimeEpochSec			= obj->startTimeEpochSec;
	    this->startTimeEpochNanoSec		= obj->startTimeEpochNanoSec;
	    this->endTimeEpochNanoSec		= obj->endTimeEpochNanoSec;

	    this->sessionIpV4Key			= obj->sessionIpV4Key;
	    this->flushTime					= obj->flushTime;
	    strcpy(this->userId, obj->userId);
	    this->packTimeMap				= obj->packTimeMap;
	}
}udpSession;

typedef struct _aaaSession
{
	uint8_t 	ipVer;
	int16_t		nasPortType;
	uint16_t	appPort;
	uint16_t 	sPort;
	uint16_t 	dPort;
	uint16_t 	reqCode;
	uint16_t 	respCode;
	uint16_t	packetIdentifier;
	uint16_t	mapIndex;
	uint16_t	flushType;
	uint32_t	accStatusType;
	uint32_t	serviceType;
	uint32_t	protocol;
	uint32_t 	sourceAddr;
	uint32_t	accTerminationCause;
	uint32_t 	destAddr;
	uint32_t	accAuth;
	uint32_t	framedIpLong;
	uint32_t	inputOctets;
	uint32_t	outputOctets;
	uint32_t	inputPackets;
	uint32_t	outputPackets;
	uint32_t	inputGigaWords;
	uint32_t	outputGigaWords;
	uint64_t 	StartTimeEpochMiliSec;
	uint64_t 	EndTimeEpochMiliSec;
	uint64_t	StartTimeEpochSec;
	uint64_t	EndTimeEpochSec;
	uint64_t	aaaKey;
	uint64_t 	flushTime;
	char 		sourceMacAddr[MAC_ADDR_LEN];
	char 		destMacAddr[MAC_ADDR_LEN];
	char		userName[AAA_USER_NAME_LEN];
	char		nasIP[16];
	char 		callingStationId[50];
	char		nasIdentifier[35];
	char		replyMsg[35];
	char		userMac[MAC_ADDR_LEN];
	char		userIpV6[IPV6_ADDR_LEN];
	bool		ipv6AddressPrefixFlag;

	~_aaaSession(){}

	_aaaSession()
	{ reset(); }

	void reset()
	{
		StartTimeEpochMiliSec = 0;
		EndTimeEpochMiliSec = 0;
		StartTimeEpochSec	= 0;
		EndTimeEpochSec		= 0;
		sourceMacAddr[0] = 0;
		destMacAddr[0] = 0;
		appPort = 0;
		sourceAddr = 0;
		destAddr = 0;
		sPort = 0;
		dPort = 0;
		ipVer = 0;
		reqCode = 0;
		respCode = 0;
		packetIdentifier = 0;

		protocol = 0;
		nasPortType = -1;
		serviceType = 0;
		accStatusType = 0;
		accTerminationCause = 0;
		aaaKey = 0;
		accAuth = 0;
		mapIndex = 0;
		flushTime = 0;
		flushType = 0;

		userName[0] = 0;
		framedIpLong = 0;
		strcpy(nasIP, "NA");

		strcpy(callingStationId, "NA");
		strcpy(nasIdentifier, "NA");
		replyMsg[0] = 0;
		userMac[0] = 0;
		strcpy(userIpV6, "NA");
		ipv6AddressPrefixFlag = false;
		inputOctets	= 0;
		outputOctets	= 0;
		inputPackets	= 0;
		outputPackets	= 0;
		inputGigaWords = 0;
		outputGigaWords = 0;
	}

	_aaaSession(const _aaaSession& obj)
	{
		this->StartTimeEpochMiliSec 	= obj.StartTimeEpochMiliSec;
		this->EndTimeEpochMiliSec 		= obj.EndTimeEpochMiliSec;
		this->StartTimeEpochSec			= obj.StartTimeEpochSec;
		this->EndTimeEpochSec			= obj.EndTimeEpochSec;
		strcpy(this->sourceMacAddr, obj.sourceMacAddr);
		strcpy(this->destMacAddr, obj.destMacAddr);
		this->appPort					= obj.appPort;
		this->sourceAddr 				= obj.sourceAddr;
		this->destAddr 					= obj.destAddr;
		this->sPort 				= obj.sPort;
		this->dPort 					= obj.dPort;
		this->ipVer						= obj.ipVer;
		this->reqCode					= obj.reqCode;
		this->respCode					= obj.respCode;

		this->packetIdentifier 			= obj.packetIdentifier;

		this->protocol 					= obj.protocol;
		this->nasPortType 				= obj.nasPortType;
		this->serviceType 				= obj.serviceType;
		this->accStatusType 			= obj.accStatusType;
		this->accTerminationCause 		= obj.accTerminationCause;
		this->aaaKey 			= obj.aaaKey;
		this->accAuth 					= obj.accAuth;
		this->mapIndex 					= obj.mapIndex;
		this->flushTime					= obj.flushTime;
		this->flushType					= obj.flushType;
		strcpy(this->userName, obj.userName);
		this->framedIpLong				= obj.framedIpLong;

		strcpy(this->nasIP, obj.nasIP);
		strcpy(this->callingStationId, obj.callingStationId);
		strcpy(this->nasIdentifier, obj.nasIdentifier);
		strcpy(this->replyMsg, obj.replyMsg);
		strcpy(this->userMac, obj.userMac);
		strcpy(this->userIpV6, obj.userIpV6);
		this->ipv6AddressPrefixFlag = obj.ipv6AddressPrefixFlag;
		this->inputOctets 	= obj.inputOctets;
		this->outputOctets	= obj.outputOctets;
		this->inputPackets  = obj.inputPackets;
		this->outputPackets = obj.outputPackets;
		this->inputGigaWords 	= obj.inputGigaWords;
		this->outputGigaWords	= obj.outputGigaWords;
	}

	void copy(const _aaaSession* obj)
	{
		this->StartTimeEpochMiliSec 	= obj->StartTimeEpochMiliSec;
		this->EndTimeEpochMiliSec 		= obj->EndTimeEpochMiliSec;
		this->StartTimeEpochSec			= obj->StartTimeEpochSec;
		this->EndTimeEpochSec			= obj->EndTimeEpochSec;

		strcpy(this->sourceMacAddr, obj->sourceMacAddr);
		strcpy(this->destMacAddr, obj->destMacAddr);
		this->appPort					= obj->appPort;
		this->sourceAddr 				= obj->sourceAddr;
		this->destAddr 					= obj->destAddr;
		this->sPort 				= obj->sPort;
		this->dPort 					= obj->dPort;
		this->ipVer						= obj->ipVer;
		this->reqCode					= obj->reqCode;
		this->respCode					= obj->respCode;

		this->packetIdentifier 			= obj->packetIdentifier;

		this->protocol 					= obj->protocol;
		this->nasPortType 				= obj->nasPortType;
		this->serviceType 				= obj->serviceType;
		this->accStatusType 			= obj->accStatusType;
		this->accTerminationCause 		= obj->accTerminationCause;
		this->aaaKey 			= obj->aaaKey;
		this->accAuth 					= obj->accAuth;
		this->mapIndex 					= obj->mapIndex;
		this->flushTime					= obj->flushTime;
		this->flushType					= obj->flushType;
		strcpy(this->userName, obj->userName);
		this->framedIpLong				= obj->framedIpLong;
		strcpy(this->nasIP, obj->nasIP);
		strcpy(this->callingStationId, obj->callingStationId);
		strcpy(this->nasIdentifier, obj->nasIdentifier);
		strcpy(this->replyMsg, obj->replyMsg);
		strcpy(this->userMac, obj->userMac);
		strcpy(this->userIpV6, obj->userIpV6);
		this->ipv6AddressPrefixFlag = obj->ipv6AddressPrefixFlag;
		this->inputOctets 	= obj->inputOctets;
		this->outputOctets	= obj->outputOctets;
		this->inputPackets  = obj->inputPackets;
		this->outputPackets = obj->outputPackets;
		this->inputGigaWords 	= obj->inputGigaWords;
		this->outputGigaWords	= obj->outputGigaWords;
	}
}aaaSession;

//typedef struct _xdrStore
//{
//	int idx;
//	char xdr[XDR_MAX_LEN];
//
//	_xdrStore()
//	{
//		reset();
//	}
//	_xdrStore(const _xdrStore& obj)
//	{
//		this->idx = obj.idx;
//		strcpy(this->xdr, obj.xdr);
//	}
//	void set(const _xdrStore *obj)
//	{
//		this->idx = obj->idx;
//		strcpy(this->xdr, obj->xdr);
//	}
//	void copy(const _xdrStore* obj)
//	{
//		this->idx = obj->idx;
//		strcpy(this->xdr, obj->xdr);
//	}
//	void operator=(const _xdrStore& obj)
//	{
//		this->idx = obj.idx;
//		strcpy(this->xdr, obj.xdr);
//	}
//	void reset()
//	{
//		this->idx = 0;
//		strcpy(this->xdr, "");
//	}
//}xdrStore;

//typedef struct
//{
//	int clientId;
//	int sock;
//	struct sockaddr address;
//	socklen_t addr_len;
//}connection_t;

namespace initSection
{
	extern std::map<uint8_t, std::string> protocolName;
	extern std::map<uint16_t, std::string> dnsErrorCode;
	extern std::map<uint16_t, std::string> tcpPorts;
	extern std::map<uint32_t, std::string> radiusCodeMap;
	extern std::map<uint32_t, std::string> serviceTypeMap;
	extern std::map<uint32_t, std::string> framedProtocolMap;
	extern std::map<uint32_t, std::string> acctAuthenticMap;
	extern std::map<uint32_t, std::string> acctTeminateMap;
	extern std::map<uint32_t, std::string> acctStatusMap;
	extern std::map<uint32_t, std::string> nasPortTypeMap;
	extern std::map<uint32_t, std::string> ipSubNetMap;
}

namespace SMGlobal
{
	/* ** Interface Settings ** */
	extern uint16_t		NO_OF_INTERFACE_SUPPORT;
	extern string		INTERFACE_NAME[MAX_INTERFACE_SUPPORT];

	/* ** Agents Setting ** */
	extern uint64_t		TCP_PACKET_PER_DAY[MAX_INTERFACE_SUPPORT];
	extern uint64_t		UDP_PACKET_PER_DAY[MAX_INTERFACE_SUPPORT];
	extern uint64_t		AAA_PACKET_PER_DAY[MAX_INTERFACE_SUPPORT];
	extern uint64_t		DNS_PACKET_PER_DAY[MAX_INTERFACE_SUPPORT];

	extern string 		TCP_SERVER_ADDRESS[MAX_INTERFACE_SUPPORT];
	extern string 		UDP_SERVER_ADDRESS[MAX_INTERFACE_SUPPORT];
	extern string 		AAA_SERVER_ADDRESS[MAX_INTERFACE_SUPPORT];
	extern string 		DNS_SERVER_ADDRESS[MAX_INTERFACE_SUPPORT];

	extern uint32_t 	TCP_PPS_PER_INTERFACE[MAX_INTERFACE_SUPPORT];
	extern uint32_t 	UDP_PPS_PER_INTERFACE[MAX_INTERFACE_SUPPORT];
	extern uint32_t 	AAA_PPS_PER_INTERFACE[MAX_INTERFACE_SUPPORT];
	extern uint32_t 	DNS_PPS_PER_INTERFACE[MAX_INTERFACE_SUPPORT];

	extern bool			TCP_AGENT_RUNNING_STATUS[MAX_INTERFACE_SUPPORT];
	extern bool			UDP_AGENT_RUNNING_STATUS[MAX_INTERFACE_SUPPORT];
	extern bool			AAA_AGENT_RUNNING_STATUS[MAX_INTERFACE_SUPPORT];
	extern bool			DNS_AGENT_RUNNING_STATUS[MAX_INTERFACE_SUPPORT];

	extern uint16_t 	TCP_AGENT_CORE[MAX_INTERFACE_SUPPORT];
	extern uint16_t 	UDP_AGENT_CORE[MAX_INTERFACE_SUPPORT];
	extern uint16_t 	AAA_AGENT_CORE[MAX_INTERFACE_SUPPORT];
	extern uint16_t 	DNS_AGENT_CORE[MAX_INTERFACE_SUPPORT];

	/* ** Packet Received by SM ** */
	extern uint64_t		TCP_SM_PKT_RECEIVED[MAX_TCP_SM_SUPPORT];
	extern uint64_t		UDP_SM_PKT_RECEIVED[MAX_UDP_SM_SUPPORT];
	extern uint64_t		DNS_SM_PKT_RECEIVED[MAX_DNS_SM_SUPPORT];
	extern uint64_t		AAA_SM_PKT_RECEIVED[MAX_AAA_SM_SUPPORT];
	extern uint64_t		UNM_SM_PKT_RECEIVED[MAX_UNM_SM_SUPPORT];

	/* ** Router Settings ** */
	extern bool			ROUTER_RUNNING_STATUS[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE];
	extern uint16_t 	ROUTER_PER_INTERFACE[MAX_INTERFACE_SUPPORT];
	extern uint16_t		ROUTER_CORE[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE];

	/* ** Session Manager Settings ** */
	extern bool			TCP_SM_RUNNING_STATUS[MAX_TCP_SM_SUPPORT];
	extern bool			UDP_SM_RUNNING_STATUS[MAX_UDP_SM_SUPPORT];
	extern bool			DNS_SM_RUNNING_STATUS[MAX_DNS_SM_SUPPORT];
	extern bool			AAA_SM_RUNNING_STATUS[MAX_AAA_SM_SUPPORT];
	extern bool			UNM_SM_RUNNING_STATUS[MAX_UNM_SM_SUPPORT];

	extern uint16_t		TCP_SM_INSTANCES;
	extern uint16_t		UDP_SM_INSTANCES;
	extern uint16_t		DNS_SM_INSTANCES;
	extern uint16_t		AAA_SM_INSTANCES;
	extern uint16_t		UNM_SM_INSTANCES;

	extern uint16_t 	TCP_SM_CORE[MAX_TCP_SM_SUPPORT];
	extern uint16_t 	UDP_SM_CORE[MAX_UDP_SM_SUPPORT];
	extern uint16_t 	DNS_SM_CORE[MAX_DNS_SM_SUPPORT];
	extern uint16_t 	AAA_SM_CORE[MAX_AAA_SM_SUPPORT];
	extern uint16_t 	UNM_SM_CORE[MAX_UNM_SM_SUPPORT];

	/* ** Flusher Settings ** */
	extern bool			TCP_FLUSHER_RUNNING_STATUS[MAX_FLUSHER_SUPPORT];
	extern bool			UDP_FLUSHER_RUNNING_STATUS[MAX_FLUSHER_SUPPORT];
	extern bool			DNS_FLUSHER_RUNNING_STATUS[MAX_FLUSHER_SUPPORT];
	extern bool			AAA_FLUSHER_RUNNING_STATUS[MAX_FLUSHER_SUPPORT];
	extern bool			UNM_FLUSHER_RUNNING_STATUS;

	extern uint16_t 	TCP_FLUSHER_INSTANCE;
	extern uint16_t 	UDP_FLUSHER_INSTANCE;
	extern uint16_t 	DNS_FLUSHER_INSTANCE;
	extern uint16_t 	AAA_FLUSHER_INSTANCE;
	extern uint16_t 	UNM_FLUSHER_INSTANCE;

	extern uint16_t 	TCP_FLUSHER_CORE[MAX_FLUSHER_SUPPORT];
	extern uint16_t 	UDP_FLUSHER_CORE[MAX_FLUSHER_SUPPORT];
	extern uint16_t 	DNS_FLUSHER_CORE[MAX_FLUSHER_SUPPORT];
	extern uint16_t 	AAA_FLUSHER_CORE[MAX_FLUSHER_SUPPORT];
	extern uint16_t 	UNM_FLUSHER_CORE;

	/* ** XDR File Name Prefix ** */
	extern string  		TCP_FILE_PREFIX;
	extern string  		UDP_FILE_PREFIX;
	extern string  		DNS_FILE_PREFIX;
	extern string  		AAA_FILE_PREFIX;

	/* ** Cleanup Settings ** */
	extern uint16_t		TCP_CLEAN_UP_TIMEOUT_SEC;
	extern uint16_t		UDP_CLEAN_UP_TIMEOUT_SEC;
	extern uint16_t		DNS_CLEAN_UP_TIMEOUT_SEC;
	extern uint16_t		AAA_CLEAN_UP_TIMEOUT_SEC;

	extern uint16_t 	SESSION_SCAN_FREQ_SEC;

	/* ** End of the Day Settings ** */
	extern uint16_t		DAY_CLEAN_HOUR;
	extern uint16_t		DAY_CLEAN_MIN;
	extern uint16_t		DAY_CLEAN_SEC;

	/* ** Threads Running Settings ** */
	extern bool			SERVER_RUNNING_STATUS;
	extern bool			STATSLOG_RUNNING_STATUS;
	extern bool			LOG_RUNNING_STATUS;
	extern bool			TIMER_RUNNING_STATUS;

	extern uint16_t		THREAD_SLEEP_TIME;
	extern uint16_t		TIMER_THREAD_CORE;

	/* ** Directories Settings ** */
	extern string		LOG_DIR;
	extern string		XDR_DIR;
	extern string		DATA_DIR[5];
	extern string		UNM_DIR;

	/* ** Admin Settings ** */
	extern bool			ADMIN_FLAG;
	extern string		ADMIN_PORT;

	/* ** TCP Settings ** */
	extern uint16_t		VPS_PACKET_PER_SEC;
	extern uint16_t		SESSION_PKT_LIMIT;
	extern uint16_t 	SESSION_TIME_LIMIT;
	extern bool			CHECK_DUPLICATE_PKT;
	extern bool			PROCESS_ACK;
	extern bool			ACK_CREATE_SESSION;

	/* ** UDP Settings ** */
	extern bool 		UDP_XDR_FOR_DNS;
	extern uint16_t		DNS_DUMP_HOUR;

	/* ** Log Settings ** */
	extern bool			PRINT_STATS;
	extern uint16_t		PROBE_ID;
	extern uint16_t		INSTANCE_ID;
	extern uint16_t		LOG_LEVEL;
	extern uint16_t 	PRINT_STATS_FREQ_SEC;
	extern uint16_t		LOG_STATS_FREQ_SEC;

	extern uint16_t		DNS_LOAD_FREQ_SEC;
	extern uint16_t		AAA_LOAD_FREQ_SEC;
	extern uint16_t		AAA_DUMP_FREQ_SEC;

	extern 	uint32_t	PEERING_DECIX[MAX_PEERING_IP][2];
	extern 	uint16_t	PEERING_DECIX_COUNT;

	extern	uint32_t	PEERING_I3DNET[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_I3DNET_COUNT;

	extern	uint32_t	PEERING_VERISIGN[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_VERISIGN_COUNT;

	extern	uint32_t	PEERING_SUBSPACE[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_SUBSPACE_COUNT;

	extern	uint32_t	PEERING_EDGECAST[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_EDGECAST_COUNT;

	extern	uint32_t	PEERING_FASTLY[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_FASTLY_COUNT;

	extern	uint32_t	PEERING_PKT_CLEARING_HOUSE[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_PKT_CLEARING_HOUSE_COUNT;

	extern	uint32_t	PEERING_HUAWEI_CLOUD[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_HUAWEI_CLOUD_COUNT;

	extern	uint32_t	PEERING_TENCENT[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_TENCENT_COUNT;

	extern	uint32_t	PEERING_ALIBABA[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_ALIBABA_COUNT;

	extern	uint32_t	PEERING_QUANTIL[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_QUANTIL_COUNT;

	extern	uint32_t	PEERING_MICROSOFT[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_MICROSOFT_COUNT;

	extern	uint32_t	PEERING_LIMELIGHT[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_LIMELIGHT_COUNT;

	extern	uint32_t	PEERING_ZENLAYER[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_ZENLAYER_COUNT;

	extern	uint32_t	PEERING_CLOUDFLARE[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_CLOUDFLARE_COUNT;

	extern	uint32_t	PEERING_NETFLIX[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_NETFLIX_COUNT;

	extern	uint32_t	PEERING_AKAMAI[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_AKAMAI_COUNT;

	extern	uint32_t	PEERING_GOOGLE[MAX_PEERING_IP][2];
	extern	uint16_t 	PEERING_GOOGLE_COUNT;

	extern	uint32_t	PEERING_FACEBOOK[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_FACEBOOK_COUNT;

	extern	uint32_t	PEERING_AMAZON[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_AMAZON_COUNT;

	extern	uint32_t	UNKNOWN_SOURCE_POOL[MAX_PEERING_IP][2];
	extern	uint16_t	UNKNOWN_SOURCE_POOL_COUNT;

	extern uint32_t		CACHE_IP_POOL[MAX_PEERING_IP][2];
	extern uint16_t		CACHE_IP_POOL_COUNT;
}

namespace IPStats
{
	extern uint64_t dnsLookupMapSize;

	extern uint32_t tcpV4SessionScanned[MAX_TCP_SM_SUPPORT];
	extern uint32_t tcpV4SessionCleaned[MAX_TCP_SM_SUPPORT];
	extern uint32_t tcpV4SessionTotalCnt[MAX_TCP_SM_SUPPORT];

	extern uint32_t tcpV6SessionScanned[MAX_TCP_SM_SUPPORT];
	extern uint32_t tcpV6SessionCleaned[MAX_TCP_SM_SUPPORT];
	extern uint32_t tcpV6SessionTotalCnt[MAX_TCP_SM_SUPPORT];

	extern uint32_t udpV4SessionScanned[MAX_UDP_SM_SUPPORT];
	extern uint32_t udpV4SessionCleaned[MAX_UDP_SM_SUPPORT];
	extern uint32_t udpV4SessionTotalCnt[MAX_UDP_SM_SUPPORT];

	extern uint32_t udpV6SessionScanned[MAX_UDP_SM_SUPPORT];
	extern uint32_t udpV6SessionCleaned[MAX_UDP_SM_SUPPORT];
	extern uint32_t udpV6SessionTotalCnt[MAX_UDP_SM_SUPPORT];

	extern uint32_t dnsV4SessionScanned[MAX_DNS_SM_SUPPORT];
	extern uint32_t dnsV4SessionCleaned[MAX_DNS_SM_SUPPORT];
	extern uint32_t dnsV4SessionTotalCnt[MAX_DNS_SM_SUPPORT];

	extern uint32_t dnsV6SessionScanned[MAX_DNS_SM_SUPPORT];
	extern uint32_t dnsV6SessionCleaned[MAX_DNS_SM_SUPPORT];
	extern uint32_t dnsV6SessionTotalCnt[MAX_DNS_SM_SUPPORT];

	extern uint32_t unTcpSessionCnt[MAX_UNM_SM_SUPPORT];
	extern uint32_t unTcpSessionScanned[MAX_UNM_SM_SUPPORT];
	extern uint32_t unTcpSessionCleaned[MAX_UNM_SM_SUPPORT];

	extern uint32_t unUdpSessionCnt[MAX_UNM_SM_SUPPORT];
	extern uint32_t unUdpSessionScanned[MAX_UNM_SM_SUPPORT];
	extern uint32_t unUdpSessionCleaned[MAX_UNM_SM_SUPPORT];

	extern uint32_t aaaAccessSessionCnt[MAX_AAA_SM_SUPPORT];
	extern uint32_t aaaAccessSessionScanned[MAX_AAA_SM_SUPPORT];
	extern uint32_t aaaAccessSessionCleaned[MAX_AAA_SM_SUPPORT];

	extern uint32_t aaaAccounSessionCnt[MAX_AAA_SM_SUPPORT];
	extern uint32_t aaaAccounSessionScanned[MAX_AAA_SM_SUPPORT];
	extern uint32_t aaaAccounSessionCleaned[MAX_AAA_SM_SUPPORT];
}

namespace DNSGlobal
{
	extern std::map<uint32_t, std::string> dnsLookUpMap[10];
	extern std::map<std::string, std::string> dnsV6LookUpMap;
}

typedef struct _userInfo
{
	char 		userName[IPV6_ADDR_LEN];					/* User Name */
	uint32_t	allocatedIpLong;						/* User Ip Long */
	uint32_t	oldAllocatedIpLong;						/* User Ip Long */

	~_userInfo(){};

	_userInfo()
	{ reset(); }

	void reset()
	{
		userName[0]  = 0;
		allocatedIpLong = 0;
		oldAllocatedIpLong = 0;
    }
}userInfo;

namespace aaaGlbMap
{
	extern std::map<uint32_t, userInfo> aaaGlbUserIpMap;	/* 01295072520@airtelbroadband.in */
	extern std::map<string, userInfo> aaaGlbUserIdMap;
	extern std::map<std::string, userInfo> aaaGlbIpv6UserMap;
}

namespace mapDnsLock
{
	extern pthread_mutex_t lockCount;
	extern pthread_cond_t nonzero;
	extern unsigned count;
}

namespace mapAAALock
{
	extern pthread_mutex_t lockCount;
	extern pthread_cond_t nonzero;
	extern unsigned count;
}

class SmGlobal
{
	public:
		SmGlobal();
		~SmGlobal();
};

#endif /* INC_IPGLOBAL_H_ */
