#pragma once

class Node {
public:
    enum class Type { E1, M, E2 };

    Node(int id, Type type);

    int getId() const;
    Type getType() const;
    int getTension() const;
    void setTension(int tension);

private:
    int id_;
    Type type_;
    int tension_{0};
};