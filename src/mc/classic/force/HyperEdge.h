#pragma once
#include "Link.h"
#include <vector>

class Variable;

class HyperEdge
{
public:
	double centerOfGravity;
	std::vector<Link> links;
	HyperEdge();
	double getSpan() const;
};

void order_pre_post( std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges );

bool compareSpan(const HyperEdge* e1, const HyperEdge* e2);

bool compare_edges(const HyperEdge* e1, const HyperEdge* e2);
bool compare_edges_reversed(const HyperEdge* e1, const HyperEdge* e2);

void order_edges( std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges, bool reversed);
