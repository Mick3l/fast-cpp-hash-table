#include <iostream>

#include "hash.h"

int main() {
    mtl::hash_map<std::string, int> map(100);
    map.Insert("hello",4);
    std::cout << map["hello"];

    return 0;
}