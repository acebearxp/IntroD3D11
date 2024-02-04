#include "pch.h"
#include "FrameCounter.h"

FrameCounter::FrameCounter()
{
	QueryPerformanceFrequency(&m_i64Freq);
	QueryPerformanceCounter(&m_i64Begin);
	m_i64Last = m_i64Begin;
}

float FrameCounter::touch()
{
	float fps = 0.0f;
	LARGE_INTEGER i64Now;
	QueryPerformanceCounter(&i64Now);
	if (i64Now.QuadPart != m_i64Last.QuadPart) {
		fps = 1.0f * m_i64Freq.QuadPart / (i64Now.QuadPart - m_i64Last.QuadPart);
	}
	m_i64Last = i64Now;

	return fps;
}
