#include "matrix.hpp"

int main() {
    Matrix m;

    // Print the initial matrix
    m.print();
    while (!m.CheckIfFull()) {
    // Call updateMatrix to allow user to modify the matrix
    m.updateMatrix();

    // Print the updated matrix
    m.print();
    }

    return 0;
}


