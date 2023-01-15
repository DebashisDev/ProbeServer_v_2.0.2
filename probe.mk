#########################################################################
#																		#
# SCRIPT NAME	: probe.mk												#
# DESCRIPTION	: Include script for SpectaProbe						#
# DATE 			: 19-12-2018										    #
# AUTHOR		: Debashis.											    #
#																		#
# For RADIUS    Compilation:: _RADIUS								#
# For Mobility  Compilation:: _GN										#
# With Solarflare use -D_SF												#
# Copyright (c) 2016, Pinnacle Digital (P) Ltd. New-Delhi				# 
#########################################################################

GCC = g++ -g -O0 -g3 -Wall -std=c++11 -fPIC -Wno-deprecated -w

CFLAGS = -D_MT_SAFE -D_REENTRANT -D_POSIX_THREAD_SAFE_FUNCTIONS -D_GNU_SOURCE -D_PTHREADS -D_POSIX_PTHREAD_SEMANTICS -D_POSIX_C_SOURCE=199506L -D__EXTENSIONS__	

AR 			= ar
ARFLAGS 	= -r
RM 			= rm 
MAKE 		= make

PROBE_BIN	= ${PROBE_ROOT}/bin

# SpectaProbe Directory

CORE_ROOT		= ${PROBE_ROOT}/core
CORE_INC		= ${CORE_ROOT}/inc
CORE_SRC		= ${CORE_ROOT}/src

TCP_ROOT		= ${PROBE_ROOT}/plugins/tcp
TCP_INC			= ${TCP_ROOT}/inc
TCP_SRC			= ${TCP_ROOT}/src

UDP_ROOT		= ${PROBE_ROOT}/plugins/udp
UDP_INC			= ${UDP_ROOT}/inc
UDP_SRC			= ${UDP_ROOT}/src

DNS_ROOT		= ${PROBE_ROOT}/plugins/dns
DNS_INC			= ${DNS_ROOT}/inc
DNS_SRC			= ${DNS_ROOT}/src

AAA_ROOT		= ${PROBE_ROOT}/plugins/aaa
AAA_INC			= ${AAA_ROOT}/inc
AAA_SRC			= ${AAA_ROOT}/src

UNKNOWN_ROOT	= ${PROBE_ROOT}/plugins/unknown
UNKNOWN_INC		= ${UNKNOWN_ROOT}/inc
UNKNOWN_SRC		= ${UNKNOWN_ROOT}/src

LOG_ROOT		= ${PROBE_ROOT}/utility/log
LOG_SRC			= ${LOG_ROOT}/src
LOG_INC			= ${LOG_ROOT}/inc

PROBE_INCLUDE 	= \
				-I${CORE_INC} 	\
				-I${LOG_INC} 	\
				-I${TCP_INC}	\
				-I${UDP_INC}	\
				-I${DNS_INC}	\
				-I${AAA_INC}	\
				-I${UNKNOWN_INC}
