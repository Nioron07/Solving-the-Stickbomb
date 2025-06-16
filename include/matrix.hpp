#pragma once

#include <vector>
#include <string>

class Matrix {
public:
    enum class Location {
        E1,  // "E1"
        M,   // "M"
        E2   // "E2"
    };

    int n_;
    std::vector<std::vector<std::string>> data_;  // Use string matrix to store '-1' or '-'

private:
    // Assigns node locations
    void AssignNodeLocations(int num1, Matrix::Location &num1_loc, int num2, Matrix::Location &num2_loc);

    // Handles rules regarding the specific edge type
    void EdgeTypeRules(Location n1, Location n2, int num1, int num2, char sign);

    // Gets the stick number (batch of numbers) that the node belongs to
    void GetStickIndices(int num, int& start, int& end);
public:
    // Constructor: prompts user for dimension (must be ≥12 and divisible by 3)
    Matrix();

    // Clears console
    void Clear() { system("cls"); }
    
    // Check if matrix is full (no "0"s left)
    bool CheckIfFull();

    // New function to update matrix with user input
    void updateMatrix();

    // Print the matrix to console
    void print() const;
};
