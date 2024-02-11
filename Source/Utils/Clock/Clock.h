#ifndef CLOCK_H
#define CLOCK_H
#include <chrono>

class Clock
{
public:
	void Restart(void);
	float GetElapsedTime();
private:
	std::chrono::high_resolution_clock::time_point clock;
};

#endif // !CLOCK_H
