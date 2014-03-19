#include <algorithm>
#include <random>
#include <fstream>
#include <iostream>

#include <unordered_map>
#include <sdd/order/order.hh>
#include <sdd/order/strategies/force.hh>

#include "mc/classic/dump.hh"
#include "mc/classic/make_order.hh"

#include "mc/classic/force/Variable.h"
#include "mc/classic/force/HyperEdge.h"

namespace pnmc { namespace mc { namespace classic {

using sdd_conf = sdd::conf1;

/*------------------------------------------------------------------------------------------------*/

sdd::order<sdd_conf>
make_order(const conf::configuration& conf, statistics& stats, const pn::net& net)
{
  std::unordered_map<std::string, Variable> variables_holder;
  std::vector<HyperEdge> edges_holder;

  unsigned int pos = 0; 
  for (const auto& place : net.places())
  {
    variables_holder.emplace(place.id, Variable(place.id, pos++));
  }
  
  for (const auto& transition : net.transitions())
  {
    HyperEdge& edge = *edges_holder.insert(edges_holder.end(), HyperEdge());
    edge.variables.reserve(transition.post.size() + transition.pre.size());

    for (const auto& arc : transition.pre)
    {
      Variable& v = variables_holder.find(arc.first)->second;
      edge.variables.push_back(&v);
      v.edges().push_back(&edge);
    }

    for (const auto& arc : transition.post)
    {
      Variable& v = variables_holder.find(arc.first)->second;
      edge.variables.push_back(&v);
      v.edges().push_back(&edge);
    }
  }
  
  std::vector<Variable*> variables;
  variables.reserve(variables_holder.size());
  for (auto& v : variables_holder)
  {
    variables.push_back(&v.second);
  }

  std::vector<HyperEdge*> edges;
  edges.reserve(edges_holder.size());
  for (auto& e : edges_holder)
  {
    edges.push_back(&e);
  }

  exportToRender("withoutForce.txt",variables,edges);
  applyForce(variables,edges);
  exportToRender("withForce.txt",variables,edges);

  // Build the order here. Let's do a sort in the meantime.
  std::sort( variables.begin(), variables.end()
           , [](Variable* lhs, Variable* rhs)
               {return lhs->position() < rhs->position();});

  // Return the order
  sdd::order_builder<sdd_conf> ob;
  // We push on top of the order so we need to iterate in the reverse order
  for (auto rcit = variables.rbegin(); rcit != variables.rend(); ++rcit)
  {
    ob.push((*rcit)->id());
  }
  return ob;
}

/*------------------------------------------------------------------------------------------------*/

}}} // namespace pnmc::mc::classic
