#pragma once
/******************************************************************************
 *  Matrix.hpp  —  public interface for the “stick-bomb” matrix
 *
 *  A project consists of N “sticks”, each rendered as a 3 × 3 sub-matrix:
 *
 *        E1  M  E2
 *        E1  M  E2
 *        E1  M  E2
 *
 *  Interactive connections placed by the user propagate into these blocks
 *  according to simple, deterministic rules (see Matrix.cpp).
 *
 *  Only the high-level API is exposed here.  Implementation details and all
 *  helper logic live in the .cpp file.
 ******************************************************************************/

#include <vector>
#include <string>
#include <utility>      // std::pair

class Matrix
{
public:
    /* ────────────────────────────────────────────────────────────────── */
    /*  Node location within a 3 × 3 stick block                         */
    /* ────────────────────────────────────────────────────────────────── */
    enum class Location { E1, M, E2 };

    /* ────────────────────────────────────────────────────────────────── */
    /*  Connection type between *two* nodes                              */
    /* ────────────────────────────────────────────────────────────────── */
    enum class Connection {
        EE,   ///< End ↔ End        (E1/E2  with  E1/E2)
        MM,   ///< Mid ↔ Mid        (M      with  M)
        ME    ///< Mid ↔ End (or End ↔ Mid) — exactly one node is Mid
    };

    /// Helper that returns EE / MM / ME for a pair of Locations.
    static Connection connectionType(Location lhs, Location rhs);

    /* ────────────────────────────────────────────────────────────────── */
    /*  Construction & user interaction                                  */
    /* ────────────────────────────────────────────────────────────────── */
    Matrix();                 ///< Prompt for stick count and build matrix
    void updateMatrix();      ///< Ask user for a connection and apply it
    void print() const;       ///< Pretty-print coloured matrix to stdout

    /* Inline validator (used by updateMatrix) */
    void checkSignBounding(char userSign,
                           int first, int second,
                           bool& flagOut);

    /* ────────────────────────────────────────────────────────────────── */
    /*  State queries                                                    */
    /* ────────────────────────────────────────────────────────────────── */
    bool isFull() const;      ///< True if no "0" cells remain

private:
    /* ────────────────────────────────────────────────────────────────── */
    /*  Internal data                                                    */
    /* ────────────────────────────────────────────────────────────────── */
    int matrixSize_{0};                                       ///< side length
    std::vector<std::vector<std::string>> data_;              ///< cell text
    unsigned num_connecs_elim_ = 0;                           ///< number of connections eliminated by last made connection

    /* ────────────────────────────────────────────────────────────────── */
    /*  Console / input helpers                                          */
    /* ────────────────────────────────────────────────────────────────── */
    static void clearConsole();           ///< blank-line separator
    static void flushBadInput();          ///< reset std::cin
    int  promptStickCount() const;        ///< ask for N (≥ 4)
    int  promptNodeIndex(const char*) const;
    char promptSign() const;              ///< ask for '+' or '-'

    /* ────────────────────────────────────────────────────────────────── */
    /*  Low-level cell manipulation                                      */
    /* ────────────────────────────────────────────────────────────────── */
    void writeCell(int row, int col, const std::string& value);
    void applyDirectedSign(int from, int to, char sign);
    bool isWritable(int from, int to) const;

    /* ────────────────────────────────────────────────────────────────── */
    /*  Stick / node helpers                                             */
    /* ────────────────────────────────────────────────────────────────── */
    std::pair<int,int> stickBlock(int nodeIdx) const; ///< return [start,end]
    void  initialiseStaticPattern();                  ///< diag scaffold
    Location locationOf(int nodeIdx) const;           ///< map to E1/M/E2
    void assignLocations(int n1, Location& l1,
                         int n2, Location& l2) const;

    /* ────────────────────────────────────────────────────────────────── */
    /*  Rule engine                                                      */
    /* ────────────────────────────────────────────────────────────────── */
    void applyEdgeTypeRules(Location loc1, Location loc2,
                            int node1, int node2, char userSign);
    void applyMultiConnectionRules(Location loc1, Location loc2,
                            int node1, int node2, char userSign);
};
