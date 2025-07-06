#pragma once

#include "node.hpp"
#include <vector>
#include <map> // Required for the adjacency list

class Stick {
public:
    Stick(int id);

    int getId() const;
    Node& getNode(Node::Type type);
    const Node& getNode(Node::Type type) const;
    Node& getNodeByIndex(int index);

    bool isInEquilibrium() const;

    // --- New Methods for Adjacency Tracking ---
    void addConnection(int adjacent_stick_id);
    int getConnectionCount(int adjacent_stick_id) const;


private:
    int id_;
    Node e1_;
    Node m_;
    Node e2_;

    // Adjacency list: maps an adjacent stick's ID to the number of connections
    std::map<int, int> adjacent_sticks_;
};