#include <vector>
#include <random>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <climits>
#include <unordered_set>
#include <unordered_map>
#include <boost/lexical_cast.hpp>

#include "Matrix.h"

Matrix::Matrix()
{
  columnNumber = 0;
  rowNumber = 0;
  currentNorm = 0;
}

Matrix::Matrix(const unsigned int height, const unsigned int width)
{

  columnNumber = width;
  rowNumber = height;
  
  bool randomBool = false;
  //std::default_random_engine generator;
  //std::uniform_int_distribution<int> distribution(0, 1);
 
  srand(time(NULL)); //Other random generator
  
  //matrix = matrix_type(); //.reserve(rowNumber);
  row_type currentRow;
  //currentRow = row_type(); //->reserve(columnNumber);

  for (unsigned int i = 0; i<rowNumber; ++i)
  {
    idList.push_back( boost::lexical_cast<std::string>( i ) );

    for (unsigned int j = 0; j<columnNumber; ++j)
    {
//      randomBool = (rand() % 2 == 0 ? false : true ); //P(1) = P(0)
      randomBool = ( ( (rand() % 2)*(rand() % 2) ) == 0 ? false : true ); //P(0) = 3/4 et P(1) = 1/4
      //(distribution(generator) == 0 ? false : true);
      currentRow.push_back( randomBool );

    }
    auto insertion = setMatrix.insert( currentRow );
    matrix.push_back(&(*insertion.first));
    currentRow.clear();
  }

  norm();
}

Matrix::Matrix(const pnmc::pn::net& net)
{
  std::unordered_map<std::string, unsigned int> id_to_pos;  
  unsigned int pos = 0; 

  for(const auto transition : net.transitions())
  {
    id_to_pos.emplace(transition.id, pos++);
  }

  

  for(const auto& place : net.places())
  {
    row_type currentRow(net.transitions().size(), false);    

    for(const auto pre : place.pre)
    {
      const auto pos = id_to_pos.find(pre.first)->second;
      currentRow[pos] = true;
    }

    for(const auto post : place.post)
    {
      const auto pos = id_to_pos.find(post.first)->second;
      currentRow[pos] = true;
    }

    idList.push_back(place.id);
    auto insertion = setMatrix.insert( currentRow );
    matrix.push_back(&(*insertion.first));
    //currentRow.clear();

  }

  columnNumber = net.transitions().size();
  rowNumber = matrix.size();

  norm();
}

Matrix* Matrix::copy()
{
  Matrix* newMatrix = new Matrix();

  newMatrix->columnNumber = this->columnNumber;
  newMatrix->rowNumber = this->rowNumber;
  newMatrix->currentNorm = this->currentNorm;
  newMatrix->idList = std::vector<std::string>(this->idList);

  newMatrix->matrix = matrix_type(this->matrix);
  
  return newMatrix;
}

const row_type* Matrix::getRow(unsigned int row)
{

  return matrix.at(row);

}

const column_type* Matrix::getColumn(unsigned int column)
{

  column_type* chosenColumn = NULL;
  chosenColumn->reserve( rowNumber );

  for (unsigned int i = 0; i < columnNumber; ++i)
  {
    (*chosenColumn).push_back( (*(matrix[i])).at(column) );
  }

  return chosenColumn;

}

/*
Not the First One, but the last one before the zeros ... (meaning that this one is at the top of the column, hence the "first")
*/
unsigned int Matrix::indexFirstOne(const unsigned int column)
{
  unsigned int i=0;
  int index = -1;

  while (i<rowNumber-1 && index == -1)
  {

    if (matrix[i]->at(column) == 1 && matrix[i+1]->at(column) == 0 && index == -1)
    {
      index = i;
    }

    ++i;
  }

  if (matrix[rowNumber - 1]->at(column) == 1 && index == -1)
  {
    index = rowNumber - 1;
  }
  else if (index == -1)
  {
    return UINT_MAX;
  }

  return index;
  
}

unsigned int Matrix::indexLastOne(const unsigned int column)
{
  unsigned int i = 0;
  int index = -1;

  while (i<rowNumber && index == -1)
  {

    if (matrix[rowNumber - 1 - i]->at(column) == 1 && index == -1)
    {
      index = rowNumber - 1 - i;
    }

    ++i;
  }

  if (index == -1)
  {
    return UINT_MAX;
  }

  return index;

}

//NE CHANGE PAS LA NORME !
void Matrix::swapRows(const unsigned int row1, const unsigned int row2)
{

  const row_type* temp = matrix[row1];
  matrix[row1] = matrix[row2];
  matrix[row2] = temp;

  const std::string tempInt = idList.at(row1);
  idList.at(row1) = idList.at(row2);
  idList.at(row2) = tempInt;

}

void Matrix::displayMatrix()
{

  for (unsigned int i = 0; i<rowNumber; i++)
  {
    for (unsigned int j = 0; j<columnNumber; j++)
    {

      std::cout << (matrix.at(i)->at(j) == true ? 1 : 0) ;

    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

}

unsigned int Matrix::normColumn(const unsigned int j)
{

  unsigned int i = 0, columnNorm = 0;
  int start = -1, end = -1;

  while (i<rowNumber && (start == -1 || end == -1))
  {

    if (matrix[i]->at(j) == 1 && start == -1)
    {
      start = i;
    }

    if (matrix[rowNumber - 1 - i]->at(j) == 1 && end == -1)
    {
      end = rowNumber - 1 - i;
    }

    ++i;
  }


  if (start != -1 && end != -1 && start != end)
  {
    for (int index = start + 1; index <= end; index++)
    {
      if (matrix[index]->at(j) == 0)
      {
        columnNorm++;
      }
    }
  }

  return columnNorm;

}

unsigned int Matrix::norm()
{

  this->currentNorm = 0;

  for (unsigned int j = 0; j < columnNumber; ++j)
  {
    this->currentNorm += normColumn(j);
  }

  return this->currentNorm;
}

unsigned int Matrix::sumRow(const row_type* row)
{

  unsigned int result = 0;

  for(unsigned int i = 0; i< row->size(); ++i)
  {
    if(row->at(i)==true)
    {
      result++;
    }
  }

  return result;

}

void Matrix::clear()
{
  matrix.clear();
}

unsigned int Matrix::highestNorm()
{
  unsigned int index = 0;
  unsigned int cNorm = normColumn(0), newNorm = 0;

  for (unsigned int i = 1; i < columnNumber; ++i)
  {
    newNorm = normColumn(i);
    if (cNorm < newNorm)
    {
      index = i;
      cNorm = newNorm;
    }
  }

  return index;
}

//Si des lignes ont (30%) des 1 au même endroit, les rapprocher ? 
//Norme limitée ? utilité ?
void Matrix::order()
{

  unsigned int indexMaxNorm = 0, oldNorm = 1, newNorm = 0;
  unsigned int indexFirstOne = 0, indexLastOne = 0;

  //Tech 1 -> Denser rows at the top -> Not working
  /*
  for(unsigned int i = 0; i < rowNumber-1; ++i)
  {
    for(unsigned int j = 0; j < rowNumber-1; ++j)
    {
      if(sumRow(matrix[j]) <= sumRow(matrix[j+1]))
      {
        swapRows(j,j+1);
      }
    }
  }
  */

  //Tech 2 -> Group the same lines (can't stop) -> OK
  for(unsigned int i = 0; i < rowNumber; ++i)
  {
    for(unsigned int j = i; j < rowNumber; ++j)
    {
      
      if(matrix.at(i) == matrix.at(j) && i!=j)
      {
        swapRows(i+1,j);
      }
    }
  }
    
  std::cout << "Fin Tech 2" << std::endl;  

  //Tech 3 -> Isolate the 0 (to the top and bottom)
  
  
  std::cout << "Fin Tech 3" << std::endl;
  std::cout << std::endl;

  while (oldNorm > newNorm && oldNorm!=0)
  {
    oldNorm = newNorm;
  
    if (oldNorm != 0)
    {
      indexMaxNorm = highestNorm();

      //std::cout << indexMaxNorm << std::endl;
      indexFirstOne = this->indexFirstOne(indexMaxNorm);
      //std::cout << indexFirstOne << std::endl;
      indexLastOne = this->indexLastOne(indexMaxNorm);
      //std::cout << indexLastOne << std::endl;

      if (indexFirstOne < indexLastOne && matrix.at(indexFirstOne+1) != matrix.at(indexFirstOne)){
        swapRows(indexFirstOne + 1, indexLastOne);
      }
    }
    
    newNorm = norm();
  }

  if (oldNorm < newNorm && indexFirstOne < indexLastOne)
  {
    swapRows(indexFirstOne+1, indexLastOne);
    norm();
  }

  
  
}
