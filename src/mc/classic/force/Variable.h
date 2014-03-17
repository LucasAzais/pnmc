#pragma once
#include <string>
#include <vector>
#include "HyperEdge.h"

class HyperEdge;

class Variable
{
private:
  const std::string& id_;
	double position_;
	std::vector<HyperEdge*> connected_edges_;

public:
	Variable(const std::string& id, double position);

  static double iterate(std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges);
  static double getSpan(const std::vector<HyperEdge*>& edges);
  static  bool compare(Variable* v1, Variable* v2)
  { return v1->position_ < v2->position_; }

  // Pour la culture
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



