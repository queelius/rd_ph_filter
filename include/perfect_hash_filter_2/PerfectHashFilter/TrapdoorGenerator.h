#ifndef __TRAPDOOR_GENERATOR_H__
#define __TRAPDOOR_GENERATOR_H__

#include <vector>
#include <iostream>
#include "PackedArray.h"

namespace alex::encrypted_search
{
    class Trapdoor
    {
    public:

    private:
    };

    template <typename T>
    class TrapdoorCodec
    {
    public:
        void test()
        {
            alex::collections::PackedArray<T> pa;
            auto& x = pa[0];
        }

        typedef T value_type;
        typedef std::vector<bool> code_type;
        
        virtual T decode(const code_type& code) const = 0;
        virtual code_type encode(const T& value) const = 0;

        virtual void write(std::ostream& file) const = 0;
        virtual void read(std::istream& file) = 0;
    };
}

#endif