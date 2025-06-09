/*
 * SimClk.hpp
 *
 *  Created on: 25 Jun 2023
 *      Author: rw123
 */

#ifndef SIMCLK_HPP_
#define SIMCLK_HPP_


#include<stdint.h>
#include <thread>
#include <atomic>
#include <functional>
#include <cstddef>


class SimClk
{
public:
	SimClk(uint32_t end_uS, std::function<void(uint64_t)> tickCallBack = nullptr, uint64_t value = 4589641);
	~SimClk();
	void start();
	void join();
	uint64_t getCurrent();
	bool getDone();
	std::thread::id getThreadId();

private:
	//std::mutex currentMutex;
	uint64_t end_uS;
	std::atomic<uint64_t> current_uS;
	//uint32_t current_uS;
	std::thread *thrd;
	std::thread::id threadId;
	uint64_t value;
	std::function<void(uint64_t)> tickCallBack;
	bool done;

	void run();
	bool isPrime(int num);
};


#endif /* SIMCLK_HPP_ */
