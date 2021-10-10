#pragma once

#include <Gluon/Core/Defines.h>

namespace VM
{

struct Cell
{
	NONCOPYABLE(Cell);
	NONMOVEABLE(Cell);

	Cell()          = default;
	virtual ~Cell() = default;

	virtual const char* GetTypename() const = 0;

	using VisitorCallback = void (*)(Cell*);
	virtual void VisitGraph(VisitorCallback callback) { callback(this); }

	bool marked = false;
	bool used   = true;
};
}