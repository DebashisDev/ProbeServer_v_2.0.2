/*
 * AaaSMInterface.cpp
 *
 *  Created on: Oct 22, 2016
 *      Author: Debashis
 */

#include "AaaSMInterface.h"

AaaSMInterface::AaaSMInterface(uint16_t id)
{
	this->_name = "AaaSMInterface";
	this->setLogLevel(Log::theLog().level());

	this->instanceId = id;
	this->timeIndex = 0;
	this->flusherId = 0;
	this->aaaKey	= 0;
}

AaaSMInterface::~AaaSMInterface()
{}

void AaaSMInterface::lockAAAMap()
{
	pthread_mutex_lock(&mapAAALock::lockCount);
	while (mapAAALock::count == 0)
		pthread_cond_wait(&mapAAALock::nonzero, &mapAAALock::lockCount);
	mapAAALock::count = mapAAALock::count - 1;
	pthread_mutex_unlock(&mapAAALock::lockCount);
}

void AaaSMInterface::unLockAAAMap()
{
    pthread_mutex_lock(&mapAAALock::lockCount);
    if (mapAAALock::count == 0)
        pthread_cond_signal(&mapAAALock::nonzero);
    mapAAALock::count = mapAAALock::count + 1;
    pthread_mutex_unlock(&mapAAALock::lockCount);
}


void AaaSMInterface::packetEntry(AAAPacket *aaaObj)
{
	switch(aaaObj->code)
	{
			case AAA_ACCESS_REQUEST:
							processAccessRequest(aaaObj);
							break;

			case AAA_ACCESS_ACCEPT:
			case AAA_ACCESS_REJECT:
							processAccessResponse(aaaObj);
							break;

			case AAA_ACCOUNTING_REQUEST:
							processAccountingRequest(aaaObj);
							break;

			case AAA_ACCOUNTING_RESPONSE:
							processAccountingResponse(aaaObj);
							break;
			default:
							break;
	}
}

void AaaSMInterface::processAccessRequest(AAAPacket *aaaObj)
{
	bool found = false;

	aaaSession *pRadiusSession = getAccessSession(aaaObj, &found);

	if(found)		/* Record Already Exist with Response*/
	{
		if(pRadiusSession->respCode == AAA_ACCESS_ACCEPT || pRadiusSession->respCode == AAA_ACCESS_REJECT)
		{
			createSession(pRadiusSession, aaaObj);
			flushSession(pRadiusSession, true, FLUSH_RSP_REQ);
		}
	}
	else if(!found) 	/* New Request Packet */
	{ createSession(pRadiusSession, aaaObj); }
}

void AaaSMInterface::processAccessResponse(AAAPacket *aaaObj)
{
	bool found = false;
	aaaSession *pRadiusSession;

	pRadiusSession = getAccessSession(aaaObj, &found);

	if(found) 		/* Found Request to Response */
	{
		if(pRadiusSession->reqCode == AAA_ACCESS_REQUEST)
		{
			switch(aaaObj->code)
			{
				case AAA_ACCESS_ACCEPT:
				case AAA_ACCESS_REJECT:
								updateSession(pRadiusSession, aaaObj);
								flushSession(pRadiusSession, true, FLUSH_REQ_RSP);
								break;
			}
		}
	}
	else if(!found)	/* We may not intrested to keep the Access Response Packet (May be blocked)*/
	{ updateSession(pRadiusSession, aaaObj); }
}

void AaaSMInterface::processAccountingRequest(AAAPacket *aaaObj)
{
	bool found = false;
	aaaSession *pAaaSession;

	pAaaSession = getAccountingSession(aaaObj, &found);

	if(found)		/* Record Already Exist with Response*/
	{
		if(pAaaSession->respCode == AAA_ACCOUNTING_RESPONSE)
		{
			createSession(pAaaSession, aaaObj);
			updateGlbTable(pAaaSession);					/* Update IP and User Id Map */
			flushSession(pAaaSession, true, FLUSH_RSP_REQ);
		}
	}
	else if(!found) 			/* New Request Packet */
	{ createSession(pAaaSession, aaaObj); }
}

void AaaSMInterface::processAccountingResponse(AAAPacket *aaaObj)
{
	bool found = false;
	aaaSession *pAaaSession;

	pAaaSession = getAccountingSession(aaaObj, &found);

	if(found) 		/* Found Request to Response */
	{
		if(pAaaSession->reqCode == AAA_ACCOUNTING_REQUEST)
		{
			updateSession(pAaaSession, aaaObj);
			updateGlbTable(pAaaSession);
			flushSession(pAaaSession, true, FLUSH_REQ_RSP);
		}
	}
	else if(!found)		/* We may not intrested to keep the Accounting Response Packet (May be blocked) */
	{ updateSession(pAaaSession, aaaObj); }
}

void AaaSMInterface::glbSessionCount()
{
	TheLog_nc_v2(Log::Info, name(),"   Radius GLB User      IPv4[ID]: %08d | IPv4[IP]: %08d",
				aaaGlbMap::aaaGlbUserIdMap.size(),
				aaaGlbMap::aaaGlbUserIpMap.size());
}

aaaSession* AaaSMInterface::getAccessSession(AAAPacket *aaaObj, bool *found)
{
	aaaSession *pAaaSession = NULL;
	uint16_t idx = aaaObj->identifier;

	aaaKey = 0;

	getSessionKey(aaaObj, aaaKey);

	std::map<uint64_t, aaaSession>::iterator it = accessMap[idx].find(aaaKey);
	if(it != accessMap[idx].end())
	{
		pAaaSession = &it->second;
		*found = true;
	}
	else
	{
		aaaSession newAaaObj;
		newAaaObj.aaaKey = aaaKey;
		newAaaObj.mapIndex = idx;
		accessMap[idx][aaaKey] = newAaaObj;
		pAaaSession = &accessMap[idx][aaaKey];
		*found = false;
	}
	return pAaaSession;
}

aaaSession* AaaSMInterface::getAccountingSession(AAAPacket *aaaObj, bool *found)
{
	aaaSession *pAaaSession = NULL;
	uint16_t idx = aaaObj->identifier;

	aaaKey = 0;

	getSessionKey(aaaObj, aaaKey);

	std::map<uint64_t, aaaSession>::iterator it = accountingMap[idx].find(aaaKey);
	if(it != accountingMap[idx].end()) {
		pAaaSession = &it->second;
		*found = true;
	}
	else
	{
			aaaSession newAaaObj;
			newAaaObj.aaaKey = aaaKey;
			newAaaObj.mapIndex = idx;
			accountingMap[idx][aaaKey] = newAaaObj;
			pAaaSession = &accountingMap[idx][aaaKey];
			*found = false;
	}
	return pAaaSession;
}

void AaaSMInterface::createSession(aaaSession *pAaaSession, AAAPacket *aaaObj)
{
	pAaaSession->ipVer					= aaaObj->ipVer;
	pAaaSession->StartTimeEpochSec 		= aaaObj->frTimeEpochSec;
	pAaaSession->StartTimeEpochMiliSec 	= aaaObj->frTimeEpochMilliSec;
	pAaaSession->sourceAddr				= aaaObj->sIp;
	pAaaSession->destAddr				= aaaObj->dIp;
	pAaaSession->sPort 					= aaaObj->sPort;
	pAaaSession->dPort 					= aaaObj->dPort;
	pAaaSession->reqCode 				= aaaObj->code;
	pAaaSession->protocol 				= aaaObj->protocol;
	pAaaSession->serviceType 			= aaaObj->serviceType;
	pAaaSession->accStatusType 			= aaaObj->accStatusType;
	pAaaSession->accTerminationCause 	= aaaObj->terminationCause;

	pAaaSession->packetIdentifier		= aaaObj->identifier;
	pAaaSession->accAuth				= aaaObj->accAuth;
	pAaaSession->framedIpLong			= aaaObj->framedIp;
	strcpy(pAaaSession->userName, 		aaaObj->userName);

	if(aaaObj->code == AAA_ACCOUNTING_REQUEST)
	{
		pAaaSession->inputOctets 		= aaaObj->inputOctets;
		pAaaSession->outputOctets		= aaaObj->outputOctets;
		pAaaSession->inputPackets		= aaaObj->inputPackets;
		pAaaSession->outputPackets		= aaaObj->outputPackets;
		pAaaSession->inputGigaWords		= aaaObj->inputGigaWords;
		pAaaSession->outputGigaWords	= aaaObj->outputGigaWords;
	}
}

void AaaSMInterface::updateSession(aaaSession *pAaaSession, AAAPacket *aaaObj)
{
	pAaaSession->respCode				= aaaObj->code;
	pAaaSession->EndTimeEpochSec 		= aaaObj->frTimeEpochSec;
	pAaaSession->EndTimeEpochMiliSec 	= aaaObj->frTimeEpochMilliSec;

	if(strstr(aaaObj->replyMsg, "password") != NULL)
		strcpy(pAaaSession->replyMsg, "password check failed");
	else if(strstr(aaaObj->replyMsg, "account") != NULL)
		strcpy(pAaaSession->replyMsg, "account not active");
	else if(strstr(aaaObj->replyMsg, "profile") != NULL)
		strcpy(pAaaSession->replyMsg, "user profile not found");
	else if(strstr(aaaObj->replyMsg, "mac") != NULL)
		strcpy(pAaaSession->replyMsg, "mac validation failed");
    else if(strstr(aaaObj->replyMsg, "Password") != NULL)
    	strcpy(pAaaSession->replyMsg, "password check failed");
    else if(strstr(aaaObj->replyMsg, "primary") != NULL)
    	strcpy(pAaaSession->replyMsg, "primary service rule not satisfied");
    else if(strstr(aaaObj->replyMsg, "Welcome") != NULL)
    	strcpy(pAaaSession->replyMsg, "Welcome");
    else
    	strcpy(pAaaSession->replyMsg, aaaObj->replyMsg);

	if(strcmp(pAaaSession->replyMsg, "NA") == 0)
	{
		switch(pAaaSession->respCode)
		{
			case AAA_ACCESS_ACCEPT:	/* Access-Accept */
					strcpy(pAaaSession->replyMsg, "Welcome");
					break;

			case AAA_ACCESS_REJECT:	/* Access-Reject */
					strcpy(pAaaSession->replyMsg, "account not active");
					break;
		}
	}
}

void AaaSMInterface::eraseSession(aaaSession *pAaaSession)
{
	switch(pAaaSession->reqCode)
	{
		case AAA_ACCESS_REQUEST:
		case AAA_ACCESS_ACCEPT:
		case AAA_ACCESS_REJECT:
						accessMap[pAaaSession->mapIndex].erase(pAaaSession->aaaKey);
						break;

		case AAA_ACCOUNTING_REQUEST:
		case AAA_ACCOUNTING_RESPONSE:
						accountingMap[pAaaSession->mapIndex].erase(pAaaSession->aaaKey);
						break;
	}
}

void AaaSMInterface::updateGlbTable(aaaSession *pAaaSession)
{
	char userIpAddress[IPV6_ADDR_LEN];

	userIpAddress[0] = 0;

	/* Don't Store the User Name Having MAC Address */
	if(strstr(pAaaSession->userName, ":") != NULL || strcmp(userIpAddress , "0.0.0.0") == 0)
		return;

	long2Ip(pAaaSession->framedIpLong, userIpAddress);

	if(strlen(pAaaSession->userName) < 3)
		return;

	/* Store with User IP */
	lockAAAMap();

	switch(pAaaSession->accStatusType)
	{
		case START_STATUS_TYPE:	/* START (1) */
			{
				TheLog_nc_v2(Log::Trace, name()," START: User: %16s| IP: %16s", pAaaSession->userName, userIpAddress);

				std::map<std::string, userInfo>::iterator userIdIt = aaaGlbMap::aaaGlbUserIdMap.find(pAaaSession->userName);

				/* If we get UPDATE request first than START request */
				if(userIdIt != aaaGlbMap::aaaGlbUserIdMap.end())
				{
					TheLog_nc_v1(Log::Trace, name()," START: User: %16s found in UserId Map", pAaaSession->userName);
					if(userIdIt->second.allocatedIpLong != pAaaSession->framedIpLong)
					{
						TheLog_nc_v1(Log::Trace, name()," START: User: %16s Ip Not same, Somehow missed STOP Request", pAaaSession->userName);

						std::map<uint32_t, userInfo>::iterator userIpIt = aaaGlbMap::aaaGlbUserIpMap.find(userIdIt->second.allocatedIpLong);
						if(userIpIt != aaaGlbMap::aaaGlbUserIpMap.end())
							aaaGlbMap::aaaGlbUserIpMap.erase(userIdIt->second.allocatedIpLong);

						userInfo userinfo;

						userinfo.allocatedIpLong = pAaaSession->framedIpLong;
						userinfo.oldAllocatedIpLong = userIdIt->second.allocatedIpLong;
						strcpy(userinfo.userName, pAaaSession->userName);

						aaaGlbMap::aaaGlbUserIpMap[userinfo.allocatedIpLong] = userinfo;

						TheLog_nc_v1(Log::Trace, name()," User: %16s Updating UserId & UserIP Map", pAaaSession->userName);

						userIdIt->second.oldAllocatedIpLong = userIdIt->second.allocatedIpLong;
						userIdIt->second.allocatedIpLong = pAaaSession->framedIpLong;
						strcpy(userIdIt->second.userName, pAaaSession->userName);
					}
					else
					{ TheLog_nc_v1(Log::Trace, name()," START: User: %16s Ip same, do noting.", pAaaSession->userName); }
				}
				else
				{
					TheLog_nc_v2(Log::Trace, name()," START: New User: %16s| IP: %16s", pAaaSession->userName, userIpAddress);

					userInfo userinfo;
					userinfo.oldAllocatedIpLong = 0;
					userinfo.allocatedIpLong = pAaaSession->framedIpLong;
					strcpy(userinfo.userName, pAaaSession->userName);

					aaaGlbMap::aaaGlbUserIdMap[userinfo.userName] = userinfo;

					/* Delete the data corresponding to frameIP in IPMap */
					std::map<uint32_t, userInfo>::iterator userIpIt = aaaGlbMap::aaaGlbUserIpMap.find(pAaaSession->framedIpLong);
					if(userIpIt != aaaGlbMap::aaaGlbUserIpMap.end())
						aaaGlbMap::aaaGlbUserIpMap.erase(pAaaSession->framedIpLong);

					aaaGlbMap::aaaGlbUserIpMap[userinfo.allocatedIpLong] = userinfo;
				}
			}
			break;

		case STOP_STATUS_TYPE:	/* STOP */
			{
				TheLog_nc_v1(Log::Trace, name()," STOP: User: %16s", pAaaSession->userName);
				std::map<std::string, userInfo>::iterator userIdIt = aaaGlbMap::aaaGlbUserIdMap.find(pAaaSession->userName);

				if(userIdIt != aaaGlbMap::aaaGlbUserIdMap.end())
				{
					TheLog_nc_v1(Log::Trace, name()," STOP: User: %16s found in UserId and UserIp Map Deleting.", pAaaSession->userName);

					std::map<uint32_t, userInfo>::iterator userIpIt = aaaGlbMap::aaaGlbUserIpMap.find(userIdIt->second.allocatedIpLong);
					if(userIpIt != aaaGlbMap::aaaGlbUserIpMap.end())
						aaaGlbMap::aaaGlbUserIpMap.erase(userIdIt->second.allocatedIpLong);

					aaaGlbMap::aaaGlbUserIdMap.erase(pAaaSession->userName);
				}
			}
			break;

		case UPDATE_STATUS_TYPE:	/* UPDATE */
			{
				TheLog_nc_v2(Log::Trace, name()," UPDATE: User: %16s| IP: %16s", pAaaSession->userName, userIpAddress);

				std::map<std::string, userInfo>::iterator userIdIt = aaaGlbMap::aaaGlbUserIdMap.find(pAaaSession->userName);
				if(userIdIt != aaaGlbMap::aaaGlbUserIdMap.end())
				{
					if(userIdIt->second.allocatedIpLong != pAaaSession->framedIpLong)
					{
						TheLog_nc_v1(Log::Trace, name()," UPDATE: User: %16s found in UserId Map, but allocated and frame IP are not same.", pAaaSession->userName);

						userIdIt->second.oldAllocatedIpLong = userIdIt->second.allocatedIpLong;
						userIdIt->second.allocatedIpLong = pAaaSession->framedIpLong;


						TheLog_nc_v1(Log::Trace, name()," UPDATE: User: %16s Reinitializing UserIp map.", pAaaSession->userName);

						std::map<uint32_t, userInfo>::iterator userIpIt = aaaGlbMap::aaaGlbUserIpMap.find(userIdIt->second.oldAllocatedIpLong);
						if(userIpIt != aaaGlbMap::aaaGlbUserIpMap.end())
							aaaGlbMap::aaaGlbUserIpMap.erase(userIdIt->second.oldAllocatedIpLong);

						userInfo userinfo;

						userinfo.allocatedIpLong = pAaaSession->framedIpLong;
						userinfo.oldAllocatedIpLong = userIdIt->second.oldAllocatedIpLong;

						strcpy(userinfo.userName, pAaaSession->userName);

						aaaGlbMap::aaaGlbUserIpMap[userinfo.allocatedIpLong] = userinfo;
					}
					else
					{ TheLog_nc_v1(Log::Trace, name()," UPDATE: User: %16s found in UserId Map, allocated and frame IP are same, do noting.", pAaaSession->userName); }
				}
				else
				{
					TheLog_nc_v2(Log::Trace, name()," UPDATE: New User: %16s| IP: %16s", pAaaSession->userName, userIpAddress);

					userInfo userinfo;

					userinfo.oldAllocatedIpLong = 0;
					userinfo.allocatedIpLong = pAaaSession->framedIpLong;
					strcpy(userinfo.userName, pAaaSession->userName);

					aaaGlbMap::aaaGlbUserIdMap[userinfo.userName] = userinfo;

					std::map<uint32_t, userInfo>::iterator userIpIt = aaaGlbMap::aaaGlbUserIpMap.find(userinfo.allocatedIpLong);
					if(userIpIt != aaaGlbMap::aaaGlbUserIpMap.end())
						aaaGlbMap::aaaGlbUserIpMap.erase(userinfo.allocatedIpLong);

					aaaGlbMap::aaaGlbUserIpMap[userinfo.allocatedIpLong] = userinfo;
				}
			}
			break;

		default:
			break;
	}

	unLockAAAMap();

	if(pAaaSession->ipv6AddressPrefixFlag)
	{
		/* Store with IPv6 User IP */
		lockAAAMap();

		std::map<std::string, userInfo>::iterator it = aaaGlbMap::aaaGlbIpv6UserMap.find(pAaaSession->userIpV6);

		if(it != aaaGlbMap::aaaGlbIpv6UserMap.end())
		{
			if(std::string(it->second.userName).compare(std::string(pAaaSession->userName)) != 0)
			{
				aaaGlbMap::aaaGlbIpv6UserMap.erase(pAaaSession->userIpV6);
				userInfo userinfo;

				strcpy(userinfo.userName, pAaaSession->userName);
				aaaGlbMap::aaaGlbIpv6UserMap[pAaaSession->userIpV6] = userinfo;
			}
		}
		else
		{
			userInfo userinfo;

			strcpy(userinfo.userName, pAaaSession->userName);

			aaaGlbMap::aaaGlbIpv6UserMap[std::string(pAaaSession->userIpV6)] = userinfo;
		}
		unLockAAAMap();
	}
}

void AaaSMInterface::timeOutCleanSession()
{
	IPStats::aaaAccessSessionCnt[instanceId] 	 = 0;
	IPStats::aaaAccessSessionScanned[instanceId] = 0;
	IPStats::aaaAccessSessionCleaned[instanceId] = 0;

	IPStats::aaaAccounSessionCnt[instanceId] 	 = 0;
	IPStats::aaaAccounSessionScanned[instanceId] = 0;
	IPStats::aaaAccounSessionCleaned[instanceId] = 0;

	for(uint16_t i = 0; i < AAA_SESSION_POOL_ARRAY_SIZE; i++)
	{
		IPStats::aaaAccessSessionCnt[instanceId] += accessMap[i].size();

		for(auto elem : accessMap[i])
		{
			IPStats::aaaAccessSessionScanned[instanceId]++;
			cleanSession(elem.first, &elem.second, i);
		}
	}
	IPStats::aaaAccessSessionCleaned[instanceId] = sessionCleanCount;
	sessionCleanCount = 0;

	eraseAccessSession();

	for(uint16_t i = 0; i < AAA_SESSION_POOL_ARRAY_SIZE; i++)
	{
		IPStats::aaaAccounSessionCnt[instanceId] += accountingMap[i].size();

		for(auto elem : accountingMap[i])
		{
			IPStats::aaaAccounSessionScanned[instanceId]++ ;
			cleanSession(elem.first, &elem.second, i);
		}
	}
	IPStats::aaaAccounSessionCleaned[instanceId] = sessionCleanCount;
	eraseAccountingSession();
}

void AaaSMInterface::cleanSession(uint32_t key, aaaSession *pAaaSession, uint16_t count)
{
	uint64_t curEpochSec = Global::CURRENT_EPOCH_SEC;
	bool eraseFlag = false;

	uint32_t IdleTimeSec = SMGlobal::AAA_CLEAN_UP_TIMEOUT_SEC;

	if(((pAaaSession->StartTimeEpochSec > 0) && ((curEpochSec - pAaaSession->StartTimeEpochSec) > IdleTimeSec)) ||
			((pAaaSession->EndTimeEpochSec > 0) && ((curEpochSec - pAaaSession->EndTimeEpochSec) > IdleTimeSec)))
	{
		cleanMap[pAaaSession->aaaKey] = count;
		sessionCleanCount++;
	}
}

void AaaSMInterface::eraseAccessSession()
{
	std::string key = "";
	aaaSession *pRadiusSession;

	for(auto elem : cleanMap)
		accessMap[elem.second].erase(elem.first);

	cleanMap.clear();
	sessionCleanCount = 0;
}

void AaaSMInterface::eraseAccountingSession()
{
	std::string key = "";
	aaaSession *pRadiusSession;

	for(auto elem : cleanMap)
		accountingMap[elem.second].erase(elem.first);

	cleanMap.clear();
	sessionCleanCount = 0;
}

void AaaSMInterface::flushSession(aaaSession *pAaaSession, bool erase, uint16_t flushType)
{
	uint64_t epochSecNow = Global::CURRENT_EPOCH_SEC;

	pAaaSession->flushTime = epochSecNow;
	pAaaSession->flushType = flushType;

	timeIndex = PKT_WRITE_TIME_INDEX(epochSecNow , Global::TIME_INDEX);
	storeSession(timeIndex, pAaaSession);

	if(erase)
		eraseSession(pAaaSession);
}

void AaaSMInterface::storeSession(uint16_t timeIndex, aaaSession *pAaaSession)
{
	flusherId = instanceId % SMGlobal::AAA_FLUSHER_INSTANCE;

	FlushRepo::aaaFlStore[flusherId][instanceId][timeIndex][FlushRepo::aaaFlCnt[flusherId][instanceId][timeIndex]].copy(pAaaSession);
	FlushRepo::aaaFlCnt[flusherId][instanceId][timeIndex]++;
}

void AaaSMInterface::getSessionKey(AAAPacket *aaaObj, uint64_t &aaaKey)
{
	if(aaaObj->code == AAA_ACCESS_REQUEST || aaaObj->code == AAA_ACCOUNTING_REQUEST)
	{
		aaaKey = (aaaObj->sIp*59)^(aaaObj->dIp)^(aaaObj->sPort << 16)^(aaaObj->dPort)^(aaaObj->identifier);
		aaaObj->direction = UP;
	}
	else if(aaaObj->code == AAA_ACCESS_ACCEPT || aaaObj->code == AAA_ACCESS_REJECT||aaaObj->code == AAA_ACCOUNTING_RESPONSE) {

		aaaKey = (aaaObj->dIp*59)^(aaaObj->sIp)^(aaaObj->dPort << 16)^(aaaObj->sPort)^(aaaObj->identifier);
		aaaObj->direction = DOWN;
	}
}

void AaaSMInterface::aaaDumpUserInfo(uint16_t hour)
{
	char fileName[50];
	fileName[0] = 0;
	userInfo	*pUserInfo;

	int noOfRecords = 0;

	/* Dumping File Hourly */
	sprintf(fileName, "%s_%02d.csv", "radiusUserIpv4", hour);
	string filePath = SMGlobal::DATA_DIR[0] + string(fileName);

	printf("AaaSMInterface [%d] Dumping Hourly Radius IPv4 User to file [%s]...\n", instanceId, filePath.c_str());
	TheLog_nc_v2(Log::Info, name()," [%d] Dumping Hourly Radius IPv4 User to file [%s]...", instanceId, filePath.c_str());

	ofstream fileHandler;

	fileHandler.open(filePath.c_str(), ios :: out);

	if(fileHandler.fail())
	{
		printf("AaaSMInterface [%d] Error in Dumping Hourly Radius IPv4 User Store File : %s\n", instanceId, filePath.c_str());
		TheLog_nc_v2(Log::Warn, name()," [%d] Error in Dumping Hourly Radius IPv4 User Store to file [%s]...", instanceId, filePath.c_str());
	}
	else
	{
		for(auto elem : aaaGlbMap::aaaGlbUserIpMap)
		{
			if(strlen(elem.second.userName) > 2)
			{
				fileHandler << elem.first << ","						// User IP
						    << elem.second.userName <<  endl;			// User Id

				noOfRecords ++;
			}
		}

		fileHandler.close();
		printf("AaaSMInterface [%d] Dumping [%d] Hourly Radius IPv4 User to file [%s]...Completed\n", instanceId, noOfRecords, filePath.c_str());
		TheLog_nc_v3(Log::Info, name()," [%d] Dumping [%d] Hourly Radius IPv4 User to file [%s]...Completed", instanceId, noOfRecords, filePath.c_str());

		filePath.clear();
	}

	/* Current Dumping File */
	filePath = SMGlobal::DATA_DIR[0] + "radiusUserIpv4.csv";
	fileHandler.open(filePath.c_str(), ios :: out);

	if(fileHandler.fail())
	{
		printf("AaaSMInterface [%d] Error in Dumping Current Radius IPv4 User Store File : %s\n", instanceId, filePath.c_str());
		TheLog_nc_v2(Log::Warn, name()," [%d] Error in Dumping Current Radius IPv4 User Store to file [%s]...", instanceId, filePath.c_str());
	}
	else
	{
		for(auto elem : aaaGlbMap::aaaGlbUserIpMap)
		{
			if(strlen(elem.second.userName) > 2)
			{
				fileHandler << elem.first << ","					// User IP
						    << elem.second.userName <<  endl;		// User Id
			}
		}

		fileHandler.close();
		printf("AaaSMInterface [%d] Dumping Current Radius IPv4 User to file [%s]...Completed\n",instanceId,filePath.c_str());
		TheLog_nc_v2(Log::Info, name()," [%d] Dumping Current Radius IPv4 User to file [%s]...Completed",instanceId,filePath.c_str());

		filePath.clear();
	}
}

void AaaSMInterface::aaaLoadUserInfo()
{
	string filePath =SMGlobal::DATA_DIR[0] + "radiusUserIpv4.csv";

	std::string line;
	uint32_t userIp;
	std::string userId, userPlan, userPolicyPlan, OLT;
	int lineCnt = 0, loadedCnt = 0;
	char* 		pch;

	ifstream fp;

	fp.open(filePath.c_str());

	if(fp.fail())
	{
		printf("AaaSMInterface [%d] Error in Loading IPv4 User file [%s]\n", instanceId, filePath.c_str());
		TheLog_nc_v2(Log::Warn, name()," [%d] Error in Loading IPv4 User file [%s]", instanceId, filePath.c_str());
	}
	else
	{
		while(!fp.eof())
		{
			getline(fp,line);
			lineCnt++;

			if(!line.empty() && line.length() > 0)
			{
				pch = strtok ((char *)line.c_str(), ",");
				if(pch == NULL) break;

				userIp = atol(pch);

				pch = strtok (NULL, ",");
				if(pch == NULL) break;

				userId = std::string(pch);

				if(userId.length() > 2)
				{
					strcpy(aaaGlbMap::aaaGlbUserIpMap[userIp].userName, userId.c_str());
					aaaGlbMap::aaaGlbUserIpMap[userIp].allocatedIpLong = (long)userIp;

					strcpy(aaaGlbMap::aaaGlbUserIdMap[userId].userName, userId.c_str());
					aaaGlbMap::aaaGlbUserIdMap[userId].allocatedIpLong = (long)userIp;
				}
				loadedCnt++;
			}
		}
		fp.close();
	}

	printf("AaaSMInterface [%d] Loaded [%d] records in IPv4 User Map from file [%s] reading [%d] lines\n",instanceId, loadedCnt, filePath.c_str(), lineCnt);
	TheLog_nc_v4(Log::Info, name()," [%d] Loaded [%d] records in IPv4 User Map from file [%s] reading [%d] lines",instanceId, loadedCnt, filePath.c_str(), lineCnt);
}
