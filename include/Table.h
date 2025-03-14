#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

struct Lexeme {
    std::string value;
    int type;
};

class Table {
protected:
    std::vector<Lexeme> table;
    std::string filename;

    void loadFromFile();

public:
    Table(int code);
    void searchByValue(std::string value);
    void findLexemebyIndex(const int& index);
    void printTable() const;

    // Новые методы доступа
    size_t getTableSize() const { return table.size(); }
    const Lexeme& getLexeme(size_t index) const { return table[index]; }
};

#endif