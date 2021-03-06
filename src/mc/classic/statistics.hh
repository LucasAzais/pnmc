#ifndef _PNMC_MC_STATISTICS_HH_
#define _PNMC_MC_STATISTICS_HH_

#include <chrono>
#include <deque>

#include "conf/configuration.hh"

namespace pnmc { namespace mc { namespace classic {

/*------------------------------------------------------------------------------------------------*/

struct statistics
{
  const conf::configuration& conf;

  std::chrono::duration<double> relation_duration;
  std::chrono::duration<double> rewrite_duration;
  std::chrono::duration<double> state_space_duration;
  std::chrono::duration<double> force_duration;
  std::chrono::duration<double> dead_states_relation_duration;
  std::chrono::duration<double> dead_states_rewrite_duration;
  std::chrono::duration<double> dead_states_duration;

  long double nb_states;

  bool interrupted;

  std::deque<unsigned int> sdd_ut_size;

  statistics(const conf::configuration& c)
    : conf(c), relation_duration(), rewrite_duration(), state_space_duration(), force_duration()
    , dead_states_relation_duration(), dead_states_rewrite_duration()
    , dead_states_duration(), nb_states(0), interrupted(false), sdd_ut_size()
  {}
};

/*------------------------------------------------------------------------------------------------*/

}}} // namespace pnmc::mc::classic

#endif // _PNMC_MC_STATISTICS_HH_
