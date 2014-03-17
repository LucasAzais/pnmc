#pragma once
#include "Variable.h"
#include <vector>

class Variable;

class HyperEdge
{
public:
	double centerOfGravity;
	std::vector<Variable*> variables;
	HyperEdge();
	double getSpan(); // elle ne modifie pas HyperEdge, donc elle devrait être 'const'
};

