//
// Created by luisl on 10/05/2025.
//

#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>

enum class NodeType { INTERNAL, LEAF };

struct Node {
    NodeType type;
    int id;
    std::vector<int> keys;
    std::vector<int> pointers; // filhos ou dados
    int nextLeaf = -1;

    std::string serialize() const;
    static Node deserialize(const std::string& line);
};



#endif //NODE_H
