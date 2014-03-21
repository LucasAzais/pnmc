#include "Variable.h"
#include "HyperEdge.h"
#include <algorithm> // for sort()
#include <iostream>
#include <fstream>
#include <time.h> // for seeding std::srand()
#include <set>

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

		for(std::vector<HyperEdge*>::iterator it2 = var.edges().begin(); it2 != var.edges().end(); ++it2)
		{
			HyperEdge& edge = **it2;
			var.position() += edge.centerOfGravity;
		}
		var.position() /= var.edges().size();
	}

	sortVariables(variables);

	refreshPositions(variables);

	return getSpan(edges);
};

double getSpan(const std::vector<HyperEdge*>& edges) {
	double span = 0;
	for(std::vector<HyperEdge*>::const_iterator cit = edges.begin(); cit != edges.end(); ++cit)
	{
		HyperEdge& e = **cit;
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

void sortVariables(std::vector<Variable*>& variables) {
	std::sort(variables.begin(),variables.end(),compare);	
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
	for(int i=0; i<100; i++) {
		previousSpan = currentSpan;
		//std::cout << currentSpan << std::endl;
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
		for(std::vector<Variable*>::iterator it2 = v.begin(); it2 != v.end(); ++it2)
		{
			Variable& var = **it2;
			out << var.position() << std::endl;
		}
	}	
};

void removeDuplicates(std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges) {
	for(std::vector<Variable*>::iterator it = variables.begin(); it != variables.end(); ++it)
	{
		std::vector<HyperEdge*>& es = (*it)->edges();
		std::set<HyperEdge*> set = std::set<HyperEdge*>(es.begin(),es.end());
		es = std::vector<HyperEdge*>(set.begin(),set.end());
	}
	for(std::vector<HyperEdge*>::iterator it = edges.begin(); it != edges.end(); ++it)
	{
		std::vector<Variable*>& vars = (*it)->variables;
		std::set<Variable*> set = std::set<Variable*>(vars.begin(),vars.end());
		vars = std::vector<Variable*>(set.begin(),set.end());
	}
};
