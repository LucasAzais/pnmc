#include <algorithm>
#include <random>
#include <fstream>
#include <iostream>
#include <string>

#include <unordered_map>
#include <deque>
#include <sdd/order/order.hh>
#include <sdd/order/strategies/force.hh>

#include "Population.h"
#include "matrixModel/Matrix.h"
#include "mc/classic/dump.hh"
#include "mc/classic/make_order.hh"

namespace pnmc { namespace mc { namespace classic {

using sdd_conf = sdd::conf1;
namespace chrono = std::chrono;

/*------------------------------------------------------------------------------------------------*/

sdd::order<sdd_conf>
make_order(const conf::configuration& conf, statistics& stats, const pn::net& net)
{

  chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();

  Matrix matrix = Matrix(net);

  std::ifstream file;
  if(conf.matrix_conf_file_path){

    file.open(*conf.matrix_conf_file_path);

    if(file.good())
    {

      int buffer_size = 256;
      char init_pop[buffer_size];
      char it_num[buffer_size];
      char mut_size[buffer_size];
      char select_size[buffer_size];

      file.getline(init_pop, buffer_size);
      file.getline(it_num, buffer_size);
      file.getline(mut_size, buffer_size);
      file.getline(select_size, buffer_size);

      std::string init_pop_str (init_pop);
      std::string it_num_str (it_num);
      std::string mut_size_str (mut_size);
      std::string select_size_str (select_size);

      init_pop_str.erase(0, 21);
      it_num_str.erase(0, 19);
      mut_size_str.erase(0, 16);
      select_size_str.erase(0, 17);;

      const unsigned int INITIAL_POPULATION = std::stoi( init_pop_str );
      const unsigned int ITERATION_NUMBER = std::stoi( it_num_str );
      const unsigned int MUTATION_SIZE = std::stoi( mut_size_str );
      const unsigned int SELECTION_SIZE = std::stoi( select_size_str );
    
      Population pop;

      if(conf.order_ordering_matrix) {

        pop.generate(&matrix, INITIAL_POPULATION);

        for(unsigned int i = 0; i < ITERATION_NUMBER; ++i)
        {
      
        pop.mutate(pop.getSize()*MUTATION_SIZE);
        pop.select(SELECTION_SIZE);

        }
        
        matrix = *(pop.getFinalMatrix());

      }
    }

    stats.matrix_order_duration = chrono::system_clock::now() - start;

    file.close();
}

  /*// Build the order here. Let's do a sort in the meantime.
  std::sort( variables.begin(), variables.end()
           , [](Variable* lhs, Variable* rhs)
               {return lhs->position() < rhs->position();});*/

  // Return the order
  sdd::order_builder<sdd_conf> ob;
  std::vector<std::string> idList = matrix.getStringIdList();

  // We push on top of the order so we need to iterate in the reverse order
  for (auto rcit = idList.rbegin(); rcit != idList.rend(); ++rcit)
  {
    ob.push(*rcit);
  }
  return ob;
}

/*------------------------------------------------------------------------------------------------*/

}}} // namespace pnmc::mc::classic
