/*
 * DebugPrint.cpp
 *
 *  Created on: 25 Jun 2023
 *      Author: rw123
 */

#include "DebugPrint.hpp"
#if defined(ENV_CONFIG__SYSTEM_PC)

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//#if(ENV_CONFIG__SYSTEM == ENV_CONFIG__SYSTEM_PC)
#include <chrono>

//DebugPrint debugPrint(&MakeTimestamp);

//const char* const LevelLabels[] = {"ZERO", "ONE ", "TWO "};

DebugStr::DebugStr(char* str, DebugPrint_defs::Levels level)
{
  this->level = level;
  int n = strlen(str);
  if(n < DEBUGPRINT_STR_LEN)
  {
    memset(this->str, 0, DEBUGPRINT_STR_LEN + 1);
    strncpy(this->str, str, n);
  }
}

DebugPrint_defs::Levels DebugStr::getLevel(){ return this->level; }

char* DebugStr::getStr() { return this->str; };

void DebugStr::clear()
{
  memset(this->str, 0, DEBUGPRINT_STR_LEN + 1);
}
//------------------------------------------------------------------
DebugPrint::DebugPrint(bool enableTimestamp)
{
	this->enableTimestamp = enableTimestamp;
	this->getTimestamp = NULL;
}

DebugPrint::DebugPrint(GetTimestamp getTimestamp, bool enableTimestamp)
: DebugPrint(enableTimestamp)
{
	this->getTimestamp = getTimestamp;
}

void DebugPrint::writeLine(char* str, DebugPrint_defs::Levels level)
{
  // delay - to not overload printf()
  for(int i=0; i<1000000; i++){
    volatile int n = i * 7;
  }

  DebugStr* pDebugStr = new DebugStr(str, level);
  this->queue.push(*pDebugStr);

  DebugStr debugStr = this->queue.pop();

	if(debugStr.getLevel() != DebugPrint_defs::Levels::Null)
	{
		printf("[%s]: ", DebugPrint_defs::LevelLabels[debugStr.getLevel()]);
	}

	if((true == this->enableTimestamp) && (NULL != this->getTimestamp))
	{
		// Use timestamp
		memset(this->timestampStr, 0, TIMESTAMP_LEN_MAX);
		this->getTimestamp(this->timestampStr);
		printf("%s: ", this->timestampStr);
	}
	else{}
	printf("%s\n", debugStr.getStr());
	/*
	if(true == this->enableTimestamp)
	{
		if(NULL != this->getTimestamp)
		{
			memset(this->timestampStr, 0, TIMESTAMP_LEN_MAX);
			this->getTimestamp(this->timestampStr);

			printf("%s: %s: %s\n", LevelLabels[level], this->timestampStr, str);
		}
		else
		{
			printf("%s\n", str);
		}
	}
	else
	{
		printf("%s\n", str);
	}
	 */
}

void DebugPrint::setGetTimestamp(GetTimestamp getTimestamp)
{
	this->getTimestamp = getTimestamp;
}

#endif
