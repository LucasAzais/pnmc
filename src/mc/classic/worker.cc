#include <cassert>
#include <chrono>
#include <iostream>
#include <set>
#include <thread>

#include <boost/dynamic_bitset.hpp>

#include <sdd/sdd.hh>
#include <sdd/tools/size.hh>

#include "mc/classic/bound_error.hh"
#include "mc/classic/bounded_post.hh"
#include "mc/classic/dead.hh"
#include "mc/classic/dump.hh"
#include "mc/classic/live.hh"
#include "mc/classic/make_order.hh"
#include "mc/classic/post.hh"
#include "mc/classic/pre.hh"
#include "mc/classic/statistics.hh"
#include "mc/classic/timed.hh"
#include "mc/classic/worker.hh"

namespace pnmc { namespace mc { namespace classic {

namespace chrono = std::chrono;

using sdd_conf = sdd::conf1 ;
using SDD = sdd::SDD<sdd_conf>;
using homomorphism = sdd::homomorphism<sdd_conf>;

using sdd::composition;
using sdd::fixpoint;
using sdd::intersection;
using sdd::sum;
using sdd::function;

/*------------------------------------------------------------------------------------------------*/

SDD
initial_state(const sdd::order<sdd_conf>& order, const pn::net& net)
{
  return SDD(order, [&](const std::string& id)
                        -> sdd::values::flat_set<unsigned int>
                       {
                         return {net.places_by_id().find(id)->marking};
                       });
}

/*------------------------------------------------------------------------------------------------*/

/// @brief Create a timed function if required by the configuration, a normal function otherwise.
template <typename Fun, typename... Args>
homomorphism
mk_fun( const conf::configuration& conf, const bool& stop, const sdd::order<sdd_conf>& o
      , const sdd_conf::Identifier& id, Args&&... args)
{
  if (conf.max_time > chrono::duration<double>(0))
  {
    return function(o, id, timed<sdd_conf, Fun>(stop, std::forward<Args>(args)...));
  }
  else
  {
    return function(o, id, Fun(std::forward<Args>(args)...));;
  }
}

/*------------------------------------------------------------------------------------------------*/

/// @brief Compute the transition relation corresponding to a petri net.
homomorphism
transition_relation( const conf::configuration& conf, const sdd::order<sdd_conf>& o
                   , const pn::net& net, boost::dynamic_bitset<>& transitions_bitset
                   , statistics& stats, const bool& stop)
{
  chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();

  std::set<homomorphism> operands;
  operands.insert(sdd::id<sdd_conf>());

  for (const auto& transition : net.transitions())
  {
    homomorphism h_t = sdd::id<sdd_conf>();

    // Add a "canary" to detect live transitions.
    if (conf.compute_dead_transitions)
    {
      if (not transition.post.empty())
      {
        const auto f = mk_fun<live>( conf, stop, o, transition.post.begin()->first, transition.index
                                   , transitions_bitset);
        h_t = sdd::carrier(o, transition.post.begin()->first, f);
      }
    }

    // Post actions.
    for (const auto& arc : transition.post)
    {
      // Is the maximal marking limited?
      homomorphism f = conf.marking_bound == 0
                     ? mk_fun<post>(conf, stop, o, arc.first, arc.second)
                     : mk_fun<bounded_post<sdd_conf>>( conf, stop, o, arc.first, arc.second
                                                     , conf.marking_bound, arc.first);
      h_t = composition(h_t, sdd::carrier(o, arc.first, f));
    }

    // Pre actions.
    for (const auto& arc : transition.pre)
    {
      homomorphism f = mk_fun<pre>(conf, stop, o, arc.first, arc.second);
      h_t = composition(h_t, sdd::carrier(o, arc.first, f));
    }

    operands.insert(h_t);
  }
  stats.relation_duration = chrono::system_clock::now() - start;
  return fixpoint(sum(o, operands.cbegin(), operands.cend()));
}

/*------------------------------------------------------------------------------------------------*/

homomorphism
rewrite( const conf::configuration& conf, const sdd::order<sdd_conf>& o
       , const homomorphism& h, statistics& stats)
{
  chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();
  const auto res = sdd::rewrite(o, h);
  stats.rewrite_duration = chrono::system_clock::now() - start;
  return res;
}

/*------------------------------------------------------------------------------------------------*/

SDD
state_space( const conf::configuration& conf, const sdd::order<sdd_conf>& o, SDD m
           , homomorphism h, statistics& stats, bool& stop, const sdd::manager<sdd_conf>& manager)
{
  SDD res;

  // To stop the clock thread.
  bool finished = false;

  // The reference time;
  const std::chrono::time_point<std::chrono::system_clock>
    beginning(std::chrono::system_clock::now());

  // Limited time mode?
  std::thread clock;
  if (conf.max_time > chrono::duration<double>(0))
  {
    clock = std::thread([&]
                        {
                          while (not finished)
                          {
                            std::this_thread::sleep_for(std::chrono::milliseconds(100));
                            if (std::chrono::system_clock::now() - beginning >= conf.max_time)
                            {
                              stop = true;
                              std::cout << "Time limit exceeded,";
                              std::cout << " it may take a while to completely stop." << std::endl;
                              break;
                            }
                          }
                        });
  }

  auto sdd_ut_thread_size
    = std::thread([&]
                  {
                    const auto sample_time = std::chrono::milliseconds(500);
                    auto last = std::chrono::system_clock::now();
                    while (not finished)
                    {
                      std::this_thread::sleep_for(std::chrono::milliseconds(100));
                      auto now = std::chrono::system_clock::now();
                      if ((now - last) >= sample_time)
                      {
                        stats.sdd_ut_size.emplace_back(manager.sdd_stats().size);
                        last = now;
                      }
                    }
                  });

  chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();
  try
  {
    res = h(o, m);
  }
  catch (const bound_error<sdd_conf>& b)
  {
    std::cout << "Marking limit (" << conf.marking_bound << ") reached for place " << b.place << "."
              << std::endl;
    stats.interrupted = true;
    res = b.result();
  }
  catch (const sdd::interrupt<sdd_conf>& i)
  {
    stats.interrupted = true;
    res = i.result();
  }
  // Tell the clock thread to stop on next wakeup.
  finished = true;
  stats.state_space_duration = chrono::system_clock::now() - start;

  if (stats.interrupted)
  {
    std::cout << "State space computation interrupted after "
              << std::chrono::duration<double>(std::chrono::system_clock::now() - beginning).count()
              << "s." << std::endl;
  }

  if (conf.max_time > chrono::duration<double>(0))
  {
    clock.join();
  }
  sdd_ut_thread_size.join();

  return res;
}

/*------------------------------------------------------------------------------------------------*/

SDD
dead_states( const conf::configuration& conf, const sdd::order<sdd_conf>& o, const pn::net& net
           , const SDD& state_space, statistics& stats)
{
  std::set<homomorphism> and_operands;
  std::set<homomorphism> or_operands;

  chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();

  // Get relation
  for (const auto& transition : net.transitions())
  {
    // We are only interested in pre actions.
    for (const auto& arc : transition.pre)
    {
      const auto h = function(o, arc.first, dead(arc.second));
      or_operands.insert(sdd::carrier(o, arc.first, h));
    }

    and_operands.insert(sum(o, or_operands.cbegin(), or_operands.cend()));
    or_operands.clear();
  }
  const auto tmp = intersection(o, and_operands.cbegin(), and_operands.cend());
  stats.dead_states_relation_duration = chrono::system_clock::now() - start;

  // Rewrite the relation
  start = chrono::system_clock::now();
  const auto h = sdd::rewrite(o, tmp);
  stats.dead_states_rewrite_duration = chrono::system_clock::now() - start;

  // Compute the dead states
  start = chrono::system_clock::now();
  const auto res = h(o, state_space);
  stats.dead_states_duration = chrono::system_clock::now() - start;

  return res;
}

/*------------------------------------------------------------------------------------------------*/

worker::worker(const conf::configuration& c)
  : conf(c)
{}

/*------------------------------------------------------------------------------------------------*/

void
worker::operator()(const pn::net& net)
const
{
  // Initialize the libsdd.
  auto manager = sdd::manager<sdd_conf>::init();

  statistics stats(conf);

  // Used in limited time mode.
  bool stop = false;

  // Build the order.
  sdd::order<sdd_conf> o = make_order(conf, stats, net);
  assert(not o.empty() && "Empty order");
  if (conf.order_show)
  {
    std::cout << o << std::endl;
  }

  // Get the initial state.
  const SDD m0 = initial_state(o, net);

  // Map of live transitions.
  boost::dynamic_bitset<> transitions_bitset(net.transitions().size());

  // Compute the transition relation.
  const auto h_classic = transition_relation(conf, o, net, transitions_bitset, stats, stop);
  if (conf.show_relation)
  {
    std::cout << h_classic << std::endl;
  }

  // Rewrite the transition relation.
  const auto h = rewrite(conf, o, h_classic, stats);
  if (conf.show_relation)
  {
    std::cout << h << std::endl;
  }

  // Compute the state space.
  const auto m = state_space(conf, o, m0, h, stats, stop, manager);
  stats.nb_states = m.size().template convert_to<long double>();
  std::cout << stats.nb_states << " states" << std::endl;

  if (conf.compute_dead_transitions)
  {
    std::deque<std::string> dead_transitions;
    for (std::size_t i = 0; i < net.transitions().size(); ++i)
    {
      if (not transitions_bitset[i])
      {
        dead_transitions.push_back(net.get_transition_by_index(i).id);
      }
    }

    if (not dead_transitions.empty())
    {
      std::cout << dead_transitions.size() << " dead transition(s): ";
      std::copy( dead_transitions.cbegin(), std::prev(dead_transitions.cend())
               , std::ostream_iterator<std::string>(std::cout, ","));
      std::cout << *std::prev(dead_transitions.cend()) << std::endl;
    }
    else
    {
      std::cout << "No dead transitions" << std::endl;
    }
  }

  if (conf.compute_dead_states)
  {
    const auto dead = dead_states(conf, o, net, m, stats);
    if (dead.empty())
    {
      std::cout << "No dead states" << std::endl;
    }
    else
    {
      std::cout << dead.size().template convert_to<long double>() << " dead state(s):"
                << std::endl;

      // Get the identifier of each level (SDD::paths() doesn't give this information).
      std::deque<std::reference_wrapper<const std::string>> identifiers;
      o.flat(std::back_inserter(identifiers));

      /*for (const auto& path : dead.paths())
      {
        auto id_cit = identifiers.cbegin();
        auto path_cit = path.cbegin();
        for (; path_cit != std::prev(path.cend()); ++path_cit, ++id_cit)
        {
          std::cout << id_cit->get() << " : " << *path_cit << ", ";
        }
        std::cout << id_cit->get() << " : " << *path_cit << std::endl;
      }*/
    }
  }

  const auto total = stats.relation_duration + stats.rewrite_duration
                   + stats.state_space_duration + stats.dead_states_duration;
  std::cout << total.count() << "s" << std::endl;

  if (conf.show_time)
  {
    std::cout << "Relation             : " << stats.relation_duration.count() << "s"
              << std::endl
              << "Rewrite              : " << stats.rewrite_duration.count() << "s"
              << std::endl
              << "State space          : " << stats.state_space_duration.count() << "s"
              << std::endl;
    if (conf.compute_dead_states)
    {
      std::cout << "Dead states relation : " << stats.dead_states_relation_duration.count()
                << "s" << std::endl
                << "Dead states rewrite  : " << stats.dead_states_rewrite_duration.count()
                << "s" << std::endl
                << "Dead states          : " << stats.dead_states_duration.count()
                << "s" << std::endl;
    }
    if (conf.order_ordering_force)
    {
      std::cout << "FORCE                : " << stats.force_duration.count() << "s" << std::endl;
    }
  }

  if (conf.show_final_sdd_bytes)
  {
    std::cout << "Final SDD size: " << sdd::tools::size(m) << " bytes" << std::endl;
  }

  dump_sdd_dot(conf, m);
  dump_lua(conf, m);
  dump_json(conf, stats, manager, m);
}

/*------------------------------------------------------------------------------------------------*/

}}} // namespace pnmc::mc::classic
