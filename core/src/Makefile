#########################################################################
#																		#
# SCRIPT NAME	: Makefile												#
# DESCRIPTION	: To build all .cpp files								#
# DATE 			: 19-02-2016										    #
# AUTHOR		: Debashis.											    #
#																		#
# Copyright (c) 2016, Pinnacle Digital (P) Ltd. New-Delhi				# 
#########################################################################

include ${PROBE_ROOT}/probe.mk
OBJS= \
	BaseConfig.o	\
	FUtility.o		\
	GConfig.o		\
	Initialize.o	\
	Main.o			\
	MapStores.o		\
	ProbeStats.o	\
	ProbeStatsLog.o	\
	PUtility.o		\
	ReadAaaAgent.o	\
	ReadTcpAgent.o	\
	ReadUdpAgent.o	\
	ReadDnsAgent.o	\
	Routers.o		\
	ServerGlobal.o	\
	SmGlobal.o		\
	SpectaProbe.o		\
	Timer.o
	
	
all:${OBJS}

.cpp.o:

	${GCC} ${CFLAGS} -I/usr/include ${PROBE_INCLUDE} -c $< 

clean:
	${RM} ${OBJS}
