#pragma once
class FrameCounter
{
public:
	FrameCounter();
	float touch();
	LARGE_INTEGER getFreq() { return m_i64Freq; }
	LARGE_INTEGER getBegin() { return m_i64Begin; }
	void setBegin(LARGE_INTEGER i64Begin) { m_i64Begin = i64Begin; }
private:
	LARGE_INTEGER m_i64Freq;
	LARGE_INTEGER m_i64Begin;
	LARGE_INTEGER m_i64Last;
};

