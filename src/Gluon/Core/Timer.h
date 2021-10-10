#pragma once

#include <Gluon/Core/Defines.h>

class Timer
{
public:
	Timer();

	// Get the time since the last call to Tick() or since the start if called
	// for the first time
	f64 Tick();

	// Set the start time to the current time
	void Restart();

	// Get the time elapsed between the two last calls to Tick()
	f64 GetDeltaTime() const;
	u64 GetDeltaTimeInNanoseconds() const;
	u64 GetDeltaTimeInMicroseconds() const;
	u64 GetDeltaTimeInMilliseconds() const;

	// Get the time since the timer started (or since the last call to
	// Restart()). Who could have guessed ?
	f64 GetTimeSinceStarted() const;

private:
	u64 m_startTime    = 0;
	u64 m_lastTick     = 0;
	u64 m_previousTick = 0;
};