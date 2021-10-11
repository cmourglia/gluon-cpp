#pragma once

#include <Gluon/Core/Defines.h>

class Timer
{
public:
	Timer();

	// Get the time since the last call to Tick() or since the start if called
	// for the first time
	f64 tick();

	// Set the start time to the current time
	void restart();

	// Get the time elapsed between the two last calls to Tick()
	f64 delta_time() const;
	u64 delta_time_ns() const;
	u64 delta_time_ms() const;
	u64 delta_time_us() const;

	// Get the time since the timer started (or since the last call to
	// Restart()). Who could have guessed ?
	f64 time_since_started() const;

private:
	u64 m_start_time    = 0;
	u64 m_last_tick     = 0;
	u64 m_previous_tick = 0;
};