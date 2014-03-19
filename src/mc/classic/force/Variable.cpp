#include "Variable.h"
#include "HyperEdge.h"
#include <algorithm> // for sort()
#include <iostream>
#include <fstream>
#include <time.h> // for seeding std::srand()

Variable::Variable(const std::string& id, double position)
  : id_(id), position_(position)
{}

double iterate(std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges){

	for (std::vector<HyperEdge*>::iterator it = edges.begin(); it != edges.end(); ++it)
	{
		HyperEdge& edge = **it;

		edge.centerOfGravity = 0;
		for (Variable* v : (*it)->variables)
		{
			edge.centerOfGravity += v->position();
		}
		edge.centerOfGravity /= edge.variables.size();
	}

	for(std::vector<Variable*>::iterator it = variables.begin(); it != variables.end(); ++it)
	{
		Variable& var = **it;
		var.position()=0;

		for(std::vector<HyperEdge*>::iterator it = var.edges().begin(); it != var.edges().end(); ++it)
		{
			HyperEdge& edge = **it;
			var.position() += edge.centerOfGravity;
		}
		var.position() /= var.edges().size();
	}

	std::sort(variables.begin(),variables.end(),compare);

	refreshPositions(variables);

	return getSpan(edges);
};

double getSpan(const std::vector<HyperEdge*>& edges) {
	double span = 0;
	for(unsigned int i=0; i<edges.size(); i++)
	{
		HyperEdge& e = *(edges[i]);
		span += e.getSpan();
	}
	return span;
};

bool
	compare(const Variable* v1, const Variable* v2)
{
	return (*v1).position() < (*v2).position();
};

void refreshPositions(std::vector<Variable*>& variables){
		for(unsigned int i=0; i<variables.size(); i++) // set the variables' positions according to their order
	{
		(*variables[i]).position() = i;
	}
};

void shuffle(std::vector<Variable*>& variables) {
	std::srand (time(NULL));
	for(std::vector<Variable*>::iterator it = variables.begin(); it != variables.end(); ++it)
	{
		Variable& var = **it;
		var.position() = std::rand();
	}
	std::sort(variables.begin(),variables.end(),compare);
};

void applyForce(std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges) {
	double previousSpan = std::numeric_limits<double>::max();
	double currentSpan = getSpan(edges);
	while(currentSpan<previousSpan) {
		previousSpan = currentSpan;
		currentSpan = iterate(variables,edges);
	}
};

void exportToRender(std::string filename, std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges) {
		
	std::ofstream out;
	out.open (filename);
	out << variables.size() << std::endl;
	for(std::vector<Variable*>::iterator it = variables.begin(); it != variables.end(); ++it)
	{
		Variable& var = **it;
		out << var.id() << std::endl;
	}
	for(std::vector<HyperEdge*>::iterator it = edges.begin(); it != edges.end(); ++it)
	{
		HyperEdge& edge = **it;
		std::vector<Variable*>& v = edge.variables;
		out << v.size() << std::endl;
		for(std::vector<Variable*>::iterator it = v.begin(); it != v.end(); ++it)
		{
			Variable& var = **it;
			out << var.position() << std::endl;
		}
	}	
};
