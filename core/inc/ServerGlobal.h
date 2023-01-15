/*
 * ServerGlobal.h
 *
 *  Created on: 29 Sep 2021
 *      Author: Debashis
 */

#ifndef CORE_SRC_SERVERGLOBAL_H_
#define CORE_SRC_SERVERGLOBAL_H_

#include <map>
#include <unordered_map>


#include "SpectaTypedef.h"
#include "SmGlobal.h"

using namespace std;

#define UP				1
#define DOWN			2
#define UNMAPPED		3

#define PKT_WRITE_TIME_INDEX(epochsec, ti) ( ((epochsec % ti) + 1) >= ti ? 0 : ((epochsec % ti) + 1) )
#define READ_TIME_INDEX(epochsec, ti) ( epochsec % ti )
#define READ_NEXT_TIME_INDEX(idx, ti) ( (idx+1) >= ti ? 0 : (idx+1) )


typedef enum
{
	PACKET_IPPROTO_IP 		= 0,	/** Dummy protocol for TCP		*/
	PACKET_IPPROTO_HOPOPTS 	= 0,	/** IPv6 Hop-by-Hop options		*/
	PACKET_IPPROTO_ICMP 	= 1,	/** Internet Control Message Protocol */
	PACKET_IPPROTO_IGMP 	= 2,	/** Internet Group management Protocol */
	PACKET_IPPROTO_IPIP 	= 4,	/** IPIP tunnels (older KA9Q tunnels use 94) */
	PACKET_IPPROTO_TCP		= 6,	/** Transmission Control Protocol	*/
	PACLET_IPPROTO_EGP 		= 8,	/** Exterior Gateway Protocol */
	PACKET_IPPROTO_PUP 		= 12,	/** PUP Protocol */
	PACKET_IPPROTO_UDP 		= 17,	/** User Datagram Protocol		*/
	PACKET_IPPROTO_DNS 		= 18,	/** DNS		*/
	PACKET_IPPROTO_IDP 		= 22,	/** XNS IDP protocol */
	PACKET_IPPROTO_TP 		= 29,	/** SO Transport Protocol Class 4. */
	PACKET_IPPROTO_DCCP 	= 33,	/** Datagram Congestion Control Protocol. */
	PACKET_IPPROTO_IPV6 	= 41,	/** IPv6 header */
	PACKET_IPPROTO_ROUTING 	= 43,	/** IPv6 Routing header */
	PACKET_IPPROTO_FRAGMENT = 44,	/** IPv6 fragmentation header */
	PACKET_IPPROTO_RSVP 	= 46,	/** Reservation Protocol */
	PACKET_IPPROTO_GRE 		= 47,	/** General Routing Encapsulation */
	PACKET_IPPROTO_GTPU 	= 48,	/** GTPU Protocol		*/
	PACKET_IPPROTO_GTPC 	= 49,	/** GTPC Protocol		*/
	PACKET_IPPROTO_ESP 		= 50,	/** encapsulating security Payload */
	PACKET_IPPROTO_AH 		= 51,	/** Authentication header */
	PACKET_IPPROTO_GX 		= 52,	/** GTPU Protocol		*/
	PACKET_IPPROTO_RADIUS 	= 53,	/** RADIUS Protocol		*/
	PACKET_IPPROTO_ICMPV6 	= 58,	/** ICMPV6 */
	PACKET_IPPROTO_NONE 	= 59,	/** IPv6 no next header */
	PACKET_IPPROTO_DSTOPTS 	= 60,	/** IPv6 destination options */
	PACKET_IPPROTO_MTP 		= 92,	/** Multicast Transport Protocol */
	PACKET_IPPROTO_ENCAP 	= 98,	/** Encapsulation Header */
	PACKET_IPPROTO_PIM 		= 103,	/** Protocol Independent Multicast */
	PACKET_IPPROTO_COMP 	= 108,	/** Compression Header Protocol */
	PACKET_IPPROTO_SCTP 	= 132,	/** SCTP Protocol		*/
	PACKET_IPPROTO_UDPLITE 	= 136,	/** UDP-Lite protocol */
	PACKET_IPPROTO_RAW 		= 255,  /** Raw IP Packets */
}IPProtocolTypes;

namespace Global
{
	/* Timer Parameters */
	extern uint16_t		TIME_INDEX;
	extern uint16_t 	CURRENT_SEC;
	extern uint16_t 	CURRENT_MIN;
	extern uint16_t 	CURRENT_HOUR;
	extern uint16_t 	CURRENT_DAY;
	extern uint16_t		CURRENT_MONTH;
	extern uint16_t 	CURRENT_YEAR;
	extern uint64_t		CURRENT_EPOCH_SEC;
	extern uint64_t 	CURRENT_EPOCH_NANO_SEC;
	extern uint64_t 	CURRENT_EPOCH_MILI_SEC;
}
/* Server communication Packet from client */
typedef struct _TcpPacket
{
	uint8_t 	ipVer;
	uint8_t 	ipTtl;
	uint8_t		direction;
	uint16_t	vLanId;
	uint16_t	frSize;
	uint16_t 	ipTLen;
	uint16_t 	ipHLen;
	uint16_t	ipIdentification;
	uint16_t 	sPort;
	uint16_t 	dPort;
	uint16_t	pLoad;
	uint16_t	tcpFlags;
	uint32_t 	sIp;
	uint32_t	dIp;
	uint32_t 	tcpSeqNo;
	uint64_t	frTimeEpochSec;				//Check size to optimize
	uint64_t	ipv4FlowId;
	uint64_t	frTimeEpochNanoSec;
	char		httpAgent[HTTP_AGENT_LEN];

	_TcpPacket()
	{ reset(); }

	_TcpPacket(const _TcpPacket& tcppkt)
	{
		this->ipVer 				= tcppkt.ipVer;
		this->ipTtl 				= tcppkt.ipTtl;
		this->direction 			= tcppkt.direction;
		this->vLanId				= tcppkt.vLanId;
		this->frSize 				= tcppkt.frSize;
		this->ipTLen 				= tcppkt.ipTLen;
		this->ipHLen 				= tcppkt.ipHLen;
		this->ipIdentification		= tcppkt.ipIdentification;
		this->sPort 				= tcppkt.sPort;
		this->dPort 				= tcppkt.dPort;
		this->pLoad 				= tcppkt.pLoad;
		this->tcpFlags 				= tcppkt.tcpFlags;
		this->sIp 					= tcppkt.sIp;
		this->dIp 					= tcppkt.dIp;
		this->tcpSeqNo 				= tcppkt.tcpSeqNo;
		this->frTimeEpochSec 		= tcppkt.frTimeEpochSec;
		this->ipv4FlowId			= tcppkt.ipv4FlowId;
		this->frTimeEpochNanoSec 	= tcppkt.frTimeEpochNanoSec;
		strcpy(this->httpAgent, tcppkt.httpAgent);
	}

	void copy(const _TcpPacket* tcppkt)
	{
		this->ipVer 				= tcppkt->ipVer;
		this->ipTtl 				= tcppkt->ipTtl;
		this->direction 			= tcppkt->direction;
		this->vLanId				= tcppkt->vLanId;
		this->frSize 				= tcppkt->frSize;
		this->ipTLen 				= tcppkt->ipTLen;
		this->ipHLen 				= tcppkt->ipHLen;
		this->ipIdentification		= tcppkt->ipIdentification;
		this->sPort 				= tcppkt->sPort;
		this->dPort 				= tcppkt->dPort;
		this->pLoad 				= tcppkt->pLoad;
		this->tcpFlags 				= tcppkt->tcpFlags;
		this->sIp 					= tcppkt->sIp;
		this->dIp 					= tcppkt->dIp;
		this->tcpSeqNo 				= tcppkt->tcpSeqNo;
		this->frTimeEpochSec 		= tcppkt->frTimeEpochSec;
		this->ipv4FlowId			= tcppkt->ipv4FlowId;
		this->frTimeEpochNanoSec 	= tcppkt->frTimeEpochNanoSec;
		strcpy(this->httpAgent, tcppkt->httpAgent);
	}

	void reset()
	{
		this->ipVer 			= 0;
		this->ipTtl 			= 0;
		this->direction			= 0;
		this->vLanId			= 0;
		this->frSize 			= 0;
		this->ipTLen 			= 0;
		this->ipHLen 			= 0;
		this->ipIdentification 	= 0;
		this->sPort				= 0;
		this->dPort 			= 0;
		this->pLoad 			= 0;
		this->tcpFlags 			= 0;
		this->sIp 				= 0;
		this->dIp 				= 0;
		this->tcpSeqNo 			= 0;
		this->frTimeEpochSec	= 0;
		this->ipv4FlowId	 	= 0;
		this->frTimeEpochNanoSec= 0;
		this->httpAgent[0]		= 0;
	}
}TcpPacket;

typedef struct _UdpPacket
{
	uint8_t 	ipVer;
	uint8_t		direction;
	uint16_t	vLanId;
	uint16_t	frSize;
	uint16_t 	ipTLen;
	uint16_t 	ipHLen;
	uint16_t 	sPort;
	uint16_t 	dPort;
	uint16_t	pLoad;
	uint32_t 	sIp;
	uint32_t	dIp;
	uint64_t	frTimeEpochSec;				//Check size to optimize
	uint64_t	ipv4FlowId;
	uint64_t	frTimeEpochNanoSec;

	_UdpPacket()
	{ reset(); }

	_UdpPacket(const _UdpPacket& udpPkt)
	{
		this->ipVer 				= udpPkt.ipVer;
		this->direction 			= udpPkt.direction;
		this->vLanId				= udpPkt.vLanId;
		this->frSize 				= udpPkt.frSize;
		this->ipTLen 				= udpPkt.ipTLen;
		this->ipHLen 				= udpPkt.ipHLen;
		this->sPort 				= udpPkt.sPort;
		this->dPort 				= udpPkt.dPort;
		this->pLoad 				= udpPkt.pLoad;
		this->sIp 					= udpPkt.sIp;
		this->dIp 					= udpPkt.dIp;
		this->frTimeEpochSec 		= udpPkt.frTimeEpochSec;
		this->ipv4FlowId			= udpPkt.ipv4FlowId;
		this->frTimeEpochNanoSec 	= udpPkt.frTimeEpochNanoSec;
	}

	void copy(const _UdpPacket* udpPkt)
	{
		this->ipVer 				= udpPkt->ipVer;
		this->direction 			= udpPkt->direction;
		this->vLanId				= udpPkt->vLanId;
		this->frSize 				= udpPkt->frSize;
		this->ipTLen 				= udpPkt->ipTLen;
		this->ipHLen 				= udpPkt->ipHLen;
		this->sPort 				= udpPkt->sPort;
		this->dPort 				= udpPkt->dPort;
		this->pLoad 				= udpPkt->pLoad;
		this->sIp 					= udpPkt->sIp;
		this->dIp 					= udpPkt->dIp;
		this->frTimeEpochSec 		= udpPkt->frTimeEpochSec;
		this->ipv4FlowId			= udpPkt->ipv4FlowId;
		this->frTimeEpochNanoSec 	= udpPkt->frTimeEpochNanoSec;
	}

	void reset()
	{
		this->ipVer 			= 0;
		this->direction			= 0;
		this->vLanId			= 0;
		this->frSize 			= 0;
		this->ipTLen 			= 0;
		this->ipHLen 			= 0;
		this->sPort				= 0;
		this->dPort 			= 0;
		this->pLoad 			= 0;
		this->sIp 				= 0;
		this->dIp 				= 0;
		this->frTimeEpochSec 	= 0;
		this->ipv4FlowId		= 0;
		this->frTimeEpochNanoSec= 0;
	}
}UdpPacket;

typedef struct _DnsPacket
{
	uint8_t 	ipVer;
	uint8_t		qrFlag;
	uint8_t		responseCode;
	char 		url[URL_LEN];
	uint16_t 	sPort;
	uint16_t 	dPort;
	uint32_t 	sIp;
	uint32_t	dIp;
	uint32_t	transactionId;
	uint64_t	frTimeEpochSec;				//Check size to optimize
	uint64_t	ipv4FlowId;
	uint64_t	frTimeEpochNanoSec;

	_DnsPacket()
	{ reset(); }

	_DnsPacket(const _DnsPacket& dnsPkt)
	{
		this->ipVer 				= dnsPkt.ipVer;
		this->qrFlag				= dnsPkt.qrFlag;
		this->responseCode 			= dnsPkt.responseCode;
		strcpy(this->url, dnsPkt.url);
		this->sPort 				= dnsPkt.sPort;
		this->dPort 				= dnsPkt.dPort;
		this->sIp 					= dnsPkt.sIp;
		this->dIp 					= dnsPkt.dIp;
		this->transactionId			= dnsPkt.transactionId;
		this->frTimeEpochSec 		= dnsPkt.frTimeEpochSec;
		this->ipv4FlowId 			= dnsPkt.ipv4FlowId;
		this->frTimeEpochNanoSec 	= dnsPkt.frTimeEpochNanoSec;
	}

	void copy(const _DnsPacket* dnsPkt)
	{
		this->ipVer 				= dnsPkt->ipVer;
		this->qrFlag				= dnsPkt->qrFlag;
		this->responseCode 			= dnsPkt->responseCode;
		strcpy(this->url, dnsPkt->url);
		this->sPort 				= dnsPkt->sPort;
		this->dPort 				= dnsPkt->dPort;
		this->sIp 					= dnsPkt->sIp;
		this->dIp 					= dnsPkt->dIp;
		this->transactionId			= dnsPkt->transactionId;
		this->frTimeEpochSec 		= dnsPkt->frTimeEpochSec;
		this->ipv4FlowId 			= dnsPkt->ipv4FlowId;
		this->frTimeEpochNanoSec 	= dnsPkt->frTimeEpochNanoSec;
	}

	void reset()
	{
		this->ipVer 			= 0;
		this->qrFlag			= 3;	/* Default is 3, 0 - Request, 1 - Response */
		this->responseCode 		= 3;
		this->url[0]			= 0;
		this->sPort				= 0;
		this->dPort 			= 0;
		this->sIp 				= 0;
		this->dIp 				= 0;
		this->transactionId		= 0;
		this->frTimeEpochSec 	= 0;
		this->ipv4FlowId		= 0;
		this->frTimeEpochNanoSec= 0;
	}
}DnsPacket;

typedef struct _AAAPacket
{
	uint8_t 	ipVer;
	uint8_t		direction;
	char		userName[AAA_USER_NAME_LEN];
	char		replyMsg[35];
	uint16_t	frSize;
	uint16_t 	ipTLen; /* TODO */
	uint16_t 	ipHLen; /* TODO */
	uint16_t	frByteLen; /* TODO */
	uint16_t 	sPort;
	uint16_t 	dPort;
	uint16_t 	code;
	uint16_t	identifier;
	uint32_t	protocol;
	uint32_t	serviceType;
	uint32_t	accStatusType;
	uint32_t	accAuth;
	uint32_t	terminationCause;
	uint32_t 	sIp;
	uint32_t	dIp;
	uint32_t	framedIp;
	uint32_t	inputOctets;
	uint32_t	outputOctets;
	uint32_t	inputGigaWords;
	uint32_t	outputGigaWords;
	uint32_t	inputPackets;
	uint32_t	outputPackets;
	uint64_t	frTimeEpochSec;				//Check size to optimize
	uint64_t    frTimeEpochMilliSec;

	_AAAPacket()
	{ reset(); }

	_AAAPacket(const _AAAPacket& aaaPkt)
	{
		this->ipVer 					= aaaPkt.ipVer;
		this->direction 				= aaaPkt.direction;
		strcpy(this->userName, aaaPkt.userName);
		strcpy(this->replyMsg, aaaPkt.replyMsg);
		this->frSize 					= aaaPkt.frSize;
		this->ipTLen 					= aaaPkt.ipTLen;
		this->ipHLen 					= aaaPkt.ipHLen;
		this->frByteLen					= aaaPkt.frByteLen;
		this->sPort 					= aaaPkt.sPort;
		this->dPort 					= aaaPkt.dPort;
		this->code 						= aaaPkt.code;
		this->identifier 				= aaaPkt.identifier;
		this->protocol 					= aaaPkt.protocol;
		this->serviceType 				= aaaPkt.serviceType;
		this->accStatusType 			= aaaPkt.accStatusType;
		this->accAuth 					= aaaPkt.accAuth;
		this->terminationCause 			= aaaPkt.terminationCause;
		this->sIp 						= aaaPkt.sIp;
		this->dIp 						= aaaPkt.dIp;
		this->framedIp 					= aaaPkt.framedIp;
		this->inputOctets 				= aaaPkt.inputOctets;
		this->outputOctets 				= aaaPkt.outputOctets;
		this->inputGigaWords 			= aaaPkt.inputGigaWords;
		this->outputGigaWords 			= aaaPkt.outputGigaWords;
		this->inputPackets 				= aaaPkt.inputPackets;
		this->outputPackets 			= aaaPkt.outputPackets;
		this->frTimeEpochSec 			= aaaPkt.frTimeEpochSec;
		this->frTimeEpochMilliSec 		= aaaPkt.frTimeEpochMilliSec;
	}

	void copy(const _AAAPacket* aaaPkt)
	{
		this->ipVer 					= aaaPkt->ipVer;
		this->direction 				= aaaPkt->direction;
		strcpy(this->userName, aaaPkt->userName);
		strcpy(this->replyMsg, aaaPkt->replyMsg);
		this->frSize 					= aaaPkt->frSize;
		this->ipTLen 					= aaaPkt->ipTLen;
		this->ipHLen 					= aaaPkt->ipHLen;
		this->frByteLen					= aaaPkt->frByteLen;
		this->sPort 					= aaaPkt->sPort;
		this->dPort 					= aaaPkt->dPort;
		this->code 						= aaaPkt->code;
		this->identifier 				= aaaPkt->identifier;
		this->protocol 					= aaaPkt->protocol;
		this->serviceType 				= aaaPkt->serviceType;
		this->accStatusType 			= aaaPkt->accStatusType;
		this->accAuth 					= aaaPkt->accAuth;
		this->terminationCause 			= aaaPkt->terminationCause;
		this->sIp 						= aaaPkt->sIp;
		this->dIp 						= aaaPkt->dIp;
		this->framedIp 					= aaaPkt->framedIp;
		this->inputOctets 				= aaaPkt->inputOctets;
		this->outputOctets 				= aaaPkt->outputOctets;
		this->inputGigaWords 			= aaaPkt->inputGigaWords;
		this->outputGigaWords 			= aaaPkt->outputGigaWords;
		this->inputPackets 				= aaaPkt->inputPackets;
		this->outputPackets 			= aaaPkt->outputPackets;
		this->frTimeEpochSec 			= aaaPkt->frTimeEpochSec;
		this->frTimeEpochMilliSec 		= aaaPkt->frTimeEpochMilliSec;
	}

	void reset()
	{
		this->ipVer 					= 0;
		this->direction 				= 0;
		strcpy(this->userName, "NA");
		strcpy(this->replyMsg, "NA");
		this->frSize 					= 0;
		this->ipTLen 					= 0;
		this->ipHLen 					= 0;
		this->frByteLen 				= 0;
		this->sPort						= 0;
		this->dPort 					= 0;
		this->code 						= 0;
		this->identifier 				= 0;
		this->protocol 					= 0;
		this->serviceType 				= 0;
		this->accStatusType 			= 0;
		this->accAuth 					= 0;
		this->terminationCause 			= 0;
		this->sIp 						= 0;
		this->dIp 						= 0;
		this->framedIp 					= 0;
		inputOctets 					= 0;
		this->outputOctets 				= 0;
		this->inputGigaWords 			= 0;
		this->outputGigaWords 			= 0;
		this->inputPackets 				= 0;
		this->outputPackets 			= 0;
		this->frTimeEpochSec 			= 0;
		this->frTimeEpochMilliSec 		= 0;
	}
}AAAPacket;

#define TCPPACKET	TcpPacket 		tcp
#define UDPPACKET	UdpPacket 		udp
#define AAAPACKET	AAAPacket 		aaa

typedef struct _MPacket
{
	uint8_t protocolType;

	union
	{
		TCPPACKET;
		UDPPACKET;
		AAAPACKET;
	};

	_MPacket()
	{ reset(); }

	void reset()
	{
		this->protocolType = 0;

		this->tcp.reset();
		this->udp.reset();
		this->aaa.reset();
	}

	void copy(const _MPacket* mpkt)
	{
		this->protocolType	= mpkt->protocolType;

		this->tcp 			= mpkt->tcp;
		this->udp			= mpkt->udp;
		this->aaa 			= mpkt->aaa;
	}
}MPacket;


class ServerGlobal
{
	public:
		ServerGlobal();
		~ServerGlobal();
};

#endif /* CORE_SRC_SERVERGLOBAL_H_ */
