#include "Variable.h"
#include <algorithm> // for sort()
#include <iostream>

Variable::Variable(const std::string& id, double position)
  : id_(id), position_(position)
{}

double Variable::iterate(std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges)
{
  // Voir avec les itérateurs ci-dessous
  // for(int i=0; i<edges.size(); i++) // computes the HyperEdges' centers of gravity
  // {
  //   (*(edges[i])).centerOfGravity = 0;
  //   for(Variable* v : (*(edges[i])).variables) 
  //   {
  //     (*(edges[i])).centerOfGravity += (*v).position;
  //   }
  //   (*(edges[i])).centerOfGravity /= (double) (*(edges[i])).variables.size();
  //   //std::cout << "COG = " << (*(edges[i])).centerOfGravity << std::endl;
  // }

  for (std::vector<HyperEdge*>::iterator it = edges.begin(); it != edges.end(); ++it)
  {
    // simplifie l'accès à l'HyperEdge
    // En effet, it est un pointeur vers un pointeur de HyperEdge
    HyperEdge& edge = **it;
  
    edge.centerOfGravity = 0;
    for (Variable* v : (*it)->variables)
    {
      edge.centerOfGravity += v->position();
    }
    edge.centerOfGravity /= edge.variables.size();
  }

  // C++11
  // for(auto& edge : edges) // attention! edge reste un HyperEdge*!
  // {
  //   edge->centerOfGravity = 0;
  //   for (Variable* v : edge->variables)
  //   {
  //     edge->centerOfGravity += v->position();
  //   }
  //   edge->centerOfGravity /= edge->variables.size();
  // }

  // À faire avec des itérateurs
	for(int i=0; i<variables.size(); i++) // computes the Variables' new positions
	{
		//std::cout << "previous pos = " << v.position << std::endl;
		(*(variables[i])).position() = 0;
		for(int j=0; j<(*(variables[i])).edges().size(); j++)
		{
			HyperEdge e = *((*(variables[i])).edges()[j]);
			(*(variables[i])).position() = (*(variables[i])).position() + e.centerOfGravity;
			//std::cout << "cog = " << e.centerOfGravity << std::endl;
		}
		(*(variables[i])).position() /= (double) (*(variables[i])).edges().size();
		//std::cout <<" ("<<i<<"=)" << (*(variables[i])).position;
	}
	
  std::sort(variables.begin(),variables.end(),Variable::compare); // sorts the variables according to their positions

  //  Avec un lambda
  // std::sort( variables.begin() ,variables.end() ,
  //           [](Variable* lhs, Variable* rhs){return lhs->position < rhs->position;});
  
	for(int i=0; i<variables.size(); i++) // set the variables' positions according to their order
	{
		(*variables[i]).position() = i;
	}
	return Variable::getSpan(edges);
}


double Variable::getSpan(const std::vector<HyperEdge*>& edges)
{
	double span = 0;
	for(HyperEdge* e0 : edges)
	{
		HyperEdge e = *e0;
		span += e.getSpan();
	}
	return span;
}

