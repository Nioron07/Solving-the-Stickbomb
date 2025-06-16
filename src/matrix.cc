/******************************************************************************
 *  Matrix.cpp  —  implementation of the stick-bomb matrix
 *
 *  Overview
 *  --------
 *  * A stick-bomb with N sticks builds a matrix of (N × 3)² cells.
 *  * Each stick is a 3 × 3 block whose nodes are labelled
 *      E1 | M | E2  (rows 0–2, cols 0–2).
 *  * The constructor lays down a static ‘x / 2’ diagonal scaffold.
 *  * Each user operation:
 *      1. asks for two node numbers and a sign (+/–)
 *      2. writes ±1 for the directed edge
 *      3. expands that edge into a pattern of x/+/– inside the two blocks.
 ******************************************************************************/

#include "Matrix.hpp"
#include <iomanip>
#include <iostream>
#include <limits>

/* shorten namespace noise */
using std::cout;
using std::cin;
using std::string;
using std::vector;

/* ANSI colour helpers (omit on non-VT consoles) */
#define RESET_COLOUR "\033[0m"
#define RED_COLOUR   "\033[31m"

/* ───────────────────────── ctor / setup ─────────────────────────────── */

Matrix::Matrix()
{
    clearConsole();
    int stickCount = promptStickCount();           // ask ≥ 4

    matrixSize_ = stickCount * 3;                  // each stick → 3 rows/cols
    data_.assign(matrixSize_, vector<string>(matrixSize_, "0"));

    initialiseStaticPattern();                     // scaffold
}

/* ───────────────── public user interface ────────────────────────────── */

void Matrix::updateMatrix()
{
    /* 1. gather user input */
    const int first  = promptNodeIndex("Enter the first number")  - 1; // 0-based
    const int second = promptNodeIndex("Enter the second number") - 1;
    const char userSign = promptSign();

    /* 2. quick bounds & occupancy check */
    if (!isWritable(first, second)) return;

    /* 3. sign-bounding sanity check */
    bool isError{};
    checkSignBounding(userSign, first, second, isError);
    if (isError) return;

    /* 4. classify node locations */
    Location locFirst, locSecond;
    assignLocations(first, locFirst, second, locSecond);

    /* 5. write ±1 for the directed edge */
    applyDirectedSign(first, second, userSign);

    /* 6. expand into the 3×3 stick blocks */
    applyEdgeTypeRules(locFirst, locSecond, first, second, userSign);
}

/* optional helper used above */
void Matrix::checkSignBounding(char sign, int i, int j, bool& flag)
{
    flag = ( (sign == '-' && data_[i][j] == "+") ||
             (sign == '+' && data_[i][j] == "-") );
    if (flag) cout << "Invalid input: sign must match existing bound.\n";
}

/* pretty-print with red x’s and axis guides */
void Matrix::print() const
{
    constexpr int W = 4;
    cout << std::setw(W) << ' ' << " |";
    for (int c = 0; c < matrixSize_; ++c) cout << std::setw(W) << c + 1;
    cout << '\n' << string(W, '-') << "-+" << string(matrixSize_ * W, '-') << '\n';

    for (int r = 0; r < matrixSize_; ++r)
    {
        cout << std::setw(W) << r + 1 << " |";
        for (int c = 0; c < matrixSize_; ++c)
        {
            const string& cell = data_[r][c];
            if (cell == "x")
                cout << RED_COLOUR << std::setw(W) << 'x' << RESET_COLOUR;
            else
                cout << std::setw(W) << cell;
        }
        cout << '\n';
    }
}

/* ───────────────── console helpers ─────────────────────────────────── */

void Matrix::clearConsole() { cout << "\n\n"; }

void Matrix::flushBadInput()
{
    cin.clear();
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cout << "Invalid input. Please enter an integer.\n";
}

int Matrix::promptStickCount() const
{
    int n;
    while (true)
    {
        cout << "Enter number of sticks (>=4): ";
        cin >> n;
        if (cin.fail())           { flushBadInput(); continue; }
        if (n < 4)                { cout << "Must be 4 or more.\n"; continue; }
        return n;
    }
}

int Matrix::promptNodeIndex(const char* label) const
{
    int idx;
    while (true)
    {
        cout << label << " (1-" << matrixSize_ << "): ";
        cin >> idx;
        if (cin.fail())           { flushBadInput(); continue; }
        if (idx < 1 || idx > matrixSize_)
            { cout << "Out of range.\n"; continue; }
        return idx;
    }
}

char Matrix::promptSign() const
{
    char s;
    while (true)
    {
        cout << "Enter operation ('+' or '-'): ";
        cin >> s;
        if (s == '+' || s == '-') return s;
        cout << "Invalid; expect + or -.\n";
    }
}

/* ───────────────── low-level cell ops ──────────────────────────────── */

void Matrix::applyDirectedSign(int i, int j, char sign)
{
    data_[i][j] = (sign == '+') ? "1"  : "-1";
    data_[j][i] = (sign == '+') ? "-1" : "1";
}

bool Matrix::isWritable(int i, int j) const
{
    const string& cur = data_[i][j];
    if (cur == "0" || cur == "+" || cur == "-") return true;
    cout << "Invalid: target cell is occupied.\n";
    return false;
}

void Matrix::writeCell(int r, int c, const string& v)
{
    if (data_[r][c] == "0" || (v != "x" && data_[r][c] == "x"))
        data_[r][c] = v;
}

/* ───────────────── stick helpers ───────────────────────────────────── */

std::pair<int,int> Matrix::stickBlock(int idx) const
{
    int s = (idx / 3) * 3;                   // inclusive [s, s+2]
    return {s, s + 2};
}

void Matrix::initialiseStaticPattern()
{
    for (int b = 0; b < matrixSize_; b += 3)
    {
        data_[b][b]         = "x";
        data_[b+1][b+1]     = "x";
        data_[b+2][b+2]     = "x";
        data_[b][b+2]       = "x";
        data_[b+2][b]       = "x";

        data_[b+1][b]       = "2";
        data_[b][b+1]       = "2";
        data_[b+1][b+2]     = "2";
        data_[b+2][b+1]     = "2";
    }
}

Matrix::Location Matrix::locationOf(int idx) const
{
    switch (idx % 3)
    {
        case 0:  return Location::E1;  // 0,3,6…
        case 1:  return Location::M;   // 1,4,7…
        default: return Location::E2;  // 2,5,8…
    }
}

void Matrix::assignLocations(int n1, Location& l1,
                             int n2, Location& l2) const
{
    l1 = locationOf(n1);
    l2 = locationOf(n2);
}

/* ───────────────── rule engine ─────────────────────────────────────── */

void Matrix::applyEdgeTypeRules(Location l1, Location l2,
                                int n1, int n2, char userSign)
{
    /* 1. block ranges */
    auto [rS, rE] = stickBlock(n1);          // node1 rows
    auto [cS, cE] = stickBlock(n2);          // node2 cols

    /* 2. blanket ‘x’ fill */
    for (int r = rS; r <= rE; ++r)
        for (int c = cS; c <= cE; ++c)
        {
            writeCell(r, c, "x");
            writeCell(c, r, "x");            // mirror
        }

    /* 3. stop early for EE / MM */
    if (connectionType(l1, l2) != Connection::ME) return;

    /* 4. Mid ↔ End special signs */
    const bool endOnRows = (l1 != Location::M);     // true if node1 is END
    int midS, midE, midC;
    int endS, endE, endC;

    if (endOnRows) {
        endS = rS; endE = rE; endC = rS + 1;
        midS = cS; midE = cE; midC = cS + 1;
    } else {
        midS = rS; midE = rE; midC = rS + 1;
        endS = cS; endE = cE; endC = cS + 1;
    }

    const string usr(1, userSign);
    const string inv(1, (userSign == '+') ? '-' : '+');

    writeCell(endC, midS, usr);          // horizontal pair
    writeCell(endC, midE, usr);

    writeCell(midS, endC, inv);          // vertical pair
    writeCell(midE, endC, inv);
}

/* ───────────────── connection helper ───────────────────────────────── */

 /******************************************************************************
  *  connectionType — classify two Locations as EE / MM / ME
  ******************************************************************************/
Matrix::Connection Matrix::connectionType(Location a, Location b)
{
    const bool aMid = (a == Location::M);
    const bool bMid = (b == Location::M);

    if ( aMid &&  bMid) return Connection::MM;
    if (!aMid && !bMid) return Connection::EE;
    return Connection::ME;               // exactly one Middle
}

/* ───────────────── isFull query ────────────────────────────────────── */
bool Matrix::isFull() const
{
    for (const auto& row : data_)
        for (const auto& cell : row)
            if (cell == "0" || cell == "+" || cell == "-") return false;
    return true;
}
