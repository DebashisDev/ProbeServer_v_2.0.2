#include <signal.h>

#include "SpectaProbe.h"

void sig_handler(int signo)
{
	if (signo == SIGTERM || signo == SIGINT)
	{
		SMGlobal::TIMER_RUNNING_STATUS = false;			/* Stop Timer Thread */
		SMGlobal::PRINT_STATS 	= false;			/* Stop Statistic Print */

		sleep(1);
													/* Stop Routers */
		for(uint16_t interfaceId = 0; interfaceId < SMGlobal::NO_OF_INTERFACE_SUPPORT; interfaceId ++)
			for(uint16_t routerId = 0; routerId < SMGlobal::ROUTER_PER_INTERFACE[interfaceId]; routerId ++)
				SMGlobal::ROUTER_RUNNING_STATUS[interfaceId][routerId] = false;

		sleep(1);
													/* Stop Read Agents */
		for(uint32_t interface = 0; interface < SMGlobal::NO_OF_INTERFACE_SUPPORT; interface ++)
		{
			SMGlobal::TCP_AGENT_RUNNING_STATUS[interface] = false;
			SMGlobal::UDP_AGENT_RUNNING_STATUS[interface] = false;

			SMGlobal::AAA_AGENT_RUNNING_STATUS[interface] = false;
			SMGlobal::DNS_AGENT_RUNNING_STATUS[interface] = false;
		}
		sleep(1);

		/* Stop Tcp Session Manager */
		for (uint16_t sessionCount = 0; sessionCount < SMGlobal::TCP_SM_INSTANCES; sessionCount ++)
			SMGlobal::TCP_SM_RUNNING_STATUS[sessionCount] = false;

		sleep(1);

		/* Stop Udp Session Manager */
		for (uint16_t sessionCount = 0; sessionCount < SMGlobal::UDP_SM_INSTANCES; sessionCount ++)
			SMGlobal::UDP_SM_RUNNING_STATUS[sessionCount] = false;

		sleep(1);

		/* Stop Dns Session Manager */
		for (uint16_t sessionCount = 0; sessionCount < SMGlobal::DNS_SM_INSTANCES; sessionCount ++)
			SMGlobal::DNS_SM_RUNNING_STATUS[sessionCount] = false;

		sleep(1);

		/* Stop Aaa Session Manager */
		for (uint16_t sessionCount = 0; sessionCount < SMGlobal::AAA_SM_INSTANCES; sessionCount ++)
			SMGlobal::AAA_SM_RUNNING_STATUS[sessionCount] = false;

		sleep(1);

		/* Stop Un-Mapped Session Manager */
		for (uint16_t sessionCount = 0; sessionCount < SMGlobal::UNM_SM_INSTANCES; sessionCount ++)
			SMGlobal::UNM_SM_RUNNING_STATUS[sessionCount] = false;

		sleep(1);

		/* Stop Tcp Flushers */
		for(uint16_t i = 0; i < SMGlobal::TCP_FLUSHER_INSTANCE; i++)
			SMGlobal::TCP_FLUSHER_RUNNING_STATUS[i] = false;

		sleep(1);

		/* Stop Udp Flusher */
		for(uint16_t i = 0; i < SMGlobal::UDP_FLUSHER_INSTANCE; i++)
			SMGlobal::UDP_FLUSHER_RUNNING_STATUS[i] = false;

		sleep(1);

		/* Stop Dns Flusher */
		for(uint16_t i = 0; i < SMGlobal::DNS_FLUSHER_INSTANCE; i++)
			SMGlobal::DNS_FLUSHER_RUNNING_STATUS[i] = false;

		sleep(1);

		/* Stop Aaa Flusher */
		for(uint16_t i = 0; i < SMGlobal::AAA_FLUSHER_INSTANCE; i++)
			SMGlobal::AAA_FLUSHER_RUNNING_STATUS[i] = false;

		sleep(1);

		SMGlobal::UNM_FLUSHER_RUNNING_STATUS = false; 	/* Stop Un-Mapped Flusher */
		sleep(1);

		SMGlobal::STATSLOG_RUNNING_STATUS 		= false;	/* Stop Statistic Thread */
		SMGlobal::LOG_RUNNING_STATUS 			= false;	/* Stop Log Thread */
		sleep(1);
		SMGlobal::SERVER_RUNNING_STATUS = false;			/* Stop Main Thread */
	}
}

int main(int argc, char *argv[])
{
	if (signal(SIGTERM, sig_handler) == SIG_ERR || signal(SIGINT, sig_handler) == SIG_ERR)
		printf("SpectaServer Can't Received Signal...\n");

	timeval curTime;
	struct tm *now_tm;

	gettimeofday(&curTime, NULL);
	now_tm = localtime(&curTime.tv_sec);

	SMGlobal::SERVER_RUNNING_STATUS = true;
	SpectaProbe *spectaProbe = new SpectaProbe("server.config");
	spectaProbe->start();

	printf("  **** SpectaServer Exiting...Please wait... ***** \n");
}
