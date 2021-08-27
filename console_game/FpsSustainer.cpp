#include "FpsSustainer.h"

#include <cstdint>
#include <windows.h>

FpsSustainer::FpsSustainer()
    : FRAME_TIME(1.0 / FPS)
{
	QueryPerformanceFrequency(&time_freq);
	QueryPerformanceCounter(&start_time);
	QueryPerformanceCounter(&end_time);
}

void FpsSustainer::init_start_time()
{
	QueryPerformanceCounter(&start_time);
}

void FpsSustainer::wait()
{
	QueryPerformanceCounter(&end_time);
	double cur_frame_time = static_cast<double>(end_time.QuadPart - start_time.QuadPart) / static_cast<double>(time_freq.QuadPart);
	if (cur_frame_time < FRAME_TIME)
	{
		DWORD sleep_time = static_cast<DWORD>((FRAME_TIME - cur_frame_time) * 1000);
		Sleep(sleep_time);
	}
}

uint16_t FpsSustainer::get_fps()
{
	return FPS;
}