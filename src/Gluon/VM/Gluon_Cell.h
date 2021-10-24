#pragma once

#include <Beard/Macros.h>

struct ZCell
{
	NONCOPYABLE(ZCell);
	NONMOVEABLE(ZCell);

	ZCell()          = default;
	virtual ~ZCell() = default;

	virtual const char* ToString() const = 0;

	using FVisitorCallback = void (*)(ZCell*);
	virtual void VisitGraph(FVisitorCallback callback)
	{
		callback(this);
	}

	bool bMarked = false;
	bool bUsed   = true;
};
