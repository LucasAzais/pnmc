#pragma once

#include "Variable.h"

enum Relation { UNDEF, PRE, PRE_POST, POST };

struct Link {
	
		Variable* variable;
		Relation relation;
		Link(Variable* var, Relation rel) { variable=var; relation=rel;};
};

inline
bool
	compare_positions(const Link& l1, const Link& l2)
{
	return l1.variable->position() < l2.variable->position();
};

inline
bool
	compare_relations(const Link& l1, const Link& l2)
{
	return l1.relation < l2.relation;
};
