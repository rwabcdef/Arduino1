/*
 * testSys.cpp
 *
 *  Created on: 26 Jun 2023
 *      Author: rw123
 */

#include <chrono>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Global.hpp"
#include "testSys.hpp"
#include "SimClk.hpp"
#include "InterruptRunner.hpp"
#include "InterruptSchedule.hpp"
#include "ATmega328.hpp"
#include "timer0.h"

using namespace std::chrono;

static bool timestampUsRealTime = false;

void setUseRealTimeInTimestamp(bool b){ timestampUsRealTime = b; }

uint8_t TESTSYS_mode = TESTSYS_TEST_MODE_NONE;

void getIntThreadId_ISR();
void timer0_ISR();

std::thread::id mainThreadId;
std::thread::id intThreadId;
const char* mainThreadIdLabel = "MAIN";
const char* intThreadIdLabel = "INT ";
const char* unknownThreadIdLabel = "UN  ";

//volatile uint32_t timer0_tick_ISR = 0;

//----------------------------------
// Global varaibles (Global.hpp)
volatile uint32_t g_simClk_uS = 0;
DebugPrint *debugPrint;
InterruptRunner *interruptRunner; // = new InterruptRunner(debugPrint);
SimClk *simClk;
//----------------------------------
// DebugPrint *debugPrint = new DebugPrint(&MakeTimestamp);
//InterruptRunner interruptRunner(debugPrint);
//InterruptRunner *interruptRunner;

std::function<void(uint64_t)> tickBind; // = [](uint64_t tick) { interruptRunner->tickRun(tick); };
//SimClk simClk(5000, tickBind, 7854983);


std::list<uint32_t>* lst;// = new std::list<uint32_t>{1};
InterruptSchedule *getIntThreadIdInterrupt;
//InterruptSchedule getIntThreadIdInterrupt(&getIntThreadId_ISR, lst);
//InterruptSchedule timer0Interrupt(&timer0_ISR, 1000);
InterruptSchedule *timer0Interrupt;

high_resolution_clock::time_point g_start;

void initDebug()
{
  InitTimestamp();
  debugPrint = new DebugPrint(&MakeTimestamp);
  //debugPrint = new DebugPrint();
}

void initRun(uint32_t simTicks)
{
  mainThreadId = std::this_thread::get_id();

  //debugPrint = new DebugPrint(&MakeTimestamp);
  // Each simClk tick (uS), runs the configured interrupt routines (ISRs)
  interruptRunner = new InterruptRunner(debugPrint);

  //
  std::function<void(uint64_t)> tickBind = [](uint64_t tick) { interruptRunner->tickRun(tick); };
  simClk = new SimClk(simTicks, tickBind, 7854983);

  lst = new std::list<uint32_t>{1};
  getIntThreadIdInterrupt = new InterruptSchedule(&getIntThreadId_ISR, lst);
  interruptRunner->RegisterInterruptSchedule(getIntThreadIdInterrupt);

  timer0Interrupt = new InterruptSchedule(&timer0_ISR, 1000);
  interruptRunner->RegisterInterruptSchedule(timer0Interrupt);
}

void InitTimestamp()
{
	g_start = high_resolution_clock::now();
}

void initSys(uint32_t simTicks)
{
	//debugPrint = new DebugPrint

	//interruptRunner = new InterruptRunner(debugPrint);

  tickBind = [](uint64_t tick) { interruptRunner->tickRun(tick); };
	simClk = new SimClk(simTicks, tickBind, 7854983);

	mainThreadId = std::this_thread::get_id();

	//std::list<uint32_t>* lst = new std::list<uint32_t>();
	//lst->push_back(1);

	// interrupt to get the thread id of the interrupt thread (i.e. simClk)
	//InterruptSchedule getIntThreadIdInterrupt(&getIntThreadId_ISR, lst);
	//interruptRunner->RegisterInterruptSchedule(&getIntThreadIdInterrupt);

	//InterruptSchedule timer0Interrupt(&timer0_ISR, 1000);
	//interruptRunner->RegisterInterruptSchedule(&timer0Interrupt);

	lst = new std::list<uint32_t>{1};
  getIntThreadIdInterrupt = new InterruptSchedule(&getIntThreadId_ISR, lst);
  interruptRunner->RegisterInterruptSchedule(getIntThreadIdInterrupt);

  timer0Interrupt = new InterruptSchedule(&timer0_ISR, 1000);
  interruptRunner->RegisterInterruptSchedule(timer0Interrupt);

	sei();

	simClk->start();
}

void endRun(){
  simClk->join();
  debugPrint->writeLine("Test End", DebugPrint_defs::Sys);
  delete simClk;
  delete interruptRunner;
  delete getIntThreadIdInterrupt;
  delete timer0Interrupt;
}

void endSys()
{
	simClk->join();
	delete simClk;
	delete interruptRunner;
}

void MakeTimestamp(char* pTimeStamp)
{
  long total, sec, milli;

  if(timestampUsRealTime)
  {
    high_resolution_clock::time_point now = high_resolution_clock::now();
    duration<double> elapsed = now - g_start;
    milliseconds elapsed_mS = duration_cast<milliseconds>(elapsed);

    total = elapsed_mS.count();

    sec = (int)(total / 1000);
    milli = (total % 1000);
  }

	std::thread::id thisThreadId = std::this_thread::get_id();
	char* threadLabel;

	if(thisThreadId == mainThreadId)
	{
		threadLabel = (char*)mainThreadIdLabel;
	}
	else if(thisThreadId == intThreadId)
	{
		threadLabel = (char*)intThreadIdLabel;
	}
	else
	{
		threadLabel = (char*)unknownThreadIdLabel;
	}

	//sprintf(pTimeStamp, "hello", 0); return;

	uint64_t sim_total_uS = simClk->getCurrent();
	uint32_t sim_S = sim_total_uS / 1000000;
	uint32_t sim_mS = (sim_total_uS - (sim_S * 1000000)) / 1000;
	uint64_t sim_uS = sim_total_uS % 1000;

	//sprintf(pTimeStamp, "hello", 0); return;

	//sprintf(pTimeStamp, "(0x%x)%06d:%03d-%03d(%06d)", std::hash<std::thread::id>{}(thisThreadId), total, sec, milli, timer0_tick_ISR);

	if(timestampUsRealTime)
	{
	  sprintf(pTimeStamp, "(%s)%06d:%03d-%03d(%03d.%03d.%03d)(%06d)",
	        threadLabel, total, sec, milli, sim_S, sim_mS, sim_uS, g_timer0_ISR);
	}
	else
	{
	  sprintf(pTimeStamp, "(%s)(%03d.%03d.%03d)(%06d)",
	            threadLabel, sim_S, sim_mS, sim_uS, g_timer0_ISR);
	}


	//strcpy(pTimeStamp, "123");
}

void timer0_ISR()
{
	//timer0_tick_ISR++;
	RUN_ISR(TIMER0_ISR);

//	static char s[32];
//	memset(s, 0, 32);
//	sprintf(s, "timer0 %d", g_timer0_ISR);
//	debugPrint->writeLine(s, DebugPrint_defs::Timer0);
}

// Interrupt used to get the thread id of the interrupt thread.
void getIntThreadId_ISR()
{
	intThreadId = std::this_thread::get_id();

//	int n = std::hash<std::thread::id>{}(std::this_thread::get_id());
//	printf("Int thread id:%d\n", n);
//
//	static char s[32];
//  memset(s, 0, 32);
//  sprintf(s, "getIntThreadId_ISR %d", 1);
//  debugPrint->writeLine(s, DebugPrint_defs::DigIn);
}
