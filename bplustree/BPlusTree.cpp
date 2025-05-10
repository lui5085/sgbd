//
// Created by luisl on 10/05/2025.
//
#include "BPlusTree.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

BPlusTree::BPlusTree(int ordem) : ordem(ordem), altura(1), rootId(0), nextNodeId(1) {
    // Cria um nó raiz folha vazio no início
    std::ifstream f(indiceFilename);
    if (!f.good()) {
        Node root;
        root.type = NodeType::LEAF;
        root.id = 0;
        saveNode(root);
    }
}

Node BPlusTree::loadNode(int id) {
    std::ifstream file(indiceFilename);
    std::string line;
    int currentId = 0;

    while (std::getline(file, line)) {
        if (currentId == id) {
            return Node::deserialize(line);
        }
        ++currentId;
    }

    throw std::runtime_error("Nó não encontrado no arquivo.");
}

void BPlusTree::saveNode(const Node& node) {
    std::fstream file(indiceFilename, std::ios::in | std::ios::out);
    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    file.close();

    std::string serialized = node.serialize();
    if (node.id < lines.size()) {
        lines[node.id] = serialized;
    } else {
        while (lines.size() < node.id) {
            lines.push_back(""); // preencher até o id desejado
        }
        lines.push_back(serialized);
    }

    std::ofstream outFile(indiceFilename, std::ios::trunc);
    for (const auto& l : lines) {
        outFile << l << "\n";
    }
}

Node BPlusTree::findLeaf(int ano, std::vector<Node>& caminho) {
    Node current = loadNode(rootId);
    caminho.push_back(current); // começa pela raiz

    while (current.type == NodeType::INTERNAL) {
        bool found = false;

        for (size_t i = 0; i < current.keys.size(); ++i) {
            if (ano < current.keys[i]) {
                current = loadNode(current.pointers[i]);
                caminho.push_back(current);
                found = true;
                break;
            }
        }

        if (!found) {
            current = loadNode(current.pointers.back());
            caminho.push_back(current);
        }
    }

    return current;
}


std::vector<int> BPlusTree::buscarLinhasCSV(int ano) {
    std::vector<int> linhas;
    std::ifstream file("vinhos.csv");
    std::string linha;
    int idx = 0;

    std::string header;
    std::getline(file, header); // descarta o cabeçalho

    while (std::getline(file, linha)) {
        std::istringstream ss(linha);
        std::string campo;
        int col = 0;
        int ano_colheita = -1;

        while (std::getline(ss, campo, ',')) {
            if (col == 2) {
                try {
                    ano_colheita = std::stoi(campo);
                } catch (...) {
                    ano_colheita = -1; // ignora linha com erro
                }
                break;
            }
            ++col;
        }

        if (ano_colheita == ano) {
            linhas.push_back(idx);
        }

        ++idx;
    }

    return linhas;
}

int BPlusTree::insert(int ano) {
    std::vector<int> linhas = buscarLinhasCSV(ano);
    if (linhas.empty()) return 0;

    std::vector<Node> caminho;
    Node folha = findLeaf(ano, caminho);

    for (int linha : linhas) {
        auto it = std::lower_bound(folha.keys.begin(), folha.keys.end(), ano);
        size_t pos = it - folha.keys.begin();
        folha.keys.insert(folha.keys.begin() + pos, ano);
        folha.pointers.insert(folha.pointers.begin() + pos, linha);
    }

    if (folha.keys.size() >= ordem) {
        splitLeafComPai(folha, caminho);
    } else {
        saveNode(folha);
    }

    return static_cast<int>(linhas.size());
}



void BPlusTree::splitLeaf(Node& folha) {
    // Criar nova folha
    Node novaFolha;
    novaFolha.type = NodeType::LEAF;
    novaFolha.id = nextNodeId++;
    novaFolha.nextLeaf = folha.nextLeaf;
    folha.nextLeaf = novaFolha.id;

    // Dividir os elementos
    int total = static_cast<int>(folha.keys.size());
    int meio = total / 2;

    // Mover metade para a nova folha
    novaFolha.keys.assign(folha.keys.begin() + meio, folha.keys.end());
    novaFolha.pointers.assign(folha.pointers.begin() + meio, folha.pointers.end());

    // Reduzir a folha original
    folha.keys.resize(meio);
    folha.pointers.resize(meio);

    // Salvar folhas
    saveNode(folha);
    saveNode(novaFolha);

    // Promover primeira chave da nova folha
    int chavePromovida = novaFolha.keys[0];

    if (folha.id == rootId) {
        // criar nova raiz
        Node novaRaiz;
        novaRaiz.type = NodeType::INTERNAL;
        novaRaiz.id = nextNodeId++;
        novaRaiz.keys.push_back(chavePromovida);
        novaRaiz.pointers.push_back(folha.id);
        novaRaiz.pointers.push_back(novaFolha.id);
        saveNode(novaRaiz);
        rootId = novaRaiz.id;
        altura++;
    } else {
        // TODO: inserir chave no nó pai (caso mais geral)
        // Requer manter rastreamento de ancestrais (pilha, recursão etc.)
        std::cerr << "ERRO: Split de folha não tratada para não-raiz.\n";
        exit(1);
    }
}

std::vector<int> BPlusTree::search(int ano) {
    std::vector<int> resultado;
    std::vector<Node> caminho;
    Node folha = findLeaf(ano, caminho); // usa a versão com caminho agora

    while (true) {
        for (size_t i = 0; i < folha.keys.size(); ++i) {
            if (folha.keys[i] == ano) {
                resultado.push_back(folha.pointers[i]);
            } else if (folha.keys[i] > ano) {
                return resultado; // já passou do valor buscado
            }
        }

        if (folha.nextLeaf == -1) break;
        folha = loadNode(folha.nextLeaf);
    }

    return resultado;
}


int BPlusTree::getAltura() const {
    return altura;
}

void BPlusTree::splitLeafComPai(Node folha, std::vector<Node>& caminho) {
    Node novaFolha;
    novaFolha.type = NodeType::LEAF;
    novaFolha.id = nextNodeId++;
    novaFolha.nextLeaf = folha.nextLeaf;
    folha.nextLeaf = novaFolha.id;

    int total = folha.keys.size();
    int meio = total / 2;

    novaFolha.keys.assign(folha.keys.begin() + meio, folha.keys.end());
    novaFolha.pointers.assign(folha.pointers.begin() + meio, folha.pointers.end());

    folha.keys.resize(meio);
    folha.pointers.resize(meio);

    saveNode(folha);
    saveNode(novaFolha);

    int chavePromovida = novaFolha.keys[0];

    // Caminho deve ter pelo menos 2 nós (pai está antes da folha)
    if (caminho.size() < 2) {
        // Criar nova raiz
        Node novaRaiz;
        novaRaiz.type = NodeType::INTERNAL;
        novaRaiz.id = nextNodeId++;
        novaRaiz.keys.push_back(chavePromovida);
        novaRaiz.pointers.push_back(folha.id);
        novaRaiz.pointers.push_back(novaFolha.id);
        saveNode(novaRaiz);
        rootId = novaRaiz.id;
        altura++;
        return;
    }

    // Subir para o pai
    Node pai = caminho[caminho.size() - 2];
    auto it = std::upper_bound(pai.keys.begin(), pai.keys.end(), chavePromovida);
    int pos = it - pai.keys.begin();
    pai.keys.insert(pai.keys.begin() + pos, chavePromovida);
    pai.pointers.insert(pai.pointers.begin() + pos + 1, novaFolha.id); // ponteiro direito

    saveNode(pai);

    if (pai.keys.size() >= ordem) {
        caminho.pop_back(); // remove folha
        caminho.pop_back(); // remove pai atual
        splitInternalComPai(pai, caminho);
    }
}

void BPlusTree::splitInternalComPai(Node interno, std::vector<Node>& caminho) {
    Node novoInterno;
    novoInterno.type = NodeType::INTERNAL;
    novoInterno.id = nextNodeId++;

    int total = interno.keys.size();
    int meio = total / 2;

    // A chave a ser promovida para o pai
    int chavePromovida = interno.keys[meio];

    // O novo nó recebe a metade superior (à direita da chave promovida)
    novoInterno.keys.assign(interno.keys.begin() + meio + 1, interno.keys.end());
    novoInterno.pointers.assign(interno.pointers.begin() + meio + 1, interno.pointers.end());

    // O nó original fica com a metade inferior
    interno.keys.resize(meio);
    interno.pointers.resize(meio + 1); // um ponteiro a mais que o número de chaves

    saveNode(interno);
    saveNode(novoInterno);

    // Se interno era a raiz, criamos uma nova raiz
    if (caminho.empty()) {
        Node novaRaiz;
        novaRaiz.type = NodeType::INTERNAL;
        novaRaiz.id = nextNodeId++;
        novaRaiz.keys.push_back(chavePromovida);
        novaRaiz.pointers.push_back(interno.id);
        novaRaiz.pointers.push_back(novoInterno.id);
        saveNode(novaRaiz);
        rootId = novaRaiz.id;
        altura++;
        return;
    }

    // Caso contrário, vamos inserir no pai
    Node pai = caminho.back();
    caminho.pop_back();

    auto it = std::upper_bound(pai.keys.begin(), pai.keys.end(), chavePromovida);
    int pos = it - pai.keys.begin();

    pai.keys.insert(pai.keys.begin() + pos, chavePromovida);
    pai.pointers.insert(pai.pointers.begin() + pos + 1, novoInterno.id); // ponteiro da direita

    saveNode(pai);

    // Se o pai também transbordar, propaga
    if (pai.keys.size() >= ordem) {
        splitInternalComPai(pai, caminho);
    }
}





#include "BPlusTree.h"
