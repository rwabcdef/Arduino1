/*
 * DebugPrint.hpp
 *
 *  Created on: 25 Jun 2023
 *      Author: rw123
 */

#include "env.hpp"



#ifndef DEBUGPRINT_HPP_
#define DEBUGPRINT_HPP_

#if defined (ENV_CONFIG__SYSTEM_PC)
#include "TSQueue.hpp"

#define TIMESTAMP_LEN_MAX 64

typedef void (*GetTimestamp)(char* pTimeStamp);

namespace DebugPrint_defs{
#ifndef ENV_CONFIG__DEBUG_LEVELS_OVERRIDE

static const char* const LevelLabels[] = {"ZERO ", "ONE  "};

enum Levels
{
	Null = -1,
	Zero = 0,
	One
};

#endif

#define DEBUGPRINT_STR_LEN 64
}


class DebugStr
{
  public:
    DebugStr(char* str, DebugPrint_defs::Levels level);
    DebugPrint_defs::Levels getLevel();
    char* getStr();
    void clear();
  private:
    DebugPrint_defs::Levels level;
    char str[DEBUGPRINT_STR_LEN + 1];
};


class DebugPrint
{
public:
	DebugPrint(bool enableTimestamp = true);
	DebugPrint(GetTimestamp getTimestamp, bool enableTimestamp = true);
	void writeLine(char* str, DebugPrint_defs::Levels level);
	void setGetTimestamp(GetTimestamp getTimestamp);

private:
	TSQueue<DebugStr> queue;
	bool enableTimestamp;
	GetTimestamp getTimestamp;
	char timestampStr[TIMESTAMP_LEN_MAX];
};

#endif

#endif /* DEBUGPRINT_HPP_ */
