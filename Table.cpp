#include "Table.h"

void Table::loadFromFile() {
    std::string lexeme;
    std::ifstream file(filename);
    int t = std::stoi(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    while (std::getline(file, lexeme)) {
        table.push_back({lexeme, t});
    }
    file.close();
}

Table::Table(int code) {
    filename = std::to_string(code) + ".txt";
    loadFromFile();
}

void Table::searchByValue(std::string value) {
    int tmp = -1;
    for (int i = 0; i < table.size(); i++) {
        if (table[i].value == value) tmp = i;
    }
    tmp == -1 ? std::cout << "No such value in table!" << std::endl : std::cout << "Lexeme index in the table: " << std::to_string(tmp) << std::endl;
}

void Table::findLexemebyIndex(const int& index) {
    std::cout << (index < table.size() ? ("Under this index is " + table[index].value) : "Out of table range.") << std::endl;
}

void Table::printTable() const {
    for (const auto& lexeme : table) {
        std::cout << "Type: " << lexeme.type << ", Value: " << lexeme.value << std::endl;
    }
}