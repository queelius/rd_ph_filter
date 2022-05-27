#include "Cmph.h"
#include <iostream>
#include <string>
#include <set>

void main()
{
    std::vector<std::string> keys;
    for (size_t i = 0; i < 1000; ++i)
    {
        keys.push_back(std::to_string(i));
    }

    alex::hash::CMPHa ph(keys, .8);
    std::cout << ph.load_factor() << std::endl;
}