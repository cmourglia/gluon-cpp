#pragma once

#include <Beard/Macros.h>

struct Cell
{
	NONCOPYABLE(Cell);
	NONMOVEABLE(Cell);

	Cell()          = default;
	virtual ~Cell() = default;

	virtual const char* to_string() const = 0;

	using VisitorCallback = void (*)(Cell*);
	virtual void visit_graph(VisitorCallback callback) { callback(this); }

	bool marked = false;
	bool used   = true;
};
