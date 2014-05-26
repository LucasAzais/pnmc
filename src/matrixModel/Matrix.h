#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include <vector>
#include <unordered_set>

#include "pn/net.hh"

using row_type = std::vector<bool>;
using column_type = std::vector<bool>;
using matrix_type = std::vector<const row_type*>;

class Matrix
{
  private:
    unsigned int columnNumber;
    unsigned int rowNumber;
    unsigned int currentNorm;
    matrix_type matrix; //1 vector = 1 row
    std::unordered_set<row_type> setMatrix;
    std::vector<std::string> idList;

  public:
    Matrix();
    Matrix(const unsigned int height, const unsigned int width);
    Matrix(const pnmc::pn::net& net);
    Matrix* copy();
    unsigned int getColumnNumber() { return columnNumber; }
    unsigned int getRowNumber() { return rowNumber; }
    unsigned int getNorm() { return currentNorm; }
    std::vector<std::string> getStringIdList() { return idList; }

    const row_type* getRow(unsigned int row);
    const column_type* getColumn(unsigned int column);
    unsigned int indexFirstOne(const unsigned int column);
    unsigned int indexLastOne(const unsigned int column);
    void swapRows(const unsigned int row1, const unsigned int row2);
    void displayMatrix();
    unsigned int normColumn(const unsigned int j);
    unsigned int norm();
    unsigned int sumRow(const row_type* row);
    void clear();

    unsigned int highestNorm();
    void order();
};

#endif // MATRIX_H_INCLUDED
