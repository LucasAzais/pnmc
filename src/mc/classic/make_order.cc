#include <algorithm>
#include <random>
#include <fstream>
#include <iostream>

#include <set>
#include <unordered_map>
#include <deque>
#include <sdd/order/order.hh>
#include <sdd/order/strategies/force.hh>

#include "mc/classic/dump.hh"
#include "mc/classic/make_order.hh"

#include "mc/classic/force/Link.h"
#include "mc/classic/force/Variable.h"
#include "mc/classic/force/HyperEdge.h"

namespace pnmc { namespace mc { namespace classic {

using sdd_conf = sdd::conf1;

/*------------------------------------------------------------------------------------------------*/

sdd::order<sdd_conf>
make_order(const conf::configuration& conf, statistics& stats, const pn::net& net)
{
  std::unordered_map<std::string, Variable> variables_holder;
  std::deque<HyperEdge> edges_holder;

  unsigned int pos = 0; 
  for (const auto& place : net.places())
  {
    if (place.connected())
    {
      variables_holder.emplace(place.id, Variable(place.id, pos++));
    }
  }
  
  for (const auto& transition : net.transitions())
  {
    edges_holder.emplace_back();
    HyperEdge& edge = edges_holder.back();
    edge.links.reserve(transition.post.size() + transition.pre.size());
    std::set<Variable*> pre_transitions, post_transitions;

    for (const auto& arc : transition.pre)
    {
      Variable& v = variables_holder.find(arc.first)->second;
      //edge.links.push_back(Link(&v,PRE));
      pre_transitions.insert(&v);
      v.edges().push_back(&edge);
    }

    for (const auto& arc : transition.post)
    {
      Variable& v = variables_holder.find(arc.first)->second;
      //edge.links.push_back(Link(&v,POST));
      post_transitions.insert(&v);
      v.edges().push_back(&edge);
    }

    for(Variable* v : pre_transitions) {
      if(post_transitions.find(v) == post_transitions.end()) {
        edge.links.push_back(Link(v,PRE));
      }
      else {
        edge.links.push_back(Link(v,PRE_POST));  
      }
    }
    for(Variable* v : post_transitions) {
      if(pre_transitions.find(v) == pre_transitions.end()) {
        edge.links.push_back(Link(v,POST));
      }
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

  if(conf.natural) {
    std::cout << "with natural order" << std::endl;
  }
  else if(conf.force) {
    std::cout << "with basic force" << std::endl;
    applyForce(variables,edges);
  }
  else if(conf.force2) {
    std::cout << "with order_edges inside force" << std::endl;
    applyForce2(variables,edges,true);
  }
  else if(conf.order_edges) {
    if(conf.order_pre_post) {
      std::cout << "with order_edges + order_pre_post" << std::endl;
      applyForce(variables,edges,true,false,true);
    }
    else {
      std::cout << "with order_edges" << std::endl;
      applyForce(variables,edges,true,false,false);
    }
  }
  else if(conf.order_edges_reversed) {
    if(conf.order_pre_post) {
      std::cout << "with order_edges_reversed + order_pre_post" << std::endl;
      applyForce(variables,edges,true,true,true);
    }
    else {
      std::cout << "with order_edges_reversed" << std::endl;
      applyForce(variables,edges,true,true,false);
    }
  }
  else if(conf.order_pre_post) {
    std::cout << "with order_pre_post" << std::endl;
    applyForce(variables,edges,false,false,true);
  }
  else if(conf.force3) {
    std::cout << "with order_edges_reversed inside force" << std::endl;
    applyForce2(variables,edges,false);
  }

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
