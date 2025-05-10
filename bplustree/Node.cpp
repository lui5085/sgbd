//
// Created by luisl on 10/05/2025.
//
#include "Node.h"
#include <sstream>
#include <string>
#include <vector>

std::string join(const std::vector<int>& vec, char sep) {
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << vec[i];
        if (i + 1 < vec.size()) oss << sep;
    }
    return oss.str();
}

std::vector<int> splitInts(const std::string& s, char sep) {
    std::vector<int> result;
    std::istringstream iss(s);
    std::string token;
    while (std::getline(iss, token, sep)) {
        result.push_back(std::stoi(token));
    }
    return result;
}

std::string Node::serialize() const {
    std::ostringstream oss;
    oss << (type == NodeType::LEAF ? "LEAF" : "INTERNAL") << "|"
        << id << "|"
        << join(keys, ',') << "|"
        << join(pointers, ',');
    if (type == NodeType::LEAF) {
        oss << "|" << nextLeaf;
    }
    return oss.str();
}

Node Node::deserialize(const std::string& line) {
    Node node;
    std::istringstream iss(line);
    std::string part;

    std::getline(iss, part, '|');
    node.type = (part == "LEAF") ? NodeType::LEAF : NodeType::INTERNAL;

    std::getline(iss, part, '|');
    node.id = std::stoi(part);

    std::getline(iss, part, '|');
    node.keys = (part.empty()) ? std::vector<int>() : splitInts(part, ',');

    std::getline(iss, part, '|');
    node.pointers = (part.empty()) ? std::vector<int>() : splitInts(part, ',');

    if (node.type == NodeType::LEAF && std::getline(iss, part, '|')) {
        node.nextLeaf = std::stoi(part);
    }

    return node;
}
#include "Node.h"
