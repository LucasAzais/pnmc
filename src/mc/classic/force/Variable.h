#pragma once
#include <vector>
#include <algorithm> // for sort()

class HyperEdge;
double getSpan(const std::vector<HyperEdge*>& edges);

class Variable
{
private:
  const std::string& id_;
	double position_;
	std::vector<HyperEdge*> connected_edges_;

public:
	Variable(const std::string& id, double position);

	bool
		operator<(const Variable& other)
		const
	{
		return position_ < other.position_;
	}

	const std::string&
 	 id()
  	const
  	{
 	   return id_;
  	}

	double
		position()
		const
	{
		return position_;
	}

	double&
		position()
	{
		return position_;
	}

	const std::vector<HyperEdge*>&
		edges()
		const
	{
		return connected_edges_;
	}

	std::vector<HyperEdge*>&
		edges()
	{
		return connected_edges_;
	}

};

double iterate(std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges);

void computeCOG(std::vector<HyperEdge*>& edges);

void computeCOG(std::vector<Variable*>& variables);

bool compare(const Variable* v1, const Variable* v2);

void shuffle(std::vector<Variable*>& variables);

void refreshPositions(std::vector<Variable*>& variables);

void sortVariables(std::vector<Variable*>& variables);

void applyForce(std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges);
void applyForce2(std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges);
void applyForce3(std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges);
void applyForce4(std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges);

void plotForce(std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges);

void exportToRender(std::string filename, std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges);

//void removeDuplicates(std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges);
