#pragma once
/******************************************************************************
 * Matrix.hpp  —  public interface for the “stick-bomb” matrix
 *
 * A project consists of N “sticks”, each rendered as a 3 × 3 sub-matrix:
 *
 * E1  M  E2
 * E1  M  E2
 * E1  M  E2
 *
 * Interactive connections placed by the user propagate into these blocks
 * according to simple, deterministic rules (see Matrix.cpp).
 *
 * Only the high-level API is exposed here.  Implementation details and all
 * helper logic live in the .cpp file.
 ******************************************************************************/

#include <vector>
#include <string>
#include <utility>
#include "stick.hpp" // Include the new stick header

class Matrix
{
public:
    /* ────────────────────────────────────────────────────────────────── */
    /* Node location within a 3 × 3 stick block                         */
    /* ────────────────────────────────────────────────────────────────── */
    enum class Location { E1, M, E2 };

    /* ────────────────────────────────────────────────────────────────── */
    /* Connection type between *two* nodes                              */
    /* ────────────────────────────────────────────────────────────────── */
    enum class Connection {
        EE,   ///< End ↔ End        (E1/E2  with  E1/E2)
        MM,   ///< Mid ↔ Mid        (M      with  M)
        ME    ///< Mid ↔ End (or End ↔ Mid) — exactly one node is Mid
    };

    /// Helper that returns EE / MM / ME for a pair of Locations.
    static Connection connectionType(Location lhs, Location rhs);

    /* ────────────────────────────────────────────────────────────────── */
    /* Construction & user interaction                                  */
    /* ────────────────────────────────────────────────────────────────── */
    Matrix();
    void updateMatrix();
    void print() const;
    void checkSignBounding(char userSign,
                           int first, int second,
                           bool& flagOut);
    bool isFull() const;

    // New helper method
    Stick& getStickFromNode(int nodeNumber);

private:
    /* ────────────────────────────────────────────────────────────────── */
    /* Internal data                                                    */
    /* ────────────────────────────────────────────────────────────────── */
    int matrixSize_{0};
    std::vector<std::vector<std::string>> data_;
    unsigned num_connecs_elim_ = 0;
    std::vector<Stick> sticks_; // Add a vector of sticks

    /* ────────────────────────────────────────────────────────────────── */
    /* Console / input helpers                                          */
    /* ────────────────────────────────────────────────────────────────── */
    static void clearConsole();           ///< blank-line separator
    static void flushBadInput();          ///< reset std::cin
    int  promptStickCount() const;        ///< ask for N (≥ 4)
    int  promptNodeIndex(const char*) const;
    char promptSign() const;              ///< ask for '+' or '-'

    /* ────────────────────────────────────────────────────────────────── */
    /* Low-level cell manipulation                                      */
    /* ────────────────────────────────────────────────────────────────── */
    void writeCell(int row, int col, const std::string& value);
    void applyDirectedSign(int from, int to, char sign);
    bool isWritable(int from, int to) const;

    /* ────────────────────────────────────────────────────────────────── */
    /* Stick / node helpers                                             */
    /* ────────────────────────────────────────────────────────────────── */
    std::pair<int,int> stickBlock(int nodeIdx) const; ///< return [start,end]
    void  initialiseStaticPattern();                  ///< diag scaffold
    Location locationOf(int nodeIdx) const;           ///< map to E1/M/E2
    void assignLocations(int n1, Location& l1,
                         int n2, Location& l2) const;

     /* ────────────────────────────────────────────────────────────────── */
    /* Rule Engine & Helpers                                            */
    /* ────────────────────────────────────────────────────────────────── */
    void applyEdgeTypeRules(Location loc1, Location loc2,
                            int node1_idx, int node2_idx, char userSign);
                            
    void applyMultiConnectionRules(int node1_idx, int node2_idx, Location loc1, Location loc2);
    void applyConnectionLimit(Node& node1, Node& node2);
    // --- New General Helper Functions ---
    void checkAndEnforceTransitiveConnections(int source_node_idx, int newly_connected_node_idx);
    std::vector<Node*> getNodeConnections(int node_idx);
    void enforceConnection(int stick1_id, int stick2_id, Connection type);
};