/*
 * Initialize.h
 *
 *  Created on: 18 Mar 2021
 *      Author: Debashis
 */

#ifndef CORE_SRC_INITIALIZE_H_
#define CORE_SRC_INITIALIZE_H_

#include "Log.h"
#include "SmGlobal.h"
#include "BaseConfig.h"

class Initialize : BaseConfig
{
	public:
		Initialize();
		~Initialize();

		void 	initProtocolName();
		void	dnsErrorCode();
		void	tcpPorts();
		void	aaaInitCode();
		void	aaaServiceType();
		void	aaaProtocol();
		void	aaaAcctAuthentication();
		void	aaaAccTerminationCause();
		void	aaaAccStatus();
		void	aaaNasPortType();
		void	ipSubNet();
		void	load1ResolvedIpv4();
		void	load2ResolvedIpv4();

		void	loadAAAData();
		void	dumpAAAData();
		void	clearAAAData();

	private:
		void	lockDnsMap();
		void	unLockDnsMap();
		void	lockAAAMap();
		void	unLockAAAMap();
};

#endif /* CORE_SRC_INITIALIZE_H_ */
