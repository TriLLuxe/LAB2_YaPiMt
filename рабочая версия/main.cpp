#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <filesystem>
#include <string>
#include <cctype>
using namespace std;

struct Lexeme {
    string value;
    int type;
};

class Table {
protected:
    vector<Lexeme> table;
    string filename;

    void loadFromFile() {
        ifstream file(filename);
        int t = stoi(filename);
        if (!file.is_open()) throw runtime_error("Could not open file: " + filename);
        string lexeme;
        while (getline(file,lexeme)) table.push_back({lexeme, t});
        file.close();
    }

public:
    Table(int code) {
        filename = to_string(code) + ".txt";
        loadFromFile();
    }

    bool searchByValue(const string& value) const {
        for (const auto& lexeme : table) {
            if (lexeme.value == value) return true;
        }
        return false;
    }

    int findIndexByValue(const string& value) const {
        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i].value == value) return i;
        }
        return -1;
    }

    void printTable() const {
        for (const auto& lexeme : table) {
            cout << "Type: " << lexeme.type << ", Value: " << lexeme.value << endl;
        }
    }
};

class KeyWordsTable : public Table {
public:
    KeyWordsTable() : Table(10) {}
};

class DelimitersTable : public Table {
public:
    DelimitersTable() : Table(20) {}
};

class HashTable {
protected:
    vector<vector<Lexeme>> elems;
    string filename;

    int hashFun(const string& elem, int size) const {
        int hash = 0;
        for (char ch : elem) hash += ch;
        return hash % size;
    }

    void loadFromFile() {
        ifstream file(filename);
        int t = stoi(filename);
        if (!file.is_open()) throw runtime_error("Could not open file: " + filename);
        string lexeme;
        while (file >> lexeme) add(lexeme, t);
        file.close();
    }

public:
    HashTable(int code, int initialSize = 10) : elems(initialSize) {
        filename = to_string(code) + ".txt";
        loadFromFile();
    }

    void add(string& elem, int type) {
        int index = hashFun(elem, elems.size());
        while (!elems[index].empty() && elems[index][0].value != elem) {
            index = (index + 1) % elems.size();
        }
        if (!elems[index].empty() && elems[index][0].value == elem) {
            //cout << "Element " << elem << " already exists in index " << index << "." << endl;
            return;
        }
        elems[index].push_back({elem, type});
        //cout << "Element " << elem << " added to index " << index << "." << endl;
    }

    int search(const string& elem) const {
        int index = hashFun(elem, elems.size());
        while (!elems[index].empty()) {
            if (!elems[index].empty() && elems[index][0].value == elem) {
                return index;
            }
            index = (index + 1) % elems.size();
        }
        return -1; // Если не найдено
    }

    void printHashTable() const {
        cout << "Hash table:" << endl;
        for (size_t i = 0; i < elems.size(); ++i) {
            for (const auto& lexeme : elems[i]) {
                cout << "Index: " << i << ", Value: " << lexeme.value << ", Type: " << lexeme.type << endl;
            }
        }
    }

    void remove(string& elem) {
        if (elems.empty()) {
            cout << "Table is empty!" << endl;
            return;
        }
        int index = hashFun(elem, elems.size());
        while (!elems[index].empty()) {
            if (!elems[index].empty() && elems[index][0].value == elem) {
                elems[index].clear();
                cout << "Element " << elem << " removed from index " << index << "." << endl;
                return;
            }
            index = (index + 1) % elems.size();
        }
        cout << "Element " << elem << " not found!" << endl;
    }

    void saveToFile() const {
        ofstream file(filename);
        if (!file.is_open()) throw runtime_error("Could not open file for writing: " + filename);
        for (const auto& chain : elems) {
            for (const auto& lexeme : chain) {
                file << lexeme.value << endl;
            }
        }
        file.close();
    }
};

class IdentifierTable : public HashTable {
public:
    IdentifierTable() : HashTable(30, 10) {}
};

class ConstantsTable : public HashTable {
public:
    ConstantsTable() : HashTable(40, 10) {}
};

class Scanner {
private:
    ifstream inputFile;
    ofstream errorFile;
    ofstream tokenFile; 
    KeyWordsTable& keywordsTable;
    DelimitersTable& delimitersTable;
    IdentifierTable& identifiersTable;
    ConstantsTable& constantsTable;

    void throwError(const string& message) {
        errorFile << message << endl;
        cout << "Error: " << message << endl;
    }

public:
    Scanner(const string& inputFilename, const string& errorFilename, const string& tokenFilename,
            KeyWordsTable& kwTable, DelimitersTable& delimTable,
            IdentifierTable& idTable, ConstantsTable& constTable)
        : keywordsTable(kwTable), delimitersTable(delimTable),
          identifiersTable(idTable), constantsTable(constTable) {
        inputFile.open(inputFilename);
        if (!inputFile.is_open()) throw runtime_error("Could not open input file: " + inputFilename);

        errorFile.open(errorFilename);
        if (!errorFile.is_open()) throw runtime_error("Could not open error file: " + errorFilename);

        tokenFile.open(tokenFilename);
        if (!tokenFile.is_open()) throw runtime_error("Could not open token file: " + tokenFilename);
    }

    ~Scanner() {
        inputFile.close();
        errorFile.close();
        tokenFile.close();
    }

    void scan() {
    char currentChar;
    string tokenValue;
    int lineNumber = 1; 
    string currentLine; // текущая строка для отслеживания ошибок

    while (inputFile.get(currentChar)) {
        if (currentChar == '\n') {
            lineNumber++;
            currentLine.clear();
            continue;
        }
        currentLine += currentChar;

        if (isspace(currentChar)) continue;

        switch (currentChar) {
            case '*':
            case '+':
            case '-':
            case '%': {
                // проверка на комбинированные операции (*=, +=, -=, %=)
                if (inputFile.peek() == '=') {
                    inputFile.get();
                    string combinedOp = string(1, currentChar) + "=";
                    
                    int index = delimitersTable.findIndexByValue(combinedOp);
                    tokenFile << "(" << 20 << ", " << index << ") " << combinedOp << endl;
                } else {
                    int index = delimitersTable.findIndexByValue(string(1, currentChar));
                    tokenFile << "(" << 20 << ", " << index << ") " << currentChar << endl;
                }
                break;
            }
            case '=': {
                if (inputFile.peek() == '=') {
                    inputFile.get();
                    int index = delimitersTable.findIndexByValue("==");
                    tokenFile << "(" << 20 << ", " << index << ") ==" << endl;
                } else {
                    int index = delimitersTable.findIndexByValue("=");
                    tokenFile << "(" << 20 << ", " << index << ") =" << endl;
                }
                break;
            }
            case '<': {
                if (inputFile.peek() == '<') {
                    inputFile.get();
                    int index = delimitersTable.findIndexByValue("<<");
                    tokenFile << "(" << 20 << ", " << index << ") <<" << endl;
                } else {
                    int index = delimitersTable.findIndexByValue("<");
                    tokenFile << "(" << 20 << ", " << index << ") <" << endl;
                }
                break;
            }
            case '>': {
                if (inputFile.peek() == '>') {
                    inputFile.get();
                    int index = delimitersTable.findIndexByValue(">>");
                    tokenFile << "(" << 20 << ", " << index << ") >>" << endl;
                } else {
                    int index = delimitersTable.findIndexByValue(">");
                    tokenFile << "(" << 20 << ", " << index << ") >" << endl;
                }
                break;
            }
            case '(': case ')': case ';': case '{': case '}': case '[': case ']': {
                // другие разделители () ; {} []
                int index = delimitersTable.findIndexByValue(string(1, currentChar));
                tokenFile << "(" << 20 << ", " << index << ") " << currentChar << endl;
                break;
            }
            case '/': {
                char nextChar = inputFile.peek(); 
                if (nextChar == '=') { 
                    inputFile.get(); 
                    string combinedOp = string(1, currentChar) + "="; 
                    
                    int index = delimitersTable.findIndexByValue(combinedOp);
                    tokenFile << "(" << 20 << ", " << index << ") " << combinedOp << endl;
                } else if (nextChar == '*') {
                    // многострочные комментарии
                    inputFile.get();
                    while (inputFile.get(currentChar)) {
                        currentLine += currentChar; 
                        if (currentChar == '*' && inputFile.peek() == '/') {
                            inputFile.get();
                            break;
                        }
                        if (currentChar == '\n') {
                            lineNumber++;
                            currentLine.clear();
                        }
                    }
                    if (!inputFile) {
                        throwError("Unclosed multi-line comment", lineNumber, currentLine);
                    }
                } else if (nextChar == '/') {
                    // однострочные комментарии
                    inputFile.ignore(numeric_limits<streamsize>::max(), '\n');
                    lineNumber++;
                    currentLine.clear();
                } else {
                    throwError("Unknown symbol: / (expected * or / for comment)", lineNumber, currentLine);
                }
                break;
            }
            default: {
                if (isalpha(currentChar)) {
                    tokenValue = "";
                    do {
                        tokenValue += currentChar;
                    } while (inputFile.get(currentChar) && (isalnum(currentChar) || currentChar == '_'));
                    inputFile.unget(); 

                    //"long long"
                    if (tokenValue == "long" && inputFile.peek() == ' ') {
                        string nextToken;
                        inputFile >> nextToken;
                        if (nextToken == "long") {
                            tokenValue += " " + nextToken;
                        } else {
                            inputFile.unget();
                        }
                    }

                    if (keywordsTable.searchByValue(tokenValue)) {
                        int index = keywordsTable.findIndexByValue(tokenValue);
                        tokenFile << "(" << 10 << ", " << index << ") " << tokenValue << endl;
                    } else {
                        string temp = tokenValue;
                        identifiersTable.add(temp, 30);
                        int index = identifiersTable.search(tokenValue);
                        tokenFile << "(" << 30 << ", " << index << ") " << tokenValue << endl;
                    }
                } else if (isdigit(currentChar)) {
                    tokenValue = "";
                    do {
                        tokenValue += currentChar;
                    } while (inputFile.get(currentChar) && isdigit(currentChar));

                    if (isalpha(currentChar) || currentChar == '_') {
                        throwError("Invalid character after number: " + string(1, currentChar), lineNumber, currentLine);
                    } else {
                        inputFile.unget();
                    }
                    string temp = tokenValue;
                    constantsTable.add(temp, 40);
                    int index = constantsTable.search(tokenValue);
                    tokenFile << "(" << 40 << ", " << index << ") " << tokenValue << endl;
                } else {
                    throwError("Unknown symbol: " + string(1, currentChar), lineNumber, currentLine);
                }
                break;
            }
        }
    }
}
    void throwError(const string& message, int lineNumber, const string& currentLine) {
        errorFile << "Error on line " << lineNumber << ": " << message << endl;
        cout << "Error on line " << lineNumber << ": " << message << endl;
    }
};

int main() {
    try {
        KeyWordsTable keywords;
        DelimitersTable delimiters;
        IdentifierTable identifiers;
        ConstantsTable constants;

        Scanner scanner("input.txt", "error.txt", "tokens.txt", keywords, delimiters, identifiers, constants);
        scanner.scan();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}