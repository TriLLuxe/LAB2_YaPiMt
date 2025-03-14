#include "HashTable.h"

int HashTable::hashFun(std::string elem, int size) const { // Добавляем const
    int hash = 0;
    for (char ch : elem) hash += ch;
    return hash % size;
}

void HashTable::loadFromFile() {
    std::string lexeme;
    std::ifstream file(filename);
    int t = std::stoi(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    while (file >> lexeme) {
        add(lexeme, t);
    }
    file.close();
}

HashTable::HashTable(int code, int initialSize) : elems(initialSize) {
    filename = std::to_string(code) + ".txt";
    loadFromFile();
}

void HashTable::add(std::string& elem, int type) {
    int index = hashFun(elem, elems.size());
    if (elems.size() <= index) elems.resize(index + 1);

    while (!elems[index].empty() && elems[index][0].value != elem) {
        index = (index + 1) % elems.size();
    }

    if (!elems[index].empty() && elems[index][0].value == elem) {
        std::cout << "Element " << elem << " already exists in index " << index << "." << std::endl;
        return;
    }
    for (auto& lex : elems[index]) {
        if (lex.value == elem) {
            std::cout << "Element " << elem << " already exists in index " << index << "." << std::endl;
            return;
        }
    }
    elems[index].push_back({elem, type});
    std::cout << "Element " << elem << " added to index " << index << "." << std::endl;
}

void HashTable::search(std::string& elem) {
    int index = hashFun(elem, elems.size());
    while (!elems[index].empty()) {
        if (!elems[index].empty() && elems[index][0].value == elem) {
            std::cout << "Lexeme index in the table: " << index << ", Type: " << elems[index][0].type << std::endl;
            return;
        }
        index = (index + 1) % elems.size();
    }
    std::cout << "Lexeme '" << elem << "' not found!" << std::endl;
}

void HashTable::printHashTable() const {
    std::cout << "Hash table:" << std::endl;
    for (int i = 0; i < elems.size(); i++) {
        for (auto& lexeme : elems[i]) {
            std::cout << "Index: " << i << ", Value: " << lexeme.value << ", Type: " << lexeme.type << std::endl;
        }
    }
}

void HashTable::remove(std::string& elem) {
    if (elems.empty()) {
        std::cout << "Table is empty!" << std::endl;
        return;
    }

    int index = hashFun(elem, elems.size());
    while (!elems[index].empty()) {
        if (!elems[index].empty() && elems[index][0].value == elem) {
            elems[index].clear();
            std::cout << "Element '" << elem << "' removed from index " << index << "." << std::endl;
            return;
        }
        index = (index + 1) % elems.size();
    }
    std::cout << "Element '" << elem << "' not found!" << std::endl;
}

void HashTable::saveToFile() const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }
    for (const auto& chain : elems) {
        for (const auto& lexeme : chain) {
            file << lexeme.value << std::endl;
        }
    }
    file.close();
}