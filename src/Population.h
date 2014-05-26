#ifndef POPULATION_H_INCLUDED
#define POPULATION_H_INCLUDED

#include <vector>

#include "matrixModel/Matrix.h"

using population_type = std::vector<Matrix*>;

bool comp(Matrix* m1, Matrix* m2);

class Population
{
  private:
    population_type population;
    unsigned int currentSize;

  public:
    Population();
    unsigned int getSize() { return currentSize; }
    void generate(Matrix* initialMatrix, const unsigned int initialPopulationSize);
    void select(const unsigned int selectionSize);
    Matrix* getFinalMatrix();
    void mutate(const unsigned int newPopulationSize);
    void displayPopulation();
    void displayFirstNorm(){ std::cout << population.at(0)->getNorm() << std::endl; }

};


#endif // POPULATION_H_INCLUDED
