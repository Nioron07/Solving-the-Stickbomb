#include "node.hpp"

Node::Node(int id, Type type) : id_(id), type_(type) {}

int Node::getId() const {
    return id_;
}

Node::Type Node::getType() const {
    return type_;
}

int Node::getTension() const {
    return tension_;
}

void Node::setTension(int tension) {
    tension_ = tension;
}