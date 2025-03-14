#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "Table.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

class HashTable {
protected:
    std::vector<std::vector<Lexeme>> elems;
    std::string filename;
    int hashFun(std::string elem, int size) const; // Добавляем const

    void loadFromFile();

public:
    HashTable(int code, int initialSize = 1);
    void add(std::string& elem, int type);
    void search(std::string& elem);
    void printHashTable() const;
    void remove(std::string& elem);
    void saveToFile() const;

    // Методы доступа
    size_t getElemsSize() const { return elems.size(); }
    const std::vector<Lexeme>& getChain(size_t index) const { return elems[index]; }
    int computeHash(const std::string& elem) const { return hashFun(elem, elems.size()); }
};

#endif