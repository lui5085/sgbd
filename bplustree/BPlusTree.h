//
// Created by luisl on 10/05/2025.
//

#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <string>
#include <vector>

#include "Node.h"

class BPlusTree {
public:
    BPlusTree(int ordem);

    int insert(int ano_colheita);
    std::vector<int> search(int ano);
    int getAltura() const;

private:
    int ordem;
    int altura;
    int rootId;
    int nextNodeId;

    Node loadNode(int id);
    void saveNode(const Node& node);
    Node findLeaf(int ano, std::vector<Node>& caminho);
    std::vector<int> buscarLinhasCSV(int ano);
    std::string indiceFilename = "indice.txt";
    void splitLeaf(Node& folha);
    void splitLeafComPai(Node folha, std::vector<Node>& caminho);
    void splitInternalComPai(Node interno, std::vector<Node>& caminho);



};


#endif //BPLUSTREE_H
