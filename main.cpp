#include <iostream>
#include <fstream>
#include <string>
#include "bplustree/BPlusTree.h"

int main() {
    std::ifstream input("in.txt");
    std::ofstream output("out.txt");

    if (!input.is_open()) {
        std::cerr << "Erro ao abrir in.txt\n";
        return 1;
    }

    std::string line;
    int ordem = 0;

    // Ler a primeira linha: FLH/<ordem>
    if (std::getline(input, line) && line.rfind("FLH/", 0) == 0) {
        ordem = std::stoi(line.substr(4));
        output << line << "\n";
    } else {
        std::cerr << "Formato inválido de entrada.\n";
        return 1;
    }

    BPlusTree arvore(ordem);

    while (std::getline(input, line)) {
        if (line.rfind("INC:", 0) == 0) {
            int x = std::stoi(line.substr(4));
            int qtd = arvore.insert(x);
            output << "INC:" << x << "/" << qtd << "\n";
        } else if (line.rfind("BUS=:", 0) == 0) {
            int x = std::stoi(line.substr(5));
            int qtd = arvore.search(x).size();
            output << "BUS=:" << x << "/" << qtd << "\n";
        }
    }

    output << "H/" << arvore.getAltura() << "\n";

    return 0;
}
