#pragma once

#include <cstdint>
#include <windows.h>

class FpsSustainer final
{
public:
	FpsSustainer();
	void init_start_time();
	void wait();

private:
	static const uint16_t FPS = 30;
	const double FRAME_TIME;
	LARGE_INTEGER time_freq;
	LARGE_INTEGER start_time;
	LARGE_INTEGER end_time;
};