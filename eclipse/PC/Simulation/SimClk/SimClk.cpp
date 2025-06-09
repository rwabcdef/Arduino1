/*
 * SimClk.cpp
 *
 *  Created on: 25 Jun 2023
 *      Author: rw123
 */


#include <chrono>
#include "SimClk.hpp"


SimClk::SimClk(uint32_t end_uS, std::function<void(uint64_t)> tickCallBack, uint64_t value)
{
	this->end_uS = end_uS;
	this->tickCallBack = tickCallBack;
	this->value = value;
	this->current_uS.store(0);
	//this->current_uS = 0;
	this->done = false;
}
SimClk::~SimClk()
{
  delete this->thrd;
}
void SimClk::start()
{
	this->thrd = new std::thread(&SimClk::run, this);
}

// Blocks the calling thread until this thread has finnished.
void SimClk::join()
{
	this->thrd->join();
}

void SimClk::run()
{


	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	this->threadId = std::this_thread::get_id();
	int i, j;
	do
	{
		for(i=0;i<500;i++)
		{
			this->isPrime(this->value);
		}

		if(this->tickCallBack != nullptr)
		{
			this->tickCallBack(this->current_uS.load());
		}

		this->current_uS.store(this->current_uS.load() + 1);
				//this->current_uS++;

	//}while(this->current_uS.load() < this->end_uS)
	}while(this->current_uS < this->end_uS);

	this->done = true;
}

uint64_t SimClk::getCurrent()
{
	//std::lock_guard<std::mutex> lock(this->currentMutex);

	return this->current_uS.load();
	//return this->current_uS;
}

bool SimClk::getDone()
{
	return this->done;
}

std::thread::id SimClk::getThreadId()
{
	return this->threadId;
}

bool SimClk::isPrime(int num){
    bool flag=true;
    for(int i = 2; i <= num / 2; i++) {
       if(num % i == 0) {
          flag = false;
          break;
       }
    }
    return flag;
}

