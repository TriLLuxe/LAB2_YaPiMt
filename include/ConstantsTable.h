#ifndef CONSTANTS_TABLE_H
#define CONSTANTS_TABLE_H

#include "HashTable.h"

class ConstantsTable : public HashTable {
public:
    ConstantsTable() : HashTable(40, 10) {}
};

#endif