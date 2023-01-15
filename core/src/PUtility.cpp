/*
 * ProbeUtility.cpp
 *
 *  Created on: 30-Jan-2016
 *      Author: Debashis
 */

#include "PUtility.h"

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <pcap.h>
#include <unistd.h>
#include <arpa/inet.h>


PUtility::PUtility()
{ }

PUtility::~PUtility()
{ }

ULONG PUtility::HextoDigits(char *hexadecimal)
{
	long decimalNumber=0;
	char hexDigits[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	int i, j, power=0, digit;

	for(i=strlen(hexadecimal)-1; i >= 0; i--)
	{
		for(j=0; j<16; j++)
		{
			if(hexadecimal[i] == hexDigits[j])
			{
				decimalNumber += j*pow(16, power);
			}
		}
		 power++;
	}
	return decimalNumber;
}

ULONG PUtility::getLength(const BYTE packet, size_t offset)
{
	char hexadecimal[10];
	hexadecimal[0] = 0;

	sprintf(hexadecimal, "%02x%02x%02x", packet[offset], packet[offset+1], packet[offset+2]);

	ULONG decimalNumber=0;
	char hexDigits[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	int i, j, power=0, digit;

	for(i=strlen(hexadecimal)-1; i >= 0; i--)
	{
		for(j=0; j<16; j++)
		{
			if(hexadecimal[i] == hexDigits[j])
			{ decimalNumber += j*pow(16, power); }
		}
		 power++;
	}
	return decimalNumber;
}

void PUtility::Append(char *original, const char *add)
{
   while(*original)
      original++;

   while((*original++ = *add++))
   *original = 0;
}

void PUtility::printBytes(const char *identifier, bool printflag, const BYTE packet, uint32_t size)
{
	if(printflag){
		printf("  %s packet of size [%d] bytes", identifier, size);
		for(uint32_t i=0;i<size;i++){
			if((i%16 == 0)) printf("\n	%04x ", i);
			printf("%02x ", packet[i]);
		}
		printf("\n\n");
	}
}

void PUtility::getIPHex(char *address, char *hexaddress)
{
	char *p, a[25];
	uint32_t i = 0;

	p = NULL;
	a[0] = 0;
	p = (char *)strtok((char *)address, ".");

	while(p!= NULL)
	{
		sprintf((char *)a, "%02x", atoi((const char *)p));
	    strcat((char *)hexaddress, (const char *)a);
	    a[0] = 0;

	    if(i < 3)
	      strcat((char *)hexaddress, ".");
	    p = (char *)strtok(NULL, ".");
	    i++;
	}
}

void PUtility::fillIP(char *address, char *fillInAddress)
{
	char *p, a[25];
	uint32_t i = 0;

	p = NULL;
	a[0] = 0;
	p = (char *)strtok((char *)address, ".");

	while(p!= NULL)
	{
		sprintf((char *)a, "%03d", atoi((const char *)p));
	    strcat((char *)fillInAddress, (const char *)a);
	    a[0] = 0;

	    if(i < 3)
	      strcat((char *)fillInAddress, ".");
	    p = (char *)strtok(NULL, ".");
	    i++;
	}
}

void PUtility::ExtractIP4Address(const BYTE packet, char *ipBuffer, uint32_t loc)
{
	unsigned int address;

	address = (packet[loc] << 24) | (packet[loc+1] << 16) | (packet[loc+2] << 8) | (packet[loc+3]);
	sprintf((char *)ipBuffer,"%d.%d.%d.%d",(address & 0xFF000000) >> 24,(address & 0x00FF0000) >> 16,(address & 0x0000FF00) >> 8, address & 0x000000FF);
}

void PUtility::ExtractIP6Address(const char *packet, char *ipBuffer, uint32_t loc)
{
	unsigned char buf[sizeof(struct in6_addr)];
	int domain = AF_INET6, ret;

	ipBuffer[0] = 0;
	ret = 0;

	sprintf(ipBuffer,"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x", packet[loc], packet[loc+1], \
			packet[loc+2], packet[loc+3], packet[loc+4], packet[loc+5], packet[loc+6], packet[loc+7], packet[loc+8], packet[loc+9], \
			packet[loc+10], packet[loc+11], packet[loc+12], packet[loc+13], packet[loc+14], packet[loc+15], packet[loc+16]);

//	ret = inet_pton(domain, IP6Addr, buf);
//	if (ret <= 0)
//	{
//		if (ret == 0) {
//			fprintf(stderr, "Not in presentation format");
//			ipBuffer[0] = 0;
//		}
//		else
//			perror("inet_pton");
//	}
//
//	if (inet_ntop(domain, buf, ipBuffer, INET6_ADDRSTRLEN) == NULL) {
//	               perror("inet_ntop");
//	               ipBuffer[0] = 0;
//	}

}

void PUtility::ExtractIP6Prefix(const char *packet, char *ipBuffer, uint32_t start, uint32_t end)
{
	ipBuffer[0] = 0;

	switch(end)
	{
		/*
		 * IPv6 Address with prefix length : 2001:db8:abcd:0012::0/64
		 * Range :: 2001:0DB8:ABCD:0012:0000:0000:0000:0000 ~ 2001:0DB8:ABCD:0012:FFFF:FFFF:FFFF:FFFF
		 */
		case 8:
			sprintf(ipBuffer,"%02x%02x:%02x%02x:%02x%02x:%02x%02x",
					packet[start], packet[start+1], packet[start+2], packet[start+3],
					packet[start+4], packet[start+5], packet[start+6], packet[start+7]);
			break;

		/*
		 * IPv6 Address with prefix length : 2001:db8:abcd:0012::0/80
		 * Range :: 2001:0DB8:ABCD:0012:0000:0000:0000:0000 ~ 2001:0DB8:ABCD:0012:0000:FFFF:FFFF:FFFF
		 */

		case 10:
			sprintf(ipBuffer,"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
					packet[start], packet[start+1], packet[start+2], packet[start+3],
					packet[start+4], packet[start+5], packet[start+6], packet[start+7],
					packet[start+8], packet[start+9]);
			break;

		/*
		 * IPv6 Address with prefix length : 2001:db8:abcd:0012::0/96
		 * Range :: 2001:0DB8:ABCD:0012:0000:0000:0000:0000 ~ 2001:0DB8:ABCD:0012:0000:0000:FFFF:FFFF
		 */

		case 12:
			sprintf(ipBuffer,"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
					packet[start], packet[start+1], packet[start+2], packet[start+3],
					packet[start+4], packet[start+5], packet[start+6], packet[start+7],
					packet[start+8], packet[start+9], packet[start+10], packet[start+11]);
			break;

		/*
		 * IPv6 Address with prefix length : 2001:db8:abcd:0012::0/112
		 * Range :: 2001:0DB8:ABCD:0012:0000:0000:0000:0000 ~ 2001:0DB8:ABCD:0012:0000:0000:0000:FFFF
		 */

		case 14:
			sprintf(ipBuffer,"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
					packet[start], packet[start+1], packet[start+2], packet[start+3],
					packet[start+4], packet[start+5], packet[start+6], packet[start+7],
					packet[start+8], packet[start+9], packet[start+10], packet[start+11],
					packet[start+12], packet[start+13]);
			break;

		/*
		 * IPv6 Address with prefix length : 2001:db8:abcd:0012::0/128
		 * Range :: 2001:0DB8:ABCD:0012:0000:0000:0000:0000 ~ 2001:0DB8:ABCD:0012:0000:0000:0000:0000
		 */

		case 16:
			sprintf(ipBuffer,"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
					packet[start], packet[start+1], packet[start+2], packet[start+3],
					packet[start+4], packet[start+5], packet[start+6], packet[start+7],
					packet[start+8], packet[start+9], packet[start+10], packet[start+11],
					packet[start+12], packet[start+13], packet[start+14], packet[start+15]);
			break;

		default:
			return;
	}
}

void PUtility::pinThread(pthread_t th,int core_num)
{
	   cpu_set_t cpuset;

           /* Set affinity mask to include CPUs 0 to 7 */

           CPU_ZERO(&cpuset);
           CPU_SET(core_num,&cpuset);

           int s = pthread_setaffinity_np(th, sizeof(cpu_set_t), &cpuset);
           if (s != 0)
               handle_error_en(s, "pthread_setaffinity_np");

           /* Check the actual affinity mask assigned to the thread */

           s = pthread_getaffinity_np(th, sizeof(cpu_set_t), &cpuset);
           if (s != 0)
               handle_error_en(s, "pthread_getaffinity_np");

           printf("Set returned by pthread_getaffinity_np() contained:\n");
           if (CPU_ISSET(core_num, &cpuset))
               printf("    CPU %d\n", core_num);

}

void PUtility::printPacketBytes(const BYTE packet, int size)
{
		for(int i=0;i<size;i++){
			if((i%16 == 0)) printf("\n	%04x ", i);
			printf("%02x ", packet[i]);
		}
		printf("\n\n");
}

vector<string> PUtility::split(string str, char delimiter)
{
	vector<string> internal;
	stringstream ss(str);
	string token;

	while(getline(ss, token, delimiter)) {
	    internal.push_back(token);
	}
	return internal;
}

void PUtility::HEXDUMP(const void* pv, int len)
{
	const unsigned char* p = (const unsigned char*) pv;
	int i;
	for( i = 0; i < len; ++i ) {
		const char* eos;
			switch( i & 15 ) {
				case 0:
					printf("%08x  ", i);
					eos = "";
					break;
				case 1:
					eos = " ";
					break;
				case 15:
					eos = "\n";
					break;
				default:
					eos = (i & 1) ? " " : "";
					break;
			}
			printf("%02x%s", (unsigned) p[i], eos);
	}
	printf(((len & 15) == 0) ? "\n" : "\n\n");
}

char* PUtility::getByteToHex(const void* pv, int len)
{
	char datadump[10000];
	char buffer[20];

	datadump[0] = buffer[0] = 0;

	const unsigned char* p = (const unsigned char*) pv;
		int i;
		for( i = 0; i < len; ++i ) {
			const char* eos;
				switch( i & 15 ) {
					case 0:
						sprintf(buffer,"%08x  ", i);
						strcat(datadump,buffer);
						buffer[0] = 0;
						eos = "";
						break;
					case 1:
						eos = " ";
						break;
					case 15:
						eos = "\n";
						break;
					default:
						eos = (i & 1) ? " " : "";
						break;
				}
				sprintf(buffer, "%02x%s", (unsigned) p[i], eos);
				strcat(datadump,buffer);
				buffer[0] = 0;
		}
		//printf(((len & 15) == 0) ? "\n" : "\n\n");

	return datadump;
}

void PUtility::writePcap(char *pcapFileName, const BYTE packet, uint64_t counter, int length, long tv_sec)
{
	char fName[10];
	fName[0] = 0;

	struct pcapPkthdr {
		uint32_t tv_sec;
		uint32_t tv_usec;
		uint32_t caplen;
		uint32_t len;
	 };

	FILE *ptr_myfile;
	sprintf(fName, "%lu-%s", counter, pcapFileName);
	ptr_myfile=fopen(fName, "wb");

	if (!ptr_myfile)
		printf("Unable to open file [%s] !\n", fName);


	struct pcap_file_header  fileHeader;
	fileHeader.magic = 0xa1b2c3d4;
	fileHeader.version_major = 2;
	fileHeader.version_minor = 4;
	fileHeader.thiszone = 0;
	fileHeader.sigfigs = 0;
	fileHeader.snaplen = 65535; //(2^16)
	fileHeader.linktype  = 1;     //Ethernet

	fwrite(&fileHeader, sizeof(fileHeader), 1, ptr_myfile);

	pcapPkthdr pkhdr;
	pkhdr.len = pkhdr.caplen = (uint32_t)length;
	pkhdr.tv_sec = tv_sec;

	fwrite(&pkhdr, sizeof(pkhdr), 1, ptr_myfile);
	fwrite(packet, length, 1, ptr_myfile);

	fclose(ptr_myfile);

	printf("pcap file written for length %d...\n", length);

	ifstream::pos_type size;
	char * memblock;

	ifstream file (fName, ios::in|ios::binary|ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char [size];
		file.seekg (0, ios::beg);
		file.read (memblock, size);
		file.close();

		PUtility::HEXDUMP(memblock,size);
	}
}
