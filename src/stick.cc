#include "stick.hpp"
#include <stdexcept>

Stick::Stick(int id)
    : id_(id),
      e1_(id * 3, Node::Type::E1),
      m_(id * 3 + 1, Node::Type::M),
      e2_(id * 3 + 2, Node::Type::E2) {}

int Stick::getId() const {
    return id_;
}

// ... (getNode methods remain the same) ...

Node& Stick::getNode(Node::Type type) {
    switch (type) {
        case Node::Type::E1: return e1_;
        case Node::Type::M:  return m_;
        case Node::Type::E2: return e2_;
        default: throw std::out_of_range("Invalid Node::Type specified.");
    }
}

const Node& Stick::getNode(Node::Type type) const {
    switch (type) {
        case Node::Type::E1: return e1_;
        case Node::Type::M:  return m_;
        case Node::Type::E2: return e2_;
        default: throw std::out_of_range("Invalid Node::Type specified.");
    }
}

Node& Stick::getNodeByIndex(int index) {
    switch (index) {
        case 0: return e1_;
        case 1: return m_;
        case 2: return e2_;
        default: throw std::out_of_range("Invalid node index specified.");
    }
}


bool Stick::isInEquilibrium() const {
    const int e1_tension = e1_.getTension();
    const int e2_tension = e2_.getTension();
    const int m_tension = m_.getTension();

    return (e1_tension == e2_tension) && (e1_tension == -m_tension) && (e1_tension != 0);
}

// --- New Method Implementations ---

void Stick::addConnection(int adjacent_stick_id) {
    adjacent_sticks_[adjacent_stick_id]++;
}

int Stick::getConnectionCount(int adjacent_stick_id) const {
    auto it = adjacent_sticks_.find(adjacent_stick_id);
    if (it != adjacent_sticks_.end()) {
        return it->second;
    }
    return 0;
}