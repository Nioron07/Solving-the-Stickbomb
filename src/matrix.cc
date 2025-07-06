/******************************************************************************
 * Matrix.cpp  —  implementation of the stick-bomb matrix
 ******************************************************************************/

#include "matrix.hpp"
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>
#include <algorithm>
#include <map>

/* shorten namespace noise */
using std::cin;
using std::cout;
using std::string;
using std::vector;

/* ANSI colour helpers (omit on non-VT consoles) */
#define RESET_COLOUR "\033[0m"
#define RED_COLOUR "\033[31m"

/* ───────────────────────── ctor / setup ─────────────────────────────── */

Matrix::Matrix()
{
    clearConsole();
    int stickCount = promptStickCount();

    matrixSize_ = stickCount * 3;
    data_.assign(matrixSize_, vector<string>(matrixSize_, "0"));

    for (int i = 0; i < stickCount; ++i) {
        sticks_.emplace_back(i);
    }
    initialiseStaticPattern();
}

/* ───────────────── public user interface ────────────────────────────── */

void Matrix::updateMatrix()
{
    num_connecs_elim_ = 0;

    const int first_idx = promptNodeIndex("Enter the first number") - 1;
    const int second_idx = promptNodeIndex("Enter the second number") - 1;
    const char userSign = promptSign();

    if (!isWritable(first_idx, second_idx)) return;

    bool isError{};
    checkSignBounding(userSign, first_idx, second_idx, isError);
    if (isError) return;

    Location locFirst, locSecond;
    assignLocations(first_idx, locFirst, second_idx, locSecond);
    
    Stick& stick1 = getStickFromNode(first_idx);
    Stick& stick2 = getStickFromNode(second_idx);
    stick1.addConnection(stick2.getId());
    stick2.addConnection(stick1.getId());

    applyDirectedSign(first_idx, second_idx, userSign);

    Node& node1_obj = stick1.getNodeByIndex(first_idx % 3);
    Node& node2_obj = stick2.getNodeByIndex(second_idx % 3);
    
    node1_obj.setTension((userSign == '+') ? 1 : -1);
    node2_obj.setTension((userSign == '+') ? -1 : 1);
    
    node1_obj.addConnection(&node2_obj);
    node2_obj.addConnection(&node1_obj);

    applyEdgeTypeRules(locFirst, locSecond, first_idx, second_idx, userSign);
    applyMultiConnectionRules(first_idx, second_idx, locFirst, locSecond);
    applyConnectionLimit(node1_obj, node2_obj);

    cout << "Number of moves eliminated: " << num_connecs_elim_;
}

/* ───────────────── rule engine ─────────────────────────────────────── */

void Matrix::applyEdgeTypeRules(Location loc1, Location loc2,
                                int node1_idx, int node2_idx, char userSign)
{
    Stick& stick1 = getStickFromNode(node1_idx);
    Stick& stick2 = getStickFromNode(node2_idx);
    
    int count = stick1.getConnectionCount(stick2.getId());

    if (count >= 2) {
        enforceConnection(stick1.getId(), stick2.getId(), Connection::EE);
    } else { 
        auto [rS, rE] = stickBlock(node1_idx);
        auto [cS, cE] = stickBlock(node2_idx);

        for (int r = rS; r <= rE; ++r) {
            for (int c = cS; c <= cE; ++c) {
                writeCell(r, c, "x");
                writeCell(c, r, "x");
            }
        }
        
        data_[node1_idx][node2_idx] = (userSign == '+') ? "1" : "-1";
        data_[node2_idx][node1_idx] = (userSign == '+') ? "-1" : "1";

        if (connectionType(loc1, loc2) == Connection::ME) {
            const string usr_sign(1, userSign);
            const string inv_sign(1, (userSign == '+') ? '-' : '+');
            if (loc1 != Location::M) { 
                writeCell(rS + 1, cS, usr_sign);
                writeCell(rS + 1, cE, usr_sign);
                writeCell(cS, rS + 1, inv_sign);
                writeCell(cE, rS + 1, inv_sign);
            } else { 
                writeCell(rS, cS + 1, usr_sign);
                writeCell(rE, cS + 1, usr_sign);
                writeCell(cS + 1, rS, inv_sign);
                writeCell(cS + 1, rE, inv_sign);
            }
        }
    }
}

void Matrix::applyMultiConnectionRules(int node1_idx, int node2_idx, Location loc1, Location loc2) {
    checkAndEnforceTransitiveConnections(node1_idx, node2_idx);
    checkAndEnforceTransitiveConnections(node2_idx, node1_idx);
}

void Matrix::checkAndEnforceTransitiveConnections(int source_node_idx, int newly_connected_node_idx) {
    Node& source_node = getStickFromNode(source_node_idx).getNodeByIndex(source_node_idx % 3);

    vector<Node*> neighbors = getNodeConnections(source_node_idx);

    if (neighbors.size() >= 2) {
        for (size_t i = 0; i < neighbors.size(); ++i) {
            for (size_t j = i + 1; j < neighbors.size(); ++j) {
                int stick1_id = neighbors[i]->getId() / 3;
                int stick2_id = neighbors[j]->getId() / 3;
                enforceConnection(stick1_id, stick2_id, Connection::EE);
            }
        }
    }
}

void Matrix::applyConnectionLimit(Node& node1, Node& node2) {
    if (node1.getConnections().size() >= 2) {
        int node_id = node1.getId();
        for (unsigned i = 0; i < matrixSize_; i++) {
            if (data_[node_id][i] != "1" && data_[node_id][i] != "-1") {
                writeCell(node_id, i, "x");
                writeCell(i, node_id, "x");
            }
        }
    }
    if (node2.getConnections().size() >= 2) {
        int node_id = node2.getId();
        for (unsigned i = 0; i < matrixSize_; i++) {
             if (data_[node_id][i] != "1" && data_[node_id][i] != "-1") {
                writeCell(node_id, i, "x");
                writeCell(i, node_id, "x");
            }
        }
    }
}

/* ───────────────── new general helpers ───────────────────────────────── */

vector<Node*> Matrix::getNodeConnections(int node_idx) {
    vector<Node*> connections;
    for(int i = 0; i < matrixSize_; ++i) {
        if(data_[node_idx][i] == "1" || data_[node_idx][i] == "-1") {
            connections.push_back(&getStickFromNode(i).getNodeByIndex(i % 3));
        }
    }
    return connections;
}


void Matrix::enforceConnection(int stick1_id, int stick2_id, Connection type) {
    if (stick1_id == stick2_id) return;
    auto [rS, rE] = stickBlock(stick1_id * 3);
    auto [cS, cE] = stickBlock(stick2_id * 3);

    if (type == Connection::EE) {
        for (int r = rS; r <= rE; ++r) {
            for (int c = cS; c <= cE; ++c) {
                 if (data_[r][c] != "1" && data_[r][c] != "-1") writeCell(r, c, "x");
                 if (data_[c][r] != "1" && data_[c][r] != "-1") writeCell(c, r, "x");
            }
        }
    }
}


/* ───────────────── low-level cell ops ──────────────────────────────── */

void Matrix::writeCell(int r, int c, const string &val) {
    string& currentCell = data_[r][c];

    if (currentCell == val || currentCell == "2") return;

    if (currentCell == "x" && (val == "+" || val == "-")) {
        currentCell = val;
        num_connecs_elim_--; 
        return;
    }
    
    if (currentCell == "x") return;

    string originalState = currentCell;
    currentCell = val;
    
    if (originalState == "0") {
        num_connecs_elim_ += (val == "x") ? 2 : 1;
    } else if (originalState == "+" || originalState == "-") {
        if (val == "x") num_connecs_elim_++;
    }
}

void Matrix::applyDirectedSign(int i, int j, char sign)
{
    writeCell(i, j, (sign == '+') ? "1" : "-1");
    writeCell(j, i, (sign == '+') ? "-1" : "1");
}

/* ───────────────────────── HELPERS (No Changes Below) ─────────────────────────────── */
void Matrix::checkSignBounding(char sign, int i, int j, bool &flag)
{
    flag = ((sign == '-' && data_[i][j] == "+") ||
            (sign == '+' && data_[i][j] == "-"));
    if (flag)
        cout << "Invalid input: sign must match existing bound.\n";
}

void Matrix::print() const
{
    constexpr int W = 4;
    cout << std::setw(W) << ' ' << " |";
    for (int c = 0; c < matrixSize_; ++c)
        cout << std::setw(W) << c + 1;
    cout << '\n'
         << string(W, '-') << "-+" << string(matrixSize_ * W, '-') << '\n';

    for (int r = 0; r < matrixSize_; ++r)
    {
        cout << std::setw(W) << r + 1 << " |";
        for (int c = 0; c < matrixSize_; ++c)
        {
            const string &cell = data_[r][c];
            if (cell == "x")
                cout << RED_COLOUR << std::setw(W) << 'x' << RESET_COLOUR;
            else
                cout << std::setw(W) << cell;
        }
        cout << '\n';
    }
}

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
        if (cin.fail())
        {
            flushBadInput();
            continue;
        }
        if (n < 4)
        {
            cout << "Must be 4 or more.\n";
            continue;
        }
        return n;
    }
}

int Matrix::promptNodeIndex(const char *label) const
{
    int idx;
    while (true)
    {
        cout << label << " (1-" << matrixSize_ << "): ";
        cin >> idx;
        if (cin.fail())
        {
            flushBadInput();
            continue;
        }
        if (idx < 1 || idx > matrixSize_)
        {
            cout << "Out of range.\n";
            continue;
        }
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
        if (s == '+' || s == '-')
            return s;
        cout << "Invalid; expect + or -.\n";
    }
}

bool Matrix::isWritable(int i, int j) const
{
    const string &cur = data_[i][j];
    if (cur == "0" || cur == "+" || cur == "-")
        return true;
    cout << "Invalid: target cell is occupied.\n";
    return false;
}

std::pair<int, int> Matrix::stickBlock(int idx) const
{
    int s = (idx / 3) * 3;
    return {s, s + 2};
}

void Matrix::initialiseStaticPattern()
{
    for (int b = 0; b < matrixSize_; b += 3)
    {
        data_[b][b] = "x";
        data_[b + 1][b + 1] = "x";
        data_[b + 2][b + 2] = "x";
        data_[b][b + 2] = "x";
        data_[b + 2][b] = "x";

        data_[b + 1][b] = "2";
        data_[b][b + 1] = "2";
        data_[b + 1][b + 2] = "2";
        data_[b + 2][b + 1] = "2";
    }
}

Matrix::Location Matrix::locationOf(int idx) const
{
    switch (idx % 3)
    {
    case 0:
        return Location::E1;
    case 1:
        return Location::M;
    default:
        return Location::E2;
    }
}

void Matrix::assignLocations(int n1, Location &l1,
                             int n2, Location &l2) const
{
    l1 = locationOf(n1);
    l2 = locationOf(n2);
}

Matrix::Connection Matrix::connectionType(Location a, Location b)
{
    const bool aMid = (a == Location::M);
    const bool bMid = (b == Location::M);

    if (aMid && bMid)
        return Connection::MM;
    if (!aMid && !bMid)
        return Connection::EE;
    return Connection::ME;
}

bool Matrix::isFull() const
{
    for (const auto &row : data_)
        for (const auto &cell : row)
            if (cell == "0" || cell == "+" || cell == "-")
                return false;
    return true;
}

Stick& Matrix::getStickFromNode(int nodeNumber) {
    int stickId = nodeNumber / 3;
    return sticks_[stickId];
}
