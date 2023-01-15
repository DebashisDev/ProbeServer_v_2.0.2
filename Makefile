#########################################################################
#																		#
# SCRIPT NAME	: Makefile												#
# DESCRIPTION	: To build the SpectaProbe along with user libraries	#
# DATE 			: 19-12-2018										    #
# AUTHOR		: Debashis.											    #
#																		#
# To make probe server : make clean; make server 						#
# Copyright (c) 2016, Pinnacle Digital (P) Ltd. New-Delhi.				# 
#########################################################################

# Include all the header directories.
include ${PROBE_ROOT}/probe.mk

PROBE_DIRS = 			\
		${CORE_SRC} 	\
		${LOG_SRC} 		\
		${TCP_SRC}		\
		${UDP_SRC} 		\
		${DNS_SRC}		\
		${AAA_SRC}		\
		${UNKNOWN_SRC}	

#########################################################################
# SCP Platform and Platform Library File Name							#
#########################################################################
PROBE_TARGET = ${PROBE_BIN}/spectaServer

#System Library
THRLIB 		= pthread
ZMQLIB 		= zmq
LIBS 		= -lm -ldl -l$(THRLIB) -l$(ZMQLIB)

#########################################################################
# For SpectaProbe
#########################################################################

server:
	echo ${PROBE_DIRS}
	for i in ${PROBE_DIRS}; \
	do \
		(cd $$i; \
		echo "*******" $$i; \
		${MAKE} all \
		); \
	done

	${GCC} -o ${PROBE_TARGET} \
				${CORE_SRC}/*.o		\
				${LOG_SRC}/*.o 		\
				${TCP_SRC}/*.o 		\
				${UDP_SRC}/*.o 		\
				${DNS_SRC}/*.o		\
				${AAA_SRC}/*.o		\
				${UNKNOWN_SRC}/*.o	\
				${LIBS}

#########################################################################
clean:
	for i in ${PROBE_DIRS}; \
	do \
		(cd $$i; \
		echo $$i; \
		${MAKE} clean \
		); \
	done

	${RM} ${PROBE_TARGET}
	
