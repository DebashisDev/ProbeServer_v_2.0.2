/*
 * MapStores.h
 *
 *  Created on: 21 Mar 2020
 *      Author: Debashis
 */

#ifndef CORE_SRC_MAPSTORES_H_
#define CORE_SRC_MAPSTORES_H_

#include <map>
#include <unordered_map>

#include "ServerGlobal.h"
#include "SmGlobal.h"
#include "SpectaTypedef.h"

namespace agentStore
{
	extern std::unordered_map<uint32_t, TcpPacket*> tcpAStore[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern bool tcpABusy[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern uint32_t tcpACnt[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];

	extern std::unordered_map<uint32_t, UdpPacket*> udpAStore[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern bool udpABusy[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern uint32_t udpACnt[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];

	extern std::unordered_map<uint32_t, AAAPacket*> aaaAStore[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern bool aaaABusy[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern uint32_t aaaACnt[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];

	extern std::unordered_map<uint32_t, DnsPacket*> dnsAStore[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern bool dnsABusy[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern uint32_t dnsACnt[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
}

namespace smStore
{
	extern std::unordered_map<uint32_t, TcpPacket> tcpSmStore[MAX_TCP_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern bool tcpSmBusy[MAX_TCP_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern uint32_t tcpSmCnt[MAX_TCP_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];

	extern std::unordered_map<uint32_t, UdpPacket> udpSmStore[MAX_UDP_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern bool udpSmBusy[MAX_UDP_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern uint32_t udpSmCnt[MAX_UDP_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];

	extern std::unordered_map<uint32_t, DnsPacket> dnsSmStore[MAX_DNS_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern bool dnsSmBusy[MAX_DNS_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern uint32_t dnsSmCnt[MAX_DNS_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];

	extern std::unordered_map<uint32_t, AAAPacket> aaaSmStore[MAX_AAA_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern bool aaaSmBusy[MAX_AAA_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern uint32_t aaaSmCnt[MAX_AAA_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];

	extern std::unordered_map<uint32_t, TcpPacket> unTcpSmStore[MAX_UNM_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern bool unTcpSmBusy[MAX_UNM_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern uint32_t unTcpSmCnt[MAX_UNM_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];

	extern std::unordered_map<uint32_t, UdpPacket> unUdpSmStore[MAX_UNM_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern bool unUdpSmBusy[MAX_UNM_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
	extern uint32_t unUdpSmCnt[MAX_UNM_SM_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE][10];
}

namespace FlushRepo
{
	extern std::unordered_map<uint32_t, tcpSession> tcpFlStore[MAX_FLUSHER_SUPPORT][MAX_TCP_SM_SUPPORT][10];
	extern uint32_t tcpFlCnt[MAX_FLUSHER_SUPPORT][MAX_TCP_SM_SUPPORT][10];

	extern std::unordered_map<uint32_t, udpSession> udpFlStore[MAX_FLUSHER_SUPPORT][MAX_UDP_SM_SUPPORT][10];
	extern uint32_t udpFlCnt[MAX_FLUSHER_SUPPORT][MAX_UDP_SM_SUPPORT][10];

	extern std::unordered_map<uint32_t, dnsSession> dnsFlStore[MAX_FLUSHER_SUPPORT][MAX_UDP_SM_SUPPORT][10];
	extern uint32_t dnsFlCnt[MAX_FLUSHER_SUPPORT][MAX_UDP_SM_SUPPORT][10];

	extern std::unordered_map<uint32_t, aaaSession> aaaFlStore[MAX_FLUSHER_SUPPORT][MAX_AAA_SM_SUPPORT][10];
	extern uint32_t aaaFlCnt[MAX_FLUSHER_SUPPORT][MAX_AAA_SM_SUPPORT][10];
}

namespace UnFlushRepo
{
	extern std::unordered_map<uint32_t, tcpSession> uTcpFlStore[MAX_UNM_SM_SUPPORT][10];
	extern uint32_t uTcpFlCnt[MAX_UNM_SM_SUPPORT][10];

	extern std::unordered_map<uint32_t, udpSession> uUdpFlStore[MAX_UNM_SM_SUPPORT][10];
	extern uint32_t uUdpFlCnt[MAX_UDP_SM_SUPPORT][10];

	extern std::unordered_map<uint32_t, dnsSession> uDnsFlStore[MAX_UNM_SM_SUPPORT][10];
	extern uint32_t uDnsFlCnt[MAX_UDP_SM_SUPPORT][10];
}
#endif /* CORE_SRC_MAPSTORES_H_ */
