#include "Clock.h"
#include <algorithm>


void Clock::Restart(void)
{
	auto end = std::chrono::high_resolution_clock::now();
	this->clock = end;
}

float Clock::GetElapsedTime()
{
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - this->clock);
	return duration.count();
}
