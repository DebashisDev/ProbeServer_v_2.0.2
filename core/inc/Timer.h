/*
 * glbTimer.h
 *
 *  Created on: Feb 6, 2019
 *      Author: Debashis
 */

#ifndef CORE_SRC_GLBTIMER_H_
#define CORE_SRC_GLBTIMER_H_

#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "ServerGlobal.h"

class Timer
{
	private:
		bool 		timerReadyState;
		timeval 	curTime;

	public:
		Timer();
		~Timer();
		void run();
		bool isGlbTimerInitialized();
};

#endif /* CORE_SRC_GLBTIMER_H_ */
