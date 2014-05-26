#include <cstdlib>
#include <time.h>
#include <random>
#include <climits>
#include <algorithm>

#include "Population.h"

Population::Population()
: population(std::vector<Matrix*>()), currentSize(0)
{

};

void Population::generate(Matrix* initialMatrix, const unsigned int initialPopulationSize)
{
  srand(time(NULL));

  unsigned int row1;
  unsigned int row2;
  unsigned int i = 0;

  Matrix* newMatrix = initialMatrix->copy();

  while(i < initialPopulationSize)
  {

    row1 = rand() % initialMatrix->getRowNumber();
    row2 = rand() % initialMatrix->getRowNumber();

    if(row1!=row2)
    {
      ++i;
      newMatrix->swapRows(row1,row2);
      newMatrix->norm();
    }

    population.push_back(newMatrix);
    //newMatrix->clear();
    newMatrix = initialMatrix->copy();
    
  }
  
  currentSize = initialPopulationSize;
}

void Population::select(const unsigned int selectionSize)
{
  
  std::sort(population.begin(), population.end(), comp);

  population.erase( population.begin() + selectionSize, population.end());
  currentSize = selectionSize;

}

Matrix* Population::getFinalMatrix()
{
  select(1);
  if(currentSize == 1)
  {
    return population.at(0);
  }
  else
  {
    return NULL;
  }
}

void Population::mutate(const unsigned int newPopulationSize)
{
  if(newPopulationSize < currentSize)
  {
    std::cout << std::endl;
    std::cout << "###################" << std::endl;
    std::cout << "ISSUE MUTATION SIZE" << std::endl;
    std::cout << "###################" << std::endl;
    std::cout << std::endl;
    exit(1);
  }

  unsigned int nextSize = 0;

  for(unsigned int i = 0; i < newPopulationSize; ++i)
  {
    this->generate(this->population.at(i), newPopulationSize/(this->population.size()));
    nextSize += currentSize;
  }

  if(newPopulationSize!=nextSize)
  {
    this->generate(this->population.at(this->population.size()-1), newPopulationSize-nextSize);
  }

  currentSize = newPopulationSize;
}

void Population::displayPopulation()
{
  for(unsigned int i = 0; i < currentSize; ++i)
  {
    population.at(i)->displayMatrix();
    std::cout << population.at(i)->getNorm() << std::endl;
    std::cout << std::endl;
  }
}

bool comp(Matrix* m1, Matrix* m2)
{
  return m1->getNorm() < m2->getNorm();
}

