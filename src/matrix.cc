#include "Matrix.hpp"
#include <iostream>
#include <iomanip>              // for std::setw
#include <limits>
#include <string>

Matrix::Matrix() {
    
    Clear();
    while (true) {
        std::cout << "Enter number of sticks (>=4): ";
        std::cin >> n_;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter an integer.\n";
            continue;
        }
        if (n_ < 4) {
            std::cout << "Number of sticks must be >=4\n";
        } else {
            break;
        }
    }
    n_ = n_ * 3;
    // Initialize matrix with "0" and set the diagonal to "x"
    data_.assign(n_, std::vector<std::string>(n_, "0"));  // Fill all with "0"
    
    // Create the pattern based on the diagonal and adjacent 3-blocks
    for (int i = 0; i < n_; ++i) {
        for (int j = 0; j < n_; ++j) {
            if (i == j) {
                data_[i][j] = "x";  // Diagonal elements are "x"
            }
        }
    }

    // Fill the 3-blocks around the diagonal
    for (int i = 1; i < n_; i += 3) {
        data_[i - 1][i + 1] = "x";
        data_[i + 1][i - 1] = "x";
        
        data_[i][i + 1] = "2";  // Value "2" for adjacent blocks
        data_[i - 1][i] = "2";  // Value "2" for adjacent blocks
        data_[i][i - 1] = "2";  // Value "2" for adjacent blocks
        data_[i + 1][i] = "2";  // Value "2" for adjacent blocks
    }
}

bool Matrix::CheckIfFull() {
    for (int i = 0; i < n_; i++) {
        for (int j = 0; j < n_; j++) {
            if (data_[i][j] == "0") {
                return false;
            }
        }
    }
    return true;
}

void Matrix::updateMatrix() {
    // Get user input directly within the function
    int num1, num2;
    char op;

    std::cout << "Enter the first number (1 to " << n_ << "): ";
    std::cin >> num1;
    std::cout << "Enter the second number (1 to " << n_ << "): ";
    std::cin >> num2;

    // Determine node locations
    Location num1_loc;
    Location num2_loc;
    AssignNodeLocations(num1, num1_loc, num2, num2_loc);

    // Convert user input to 0-indexed
    num1 -= 1;
    num2 -= 1;

    // Ensure the numbers are within bounds
    if (num1 < 0 || num1 >= n_ || num2 < 0 || num2 >= n_ || data_[num1][num2] != "0") {
        Clear();
        std::cout << "Invalid connection" << std::endl;
        return;
    }
    std::cout << "Enter operation ('+' or '-'): ";
    std::cin >> op;

    // Update the matrix based on the operation
    if (op == '+') {
        data_[num1][num2] = "1";  // Place 1 as "1"
        data_[num2][num1] = "-1";   // Place -1 as "-1"
    } else if (op == '-') {
        data_[num1][num2] = "-1";   // Place -1 as "-1"
        data_[num2][num1] = "1";  // Place 1 as "1"
    } else {
        Clear();
        std::cout << "Invalid operation. Use '+' or '-' only." << std::endl;
        return;
    }

    // Replace zeros in both the rows and columns of the specified numbers with '-'
    EdgeTypeRules(num1_loc, num2_loc, num1, num2);
}

void Matrix::AssignNodeLocations(int num1, Matrix::Location &num1_loc, int num2, Matrix::Location &num2_loc)
{
    if (num1 % 3 == 1)
    {
        num1_loc = Location::E1;
    }
    else if (num1 % 3 == 2)
    {
        num1_loc = Location::M;
    }
    else
    {
        num1_loc = Location::E2;
    }

    if (num2 % 3 == 1)
    {
        num2_loc = Location::E1;
    }
    else if (num2 % 3 == 2)
    {
        num2_loc = Location::M;
    }
    else
    {
        num2_loc = Location::E2;
    }
}

void Matrix::print() const
{
    constexpr int cellWidth = 4;

    /* column header ------------------------------------------------------ */
    std::cout << std::setw(cellWidth) << ' ' << " |";      // ← divider here
    for (int j = 0; j < n_; ++j)
        std::cout << std::setw(cellWidth) << j + 1;
    std::cout << '\n';

    /* horizontal ruler --------------------------------------------------- */
    std::cout << std::string(cellWidth, '-') << "-+"        // ← matches the “ |”
              << std::string(n_ * cellWidth, '-') << '\n';

    /* rows ---------------------------------------------------------------- */
    for (int i = 0; i < n_; ++i) {
        std::cout << std::setw(cellWidth) << i + 1 << " |"; // ← divider here

        for (int j = 0; j < n_; ++j) {
            if (data_[i][j] == "x") {
                std::cout << "\033[31m"
                          << std::setw(cellWidth) << 'x'
                          << "\033[0m";
            } else {
                std::cout << std::setw(cellWidth) << data_[i][j];
            }
        }
        std::cout << '\n';
    }
}




void Matrix::EdgeTypeRules(Location n1, Location n2, int num1, int num2) {
    if ((n1 == Location::E1 && n2 == Location::E1) || (n1 == Location::E2 && n2 == Location::E1) || (n1 == Location::E1 && n2 == Location::E2) || (n1 == Location::E2 && n2 == Location::E2)) {
        // End to End connection
        int num1_start;
        int num1_end;
        GetStickIndices(num1, num1_start, num1_end);

        int num2_start;
        int num2_end;
        GetStickIndices(num2, num2_start, num2_end);

        for (int i = num1_start; i <= num1_end; i++) {
            for (int j = num2_start; j <= num2_end; j++) {
                if (data_[i][j] == "0") {
                    data_[i][j] = "x";
                }                
                if (data_[j][i] == "0") {
                    data_[j][i] = "x";
                }
            }
        }
    } else if (n1 == Location::M && n2 == Location::M) {
        // Middle to Middle Connection 
                int num1_start;
        int num1_end;
        GetStickIndices(num1, num1_start, num1_end);

        int num2_start;
        int num2_end;
        GetStickIndices(num2, num2_start, num2_end);

        for (int i = num1_start; i <= num1_end; i++) {
            for (int j = num2_start; j <= num2_end; j++) {
                if (data_[i][j] == "0") {
                    data_[i][j] = "x";
                }                
                if (data_[j][i] == "0") {
                    data_[j][i] = "x";
                }
            }
        }
    }
}

void Matrix::GetStickIndices(int num, int& start, int& end) {
    start = ((((num) / 3) + 1) * 3) - 3;
    end = ((((num) / 3) + 1) * 3) - 1;
}