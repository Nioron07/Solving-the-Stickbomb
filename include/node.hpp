#pragma once

#include <set> // Required for std::set

class Node {
public:
    enum class Type { E1, M, E2 };

    Node(int id, Type type);

    int getId() const;
    Type getType() const;
    int getTension() const;
    void setTension(int tension);

    // --- New Methods for Adjacency List ---
    void addConnection(Node* node);
    const std::set<Node*>& getConnections() const;

private:
    int id_;
    Type type_;
    int tension_{0};
    std::set<Node*> adjacent_nodes_; // Adjacency list
};