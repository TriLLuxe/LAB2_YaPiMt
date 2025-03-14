#ifndef SCANNER_H
#define SCANNER_H

#include "KeyWordsTable.h"
#include "DelimitersTable.h"
#include "IdentifierTable.h"
#include "ConstantsTable.h"
#include <fstream>
#include <set>

class Scanner {
private:
    std::ifstream input;
    std::ofstream tokensFile;
    std::ofstream errorsFile;
    KeyWordsTable& keywords;
    DelimitersTable& delimiters;
    IdentifierTable& identifiers;
    ConstantsTable& constants;
    int line;
    int column;
    std::set<char> potentialDoubleStarters;
    std::set<std::string> singleDelimiters;
    std::set<std::string> doubleDelimiters;

    bool isLetter(char c) const;
    bool isDigit(char c) const;
    bool isWhitespace(char c) const;

    void initDelimiters();
    std::string accumulateIdentifier(char first);
    std::string accumulateConstant(char first);
    void emitToken(int type, int index);
    void reportError(const std::string& message);

public:
    Scanner(const std::string& inputFile, KeyWordsTable& kw, DelimitersTable& dl, IdentifierTable& id, ConstantsTable& cn);
    void scan();
};

#endif