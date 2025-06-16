/******************************************************************************
 *  main.cpp  —  demo driver for the stick-bomb Matrix class
 *
 *  • Builds a Matrix (user is prompted for stick count).
 *  • Prints the initial scaffold.
 *  • Repeatedly asks the user for connections until the matrix is full.
 ******************************************************************************/

#include "Matrix.hpp"
#include <iostream>

int main()
{
    Matrix matrix;           // user is asked for stick count in the ctor

    std::cout << "\nInitial matrix:\n";
    matrix.print();

    while (!matrix.isFull())
    {
        std::cout << "\nAdd a connection\n";
        matrix.updateMatrix();

        std::cout << "\nCurrent matrix:\n";
        matrix.print();
    }

    std::cout << "\n🎉  Matrix is completely filled.  Goodbye!\n";
    return 0;
}
