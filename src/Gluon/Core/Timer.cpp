#include <Gluon/Core/Timer.h>

#include <chrono>

using Clock = std::chrono::high_resolution_clock;

Timer::Timer() { Restart(); }

f64 Timer::Tick()
{
	auto t         = Clock::now();
	m_previousTick = m_lastTick;
	m_lastTick     = t.time_since_epoch().count();

	return GetDeltaTime();
}

void Timer::Restart()
{
	auto t      = Clock::now();
	m_startTime = t.time_since_epoch().count();
	m_lastTick = m_previousTick = m_startTime;
}

u64 Timer::GetDeltaTimeInNanoseconds() const
{
	u64 dt = m_lastTick - m_previousTick;
	return dt;
}

u64 Timer::GetDeltaTimeInMicroseconds() const
{
	return GetDeltaTimeInNanoseconds() / 1000;
}

u64 Timer::GetDeltaTimeInMilliseconds() const
{
	return GetDeltaTimeInNanoseconds() / 1000;
}

f64 Timer::GetDeltaTime() const
{
	return static_cast<f64>(GetDeltaTimeInNanoseconds()) * 1e-9;
}

f64 Timer::GetTimeSinceStarted() const
{
	u64 t  = Clock::now().time_since_epoch().count();
	u64 dt = t - m_startTime;
	return static_cast<f64>(dt) * 1e-9;
}
