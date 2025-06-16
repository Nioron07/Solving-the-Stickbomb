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
    if (num1 < 0 || num1 >= n_ || num2 < 0 || num2 >= n_ || (data_[num1][num2] != "0" && data_[num1][num2] != "-" && data_[num1][num2] != "+")) {
        Clear();
        std::cout << "Invalid connection" << std::endl;
        return;
    }
    std::cout << "Enter operation ('+' or '-'): ";
    std::cin >> op;

    // Update the matrix based on the operation
    if (op == '+' && (data_[num1][num2] != "-")) {
        data_[num1][num2] = "1";  // Place 1 as "1"
        data_[num2][num1] = "-1";   // Place -1 as "-1"
    } else if (op == '-'  && (data_[num1][num2] != "+")) {
        data_[num1][num2] = "-1";   // Place -1 as "-1"
        data_[num2][num1] = "1";  // Place 1 as "1"
    } else {
        Clear();
        std::cout << "Invalid operation. Use '+' or '-' only and the sign must match the required sign." << std::endl;
        return;
    }

    // Replace zeros in both the rows and columns of the specified numbers with '-'
    EdgeTypeRules(num1_loc, num2_loc, num1, num2, op);
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




/**************************************************************************
 * Matrix::EdgeTypeRules
 * ---------------------
 * Update the adjacency matrix `data_` after the user joins two “sticks”.
 *
 *  ❶ End ↔ End   or   Mid ↔ Mid
 *     • Every untouched cell in the two 3 × 3 rectangles becomes "x".
 *
 *  ❷ Mid ↔ End   (exactly one side is “M”)
 *     • First blanket-fill the two rectangles with 'x'.
 *     • Then add four signs:
 *         – The **End-stick** block gets a horizontal pair of the
 *           *user* sign (`+` or `-`).
 *         – The **Mid-stick** block gets a vertical pair of the
 *           *inverse* sign.
 **************************************************************************/
void Matrix::EdgeTypeRules(Location  node1Location,
                           Location  node2Location,
                           int       node1Index,
                           int       node2Index,
                           char      userSign)
{
    /* --------------------------------------------------------------- */
    /* 1. Map each node index → its 3 × 3 stick block [start, end]     */
    /* --------------------------------------------------------------- */
    auto stickRange = [this](int nodeIdx, int& start, int& end)
    {
        GetStickIndices(nodeIdx, start, end);      // inclusive ends
    };

    int n1Start{}, n1End{};
    int n2Start{}, n2End{};
    stickRange(node1Index, n1Start, n1End);
    stickRange(node2Index, n2Start, n2End);

    /* --------------------------------------------------------------- */
    /* 2. Helpers                                                      */
    /* --------------------------------------------------------------- */

    /* Write 'x', '+' or '-', but allow a sign to overwrite any 'x'.   */
    auto writeCell = [this](int r, int c, const std::string& v)
    {
        if (data_[r][c] == "0" || (v != "x" && data_[r][c] == "x"))
            data_[r][c] = v;
    };

    /* Blanket-fill both rectangles (A×B and B×A) with 'x'.            */
    auto fillRectsWithX = [&](int aStart, int aEnd, int bStart, int bEnd)
    {
        for (int r = aStart; r <= aEnd; ++r)
            for (int c = bStart; c <= bEnd; ++c)
            {
                writeCell(r, c, "x");   // A × B
                writeCell(c, r, "x");   // B × A (mirror)
            }
    };

    /* --------------------------------------------------------------- */
    /* 3. Work out the link type                                       */
    /* --------------------------------------------------------------- */
    const bool n1IsMid = (node1Location == Location::M);
    const bool n2IsMid = (node2Location == Location::M);

    /* ---------- CASE A : End–End  OR  Mid–Mid  --------------------- */
    if (n1IsMid == n2IsMid)          // both true  OR  both false
    {
        fillRectsWithX(n1Start, n1End, n2Start, n2End);
        return;
    }

    /* ---------- CASE B : Mid–End  ---------------------------------- */

    /* Identify which block is End, which is Mid — independent of the
       row/column orientation.                                         */
    int endStart, endEnd, midStart, midEnd;

    if (n1IsMid) {                 /* node1 = Mid, node2 = End */
        midStart = n1Start;  midEnd = n1End;
        endStart = n2Start;  endEnd = n2End;
    } else {                       /* node1 = End, node2 = Mid */
        endStart = n1Start;  endEnd = n1End;
        midStart = n2Start;  midEnd = n2End;
    }

    const int endCentre = endStart + 1;        // centre row & col of End block
    const int midCentre = midStart + 1;        // centre row & col of Mid block

    const std::string  signStr        (1,  userSign);
    const std::string  inverseSignStr (1, (userSign == '+') ? '-' : '+');

    /* 3·1  blanket pass — fill everything with 'x' first ------------- */
    fillRectsWithX(midStart, midEnd, endStart, endEnd);

    /* 3·2  overwrite the four sign cells ----------------------------- */

    /* Horizontal user-sign in End block (centre ROW of End, at the
       left & right ends of the Mid block’s columns).                  */
    writeCell(endCentre, midStart, signStr);   // left  '+'
    writeCell(endCentre, midEnd,   signStr);   // right '+'

    /* Vertical inverse-sign in Mid block (centre COL of End, at the
       top & bottom of the Mid block’s rows).                          */
    writeCell(midStart, endCentre, inverseSignStr);  // top    '-'
    writeCell(midEnd,   endCentre, inverseSignStr);  // bottom '-'
}






void Matrix::GetStickIndices(int num, int& start, int& end) {
    start = ((((num) / 3) + 1) * 3) - 3;
    end = ((((num) / 3) + 1) * 3) - 1;
}