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

  /*
  std::vector<unsigned int> norms = std::vector<unsigned int>(selectionSize, 0); 
  std::vector<unsigned int> bestNormsIndex = std::vector<unsigned int>(selectionSize, UINT_MAX);  

  for(unsigned int i = 0; i < currentSize; ++i)
  {
    norms[i] = population[i]->norm();
  }

  for(unsigned int j = 0; j < selectionSize; ++j)
  {
    for(unsigned int k = 0; k < currentSize; ++k)
    {
      if(norms[k]<norms[bestNormsIndex[j]])
      {
        //Put UINT_MAX back in, so that it doesn't pick the same norm again and again.
        bestNormsIndex[j] = k;
        norms[k] = UINT_MAX;
      }
    }
  }

  for(unsigned int i = 0; i < selectionSize; ++i)
  {
    population[bestNormsIndex[i]]->erase();
  }

  currentSize = selectionSize;
  */
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

