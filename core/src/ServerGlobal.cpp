/*
 * ServerGlobal.cpp
 *
 *  Created on: 29 Sep 2021
 *      Author: Debashis
 */

#include "ServerGlobal.h"

ServerGlobal::ServerGlobal()
{}

ServerGlobal::~ServerGlobal()
{}

namespace Global
{
	uint16_t	TIME_INDEX 				= 10;
	uint16_t 	CURRENT_SEC 			= 0;
	uint16_t 	CURRENT_MIN 			= 0;
	uint16_t 	CURRENT_HOUR 			= 0;
	uint16_t 	CURRENT_DAY 			= 0;
	uint16_t	CURRENT_MONTH 			= 0;
	uint16_t 	CURRENT_YEAR 			= 0;
	uint64_t	CURRENT_EPOCH_SEC 		= 0;
	uint64_t 	CURRENT_EPOCH_NANO_SEC 	= 0;
	uint64_t 	CURRENT_EPOCH_MILI_SEC 	= 0;
}
