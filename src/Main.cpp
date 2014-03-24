#include <cstring>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include "matrixModel/Matrix.h"
#include "parsers/parse.hh"
#include "Population.h"

using namespace std;


int main(int argc, char** argv)
{

  Matrix matrix;

  using namespace pnmc;

  if(argc == 1)
  {
    matrix = Matrix(100,100);
  }
  else
  {
    try
    {
      boost::optional<conf::configuration> conf_opt;
      try
      {
        conf_opt = conf::fill_configuration(argc, argv);
      }
      catch (const boost::program_options::error& e)
      {
        std::cerr << e.what() << std::endl;
        return 1;
      }
  
      if (not conf_opt) // --help or --version
      {
        return 0;
      }
  
      const auto& conf = *conf_opt;
      try
      {
        auto in = file_or_cin(conf);
        const auto net_ptr = parsers::parse(conf, *in);
        if (conf.delete_file and not conf.read_stdin)
        {
          ::remove(conf.file_name.c_str());
        }
        mc::mc worker(conf);
        worker(*net_ptr);
      }
      catch (const unreadable_file&)
      {
        std::cerr << "Can't open '" << conf.file_name << "'." << std::endl;
        return 2;
      }
      catch (const parsers::parse_error& p)
      {
        std::cerr << "Error when parsing input." << std::endl;
        std::cerr << p.what() << std::endl;
        return 3;
      }
    }
    catch (std::exception& e)
    {
      std::cerr << "Fatal error. Please report the following to a.hamez@isae.fr." << std::endl;
      std::cerr << e.what() << std::endl;
      return -1;
    }
  
    matrix = Matrix(*net_ptr);
  } //END PARSER

  //matrix.displayMatrix();

  std::cout << matrix.getNorm() << std::endl;
  std::cout << std::endl;
  
  Population pop;
  pop.generate(&matrix, 40);
  
  std::cout << "GLaDOS is working" << std::endl;
  std::cout << std::endl;

  for(unsigned int i = 0; i < 100; ++i)
  {
  
    pop.mutate(pop.getSize()*200);
    pop.select(10);

  }

  pop.select(1);
  
  //pop.displayPopulation();
  pop.displayFirstNorm();
/*
  //pop.displayPopulation();

  std::cout << "END FIRST GENERATION" << std::endl;
  std::cout << "START MUTATION" << std::endl;

  pop.mutate(40);

  //pop.displayPopulation();

  std::cout << "END MUTATION" << std::endl;
  std::cout << "START SELECTION" << std::endl;
  
  pop.select(1);

  pop.displayPopulation();

  std::cout << "END SELECTION" << std::endl;
*/

  //matrix.order();
  
  //matrix.displayMatrix();

  //std::cout << matrix.norm() << std::endl;
  //std::cout << std::endl;


  //system("pause");
  return 0;
}
