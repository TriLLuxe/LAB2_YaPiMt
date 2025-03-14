#include "KeyWordsTable.h"
#include "DelimitersTable.h"
#include "IdentifierTable.h"
#include "ConstantsTable.h"
#include "Scanner.h"
#include <iostream>

int main() {
    try {
        KeyWordsTable keywords;
        DelimitersTable delimiters;
        IdentifierTable identifiers;
        ConstantsTable constants;

        Scanner scanner("input.cpp", keywords, delimiters, identifiers, constants);
        scanner.scan();

        identifiers.saveToFile();
        constants.saveToFile();

        std::cout << "Scanning completed successfully." << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}