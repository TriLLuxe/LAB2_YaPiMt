#include "Scanner.h"
#include <cctype>

Scanner::Scanner(const std::string& inputFile, KeyWordsTable& kw, DelimitersTable& dl, IdentifierTable& id, ConstantsTable& cn)
    : input(inputFile), tokensFile("tokens.txt"), errorsFile("errors.txt"), keywords(kw), delimiters(dl), identifiers(id), constants(cn), line(1), column(1) {
    if (!input.is_open()) {
        throw std::runtime_error("Could not open input file: " + inputFile);
    }
    initDelimiters();
}

bool Scanner::isLetter(char c) const { return std::isalpha(c) || c == '_'; }
bool Scanner::isDigit(char c) const { return std::isdigit(c); }
bool Scanner::isWhitespace(char c) const { return std::isspace(c); }

void Scanner::initDelimiters() {
    for (size_t i = 0; i < delimiters.getTableSize(); i++) {
        const Lexeme& lexeme = delimiters.getLexeme(i);
        if (lexeme.value.length() == 1) {
            singleDelimiters.insert(lexeme.value);
        } else if (lexeme.value.length() == 2) {
            doubleDelimiters.insert(lexeme.value);
            potentialDoubleStarters.insert(lexeme.value[0]);
        }
    }
}

std::string Scanner::accumulateIdentifier(char first) {
    std::string lexeme = std::string(1, first);
    char c;
    while (input.get(c) && (isLetter(c) || isDigit(c))) {
        lexeme += c;
        column++;
    }
    if (input) input.putback(c);
    return lexeme;
}

std::string Scanner::accumulateConstant(char first) {
    std::string lexeme = std::string(1, first);
    char c;
    while (input.get(c) && isDigit(c)) {
        lexeme += c;
        column++;
    }
    if (input) input.putback(c);
    return lexeme;
}

void Scanner::emitToken(int type, int index) {
    tokensFile << type << " " << index << std::endl;
}

void Scanner::reportError(const std::string& message) {
    errorsFile << "Error at line " << line << ", column " << column << ": " << message << std::endl;
}

void Scanner::scan() {
    char c;
    while (input.get(c)) {
        if (c == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }

        if (isLetter(c) || c == '_') {
            std::string lexeme = accumulateIdentifier(c);
            int kwIndex = -1;
            for (size_t i = 0; i < keywords.getTableSize(); i++) {
                if (keywords.getLexeme(i).value == lexeme) {
                    kwIndex = i;
                    break;
                }
            }
            if (kwIndex >= 0) {
                emitToken(10, kwIndex);
            } else {
                int idIndex = -1;
                for (size_t i = 0; i < identifiers.getElemsSize(); i++) {
                    const auto& chain = identifiers.getChain(i);
                    for (const auto& lex : chain) {
                        if (lex.value == lexeme) {
                            idIndex = i;
                            break;
                        }
                    }
                    if (idIndex != -1) break;
                }
                if (idIndex == -1) {
                    std::string temp = lexeme;
                    identifiers.add(temp, 30);
                    idIndex = identifiers.computeHash(lexeme);
                }
                emitToken(30, idIndex);
            }
        } else if (isDigit(c)) {
            std::string lexeme = accumulateConstant(c);
            int constIndex = -1;
            for (size_t i = 0; i < constants.getElemsSize(); i++) {
                const auto& chain = constants.getChain(i);
                for (const auto& lex : chain) {
                    if (lex.value == lexeme) {
                        constIndex = i;
                        break;
                    }
                }
                if (constIndex != -1) break;
            }
            if (constIndex == -1) {
                std::string temp = lexeme;
                constants.add(temp, 40);
                constIndex = constants.computeHash(lexeme);
            }
            emitToken(40, constIndex);
        } else if (potentialDoubleStarters.count(c) > 0) {
            char d = input.peek();
            std::string cd = std::string(1, c) + d;
            int delimIndex = -1;
            for (size_t i = 0; i < delimiters.getTableSize(); i++) {
                if (delimiters.getLexeme(i).value == cd) {
                    delimIndex = i;
                    break;
                }
            }
            if (delimIndex >= 0) {
                input.get();
                column++;
                emitToken(20, delimIndex);
            } else {
                for (size_t i = 0; i < delimiters.getTableSize(); i++) {
                    if (delimiters.getLexeme(i).value == std::string(1, c)) {
                        delimIndex = i;
                        break;
                    }
                }
                if (delimIndex >= 0) {
                    emitToken(20, delimIndex);
                } else {
                    reportError("Invalid delimiter: " + std::string(1, c));
                }
            }
        } else if (singleDelimiters.count(std::string(1, c)) > 0) {
            int delimIndex = -1;
            for (size_t i = 0; i < delimiters.getTableSize(); i++) {
                if (delimiters.getLexeme(i).value == std::string(1, c)) {
                    delimIndex = i;
                    break;
                }
            }
            emitToken(20, delimIndex);
        } else if (isWhitespace(c)) {
            continue;
        } else {
            reportError("Invalid character: " + std::string(1, c));
        }
    }
}