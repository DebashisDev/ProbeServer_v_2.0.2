/*
 * ProbeUtility.h
 *
 *  Created on: 30-Jan-2016
 *      Author: Debashis
 */

#ifndef SRC_PROBEUTILITY_H_
#define SRC_PROBEUTILITY_H_


#include <netinet/tcp.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string>
#include <stdlib.h>
#include <vector>
#include <sstream>

#include "SmGlobal.h"

#define BASE		10000000000


#define handle_error_en(en, msg) \
       do { perror(msg); exit(EXIT_FAILURE); } while (0)

class PUtility
{
	public:
		PUtility();
		~PUtility();

		void 	 Append(char *original, const char *add);
		void 	 printBytes(const char *identifier, bool printflag, const BYTE packet, uint32_t size);
		void	 getIPHex(char *address, char *hexaddress);
		void	 fillIP(char *address, char *fillInAddress);
		void	 ExtractIP4Address(const BYTE packet, char *ipBuffer, uint32_t loc);
		void	 ExtractIP6Address(const char *packet, char *ipBuffer, uint32_t loc);
		void	 ExtractIP6Prefix(const char *packet, char *ipBuffer, uint32_t loc, uint32_t end);
		void 	 pinThread(pthread_t th, int core_num);
		static vector<string> split(string str, char delimiter);
		void 	 printPacketBytes(const BYTE packet, int size);
		ULONG   HextoDigits(char *hexadecimal);
		ULONG   getLength(const BYTE packet, size_t offset);
		void 	 HEXDUMP(const void* pv, int len);
		char*    getByteToHex(const void* pv, int len);
		void writePcap(char *pcapFileName, const BYTE packet, uint64_t counter, int length, long tv_sec);
};

#endif /* SRC_PROBEUTILITY_H_ */
