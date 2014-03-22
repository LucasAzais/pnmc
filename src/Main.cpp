#include <cstring>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include "matrixModel/Matrix.h"
#include "forceModel/Variable.h"
#include "forceModel/HyperEdge.h"
#include "parsers/parse.hh"
#include "Population.h"

using namespace std;


int main(int argc, char** argv)
{

  Matrix matrix;

  using namespace pnmc::parsers;

  if(argc == 1)
  {
    matrix = Matrix(100,100);
  }
  else  //START PARSER
  {
  
    if (argc != 3)
    {
      std::cerr << "Usage: " << argv[0] << " bpn|prod|tina|xml file" << std::endl;
      return 1;
    }
    
    input_format format;
    if (strcmp(argv[1], "bpn") == 0)
    {
      format = input_format::bpn;
    }
    else if (strcmp(argv[1], "prod") == 0)
    {
      format = input_format::prod;
    }
    else if (strcmp(argv[1], "tina") == 0)
    {
      format = input_format::tina;
    }
    else if (strcmp(argv[1], "xml") == 0)
    {
      format = input_format::xml;
    }
    else
    {
      std::cerr << "Invalid format specification" << std::endl;
      return 2;
    }
    
    std::ifstream file(argv[2]);
    if (not file.is_open())
    {
      std::cerr << "Can't read file '" << argv[2] << "'" << std::endl;
      return 3;
    }
  
    const auto net_ptr = parse(format, file);

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
