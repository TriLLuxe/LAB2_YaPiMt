#ifndef IDENTIFIER_TABLE_H
#define IDENTIFIER_TABLE_H

#include "HashTable.h"

class IdentifierTable : public HashTable {
public:
    IdentifierTable() : HashTable(30, 10) {}
};

#endif